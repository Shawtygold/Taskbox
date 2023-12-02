#include "close_task.h"

void close_task(dpp::cluster& bot, const dpp::button_click_t& event) {

	const std::string exception_title = "Exception";
	const std::string warn_title = "Warning";
	const std::string complete_title = "Complete";
	const uint32_t warn_color = dpp::colors::yellow;
	const uint32_t ex_color = dpp::colors::red;
	const uint32_t complete_color = dpp::colors::green;

	const dpp::snowflake channel_id = event.command.channel_id;
	const dpp::snowflake guild_id = event.command.guild_id;
	dpp::channel* cFind = dpp::find_channel(channel_id);
	const dpp::guild* gFind = dpp::find_guild(guild_id);
	const dpp::guild_member source_member = event.command.member;

	const bool bot_permissions = event.command.app_permissions.has(dpp::p_manage_channels, dpp::p_manage_roles);

	event.reply();	
	
	// if the guild is not found
	if (gFind == nullptr) 
	{
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, exception_title, "Something went wrong. I couldn't find the guild.")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	// if the channel is not found
	if (cFind == nullptr) 
	{
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, exception_title, "Something went wrong. I couldn't find this channel.")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	// if bot does not have permissions
	if (!bot_permissions) 
	{
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(warn_color, warn_title, "Hmm, looks like I'm not allowed to manage channels and manage roles.")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	// if the task is archived
	if (database_manager::find_archived_task(channel_id)) 
	{
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(warn_color, warn_title, "This channel has already been closed.")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	// if the task is not in the database
	if (!database_manager::find_task(channel_id)) 
	{
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, exception_title, "Failed to find the current task in the database.")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	// configure existence check
	if (!database_manager::find_config(guild_id)) 
	{
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(warn_color, warn_title, "You haven't set the configuration yet. To do so, use the ``/setconfig`` command")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	config config = database_manager::get_config(guild_id);

	std::vector<unsigned __int64> support_roles = config.support_roles;
	if (support_roles.size() == 0) 
	{
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, exception_title, "I couldn't get support roles. Please check if the config is correct or contact the developer.")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	bool sm_have_role = false;

	for (auto iter{ support_roles.begin() }; iter != support_roles.end(); iter++) 
	{
		// if source member roles contains support role or source member is administrator
		if (std::find(source_member.get_roles().begin(), source_member.get_roles().end(), *iter) != source_member.get_roles().end() || cFind->get_user_permissions(source_member).can(dpp::p_administrator)) {
			sm_have_role = true;
			break;
		}
	}

	// if source member does not have support role
	if (!sm_have_role) 
	{
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(warn_color, warn_title, "You do not have sufficient permissions to use this panel.")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	// rename
	cFind->set_name(cFind->name.replace(0, 4, "closed"));

	// moving a task to another category
	if (config.closed_category_id != 0) 
	{
		dpp::channel* category = dpp::find_channel(config.closed_category_id);

		if (category != nullptr) {

			cFind->set_position(category->id);
			cFind->set_parent_id(category->id);
		}
		else
		{
			// error message sending
			dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, exception_title, "Configuration error, unable to define category ID for closed tasks.")) };
			bot.interaction_followup_create(event.command.token, dpp::message(cFind->id, *embed).set_flags(dpp::m_ephemeral), 0);
			delete embed; embed = nullptr;
		}
	}

	// edit channel
	bot.channel_edit(*cFind, [&bot, event](const dpp::confirmation_callback_t& callback) -> void 
	{
		if (callback.is_error()) {
			bot.log(dpp::loglevel::ll_error, callback.get_error().message);
			std::string err_msg = callback.get_error().message;

			if (err_msg.find("The resource is being rate limited") == std::string::npos) {
				dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, "Callback error", callback.get_error().message)) };
				bot.interaction_followup_create(event.command.token, dpp::message(event.command.channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
				delete embed; embed = nullptr;

				return;
			}
		}
	});

	unsigned __int64 creator_id = database_manager::get_task_creator_id(cFind->id);

	// adding a task to the archive task database
	if (!database_manager::archive_task(cFind->id, cFind->parent_id, guild_id, creator_id)) 
	{
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, exception_title, "Failed to archive the task.")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	// deleting a task from the tasks database
	if (!database_manager::delete_task(cFind->id)) 
	{
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, exception_title, "Failed to delete the task from the database.")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	dpp::embed* embed = { new dpp::embed()};
	embed->set_color(dpp::colors::red);
	embed->set_description("Task closed by " + source_member.get_mention());

	bot.message_create(dpp::message(event.command.channel_id, *embed));

	delete embed; embed = nullptr;

	dpp::embed* moderation_panel = { new dpp::embed() };
	moderation_panel->set_description("```Support team controls```");
	moderation_panel->set_color(0x2b2d31);

	dpp::message* msg = { new dpp::message(cFind->id, *moderation_panel) };

	msg->add_component(
		dpp::component().add_component(
			dpp::component()
			.set_label("Open")
			.set_emoji(dpp::unicode_emoji::unlock)
			.set_type(dpp::cot_button)
			.set_style(dpp::cos_secondary)
			.set_id("open_task"))
		.add_component(
			dpp::component()
			.set_label("Delete")
			.set_emoji(dpp::unicode_emoji::no_entry)
			.set_type(dpp::cot_button)
			.set_style(dpp::cos_secondary)
			.set_id("delete_task")
		));

	bot.message_create(*msg);

	delete moderation_panel; moderation_panel = nullptr;
	delete msg; msg = nullptr;

	// log
	if (config.log_channel_id != 0) 
	{
		dpp::channel* log_channel = dpp::find_channel(config.log_channel_id);

		if (log_channel == nullptr) 
		{
			dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, "Exception", "Failed to write a log to the log channel."))};
			bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);

			delete embed; embed = nullptr;

			return;
		}

		dpp::embed* log_embed = {new dpp::embed()};
		log_embed->set_color(dpp::colors::red);
		log_embed->set_description("Task " + cFind->get_mention() + " closed by " + source_member.get_mention());

		bot.message_create(dpp::message(log_channel->id, *log_embed));

		delete log_embed; log_embed = nullptr;
	}
}