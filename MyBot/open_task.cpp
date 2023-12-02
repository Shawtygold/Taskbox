#include "open_task.h"

void open_task(dpp::cluster& bot, const dpp::button_click_t& event) {

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

	// if the task is open
	if (database_manager::find_task(cFind->id)) 
	{
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(warn_color, warn_title, "This task is open.")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	// if the task could not be found in the task archive
	if (!database_manager::find_archived_task(channel_id)) 
	{
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, exception_title, "Failed to find a closed task in the database.")) };
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

	// rename
	cFind->set_name(cFind->name.replace(0, 6, "task"));

	// moving a task to another category
	if (config.open_category_id != 0) 
	{
		dpp::channel* category = dpp::find_channel(config.open_category_id);

		if (category != nullptr) 
		{
			cFind->set_position(category->id);
			cFind->set_parent_id(category->id);
		}
		else 
		{
			// error message sending
			dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, exception_title, "Configuration error, unable to define category ID for open tasks.")) };
			bot.interaction_followup_create(event.command.token, dpp::message(cFind->id, *embed).set_flags(dpp::m_ephemeral), 0);

			delete embed; embed = nullptr;
		}
	}

	// edit channel
	bot.channel_edit(*cFind, [&bot, event](const dpp::confirmation_callback_t& callback) -> void
	{
		if (callback.is_error()) {
			bot.log(dpp::loglevel::ll_error, callback.get_error().message);

			dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, "Callback error", callback.get_error().message)) };
			bot.interaction_followup_create(event.command.token, dpp::message(event.command.channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
			delete embed; embed = nullptr;

			return;
		}
	});

	unsigned __int64 creator_id = database_manager::get_archived_task_creator_id(cFind->id);

	// adding a task to the database
	if (!database_manager::add_task(cFind->id, cFind->parent_id, guild_id, creator_id)) 
	{
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, exception_title, "Failed to add a task to the database.")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	// deleting a task from the task archive
	if (!database_manager::delete_archived_task(cFind->id)) 
	{
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, exception_title, "Failed to delete the archived task from the database.")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	//// event reply
	//bot.interaction_followup_create(event.command.token, dpp::message(channel_id, "Task successfully opened!").set_flags(dpp::m_ephemeral), 0);

	dpp::embed* embed = { new dpp::embed() };
	embed->set_color(dpp::colors::green);
	embed->set_description("Task opened by " + source_member.get_mention());

	bot.message_create(dpp::message(event.command.channel_id, *embed));

	delete embed; embed = nullptr;

	bot.messages_get(cFind->id, cFind->last_message_id, 0, 0, 100, [&bot, config, cFind, source_member, event](const dpp::confirmation_callback_t& callback) -> void 
	{
		if (callback.is_error()) 
		{
			bot.log(dpp::loglevel::ll_error, callback.get_error().message);

			dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, "Callback error", callback.get_error().message)) };
			bot.interaction_followup_create(event.command.token, dpp::message(cFind->id, *embed).set_flags(dpp::m_ephemeral), 0);
			delete embed; embed = nullptr;

			return;
		}

		dpp::message_map map = callback.get<dpp::message_map>();

		// deleting "Support team controls" message 
		for (auto& iter : map) 
		{		
			if (iter.second.embeds.size() != 0) {
				std::vector<dpp::embed> embeds = iter.second.embeds;

				if (embeds[0].description.find("Support team controls") != std::string::npos) {
						bot.message_delete(iter.first, cFind->id);
				}
			}		
		}
	});

	// log
	if (config.log_channel_id != 0) 
	{
		dpp::channel* log_channel = dpp::find_channel(config.log_channel_id);

		if (log_channel == nullptr) 
		{
			dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, "Exception", "Failed to write a log to the log channel.")) };
			bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);

			delete embed; embed = nullptr;

			return;
		}

		dpp::embed* log_embed = { new dpp::embed() };
		log_embed->set_color(dpp::colors::green);
		log_embed->set_description("Task " + cFind->get_mention() + " opened by " + source_member.get_mention());

		bot.message_create(dpp::message(log_channel->id, *log_embed));

		delete log_embed; log_embed = nullptr;
	}
}