#include "create_task.h"

void create_task(dpp::cluster& bot, const dpp::button_click_t& event) {

	const std::string exception_title = "Exception";
	const std::string warn_title = "Warning";
	const std::string complete_title = "Complete";
	const uint32_t warn_color = dpp::colors::yellow;
	const uint32_t ex_color = dpp::colors::red;
	const uint32_t complete_color = dpp::colors::green;

	const dpp::snowflake channel_id = event.command.channel_id;
	const dpp::snowflake guild_id = event.command.guild_id;
	const dpp::channel* cFind = dpp::find_channel(channel_id);
	const dpp::guild* gFind = dpp::find_guild(guild_id);
	const dpp::guild_member source_member = event.command.member;

	const bool bot_permissions = event.command.app_permissions.has(dpp::p_manage_channels, dpp::p_manage_roles);

	event.thinking(true);

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

	// task channel creation
	dpp::channel* task = { new dpp::channel() };
	task->set_name("task-" + std::to_string(config.count_tasks));
	task->set_guild_id(guild_id);

	if (config.open_category_id != 0) 
	{
		dpp::channel* category = dpp::find_channel(config.open_category_id);

		if (category != nullptr)
		{
			task->set_parent_id(category->id);		
		}
		else 
		{
			// error message sending
			dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, exception_title, "Configuration error, unable to define category ID for open tasks.")) };
			bot.interaction_followup_create(event.command.token, dpp::message(cFind->id, *embed).set_flags(dpp::m_ephemeral), 0);

			delete embed; embed = nullptr;
		}
	}

	task->set_type(dpp::channel_type::CHANNEL_TEXT);

	// taking away the ability to watch the channel (everyone)
	task->set_permission_overwrite(gFind->roles.front(), dpp::overwrite_type::ot_role, 0, dpp::permissions::p_view_channel);

	// adding the ability for support_members to view the channel
	for (auto iter{ support_roles.begin() }; iter != support_roles.end(); iter++) 
	{
		task->set_permission_overwrite(*iter, dpp::overwrite_type::ot_role, dpp::permissions::p_view_channel, 0);
	}
	
	// create channel
	bot.channel_create(*task, [&bot, source_member, config, channel_id, event](const dpp::confirmation_callback_t& callback) -> void 
	{
		if (callback.is_error()) 
		{
			bot.log(dpp::loglevel::ll_error, callback.get_error().message);

			dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, "Callback error", callback.get_error().message)) };
			bot.interaction_followup_create(event.command.token, dpp::message(event.command.channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
			delete embed; embed = nullptr;

			return;
		}

		auto task = callback.get<dpp::channel>();
		
		// add a task to the database
		if (!database_manager::add_task(task.id, task.parent_id, event.command.guild_id, source_member.user_id)) 
		{
			dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, "Exception", "Failed to add a task to the database.")) };
			bot.interaction_followup_create(event.command.token, dpp::message(event.command.channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
			delete embed; embed = nullptr;

			return;
		}
			
		// welcome message
		dpp::embed* embed = { new dpp::embed() };
		embed->set_color(0x9246FF);
		embed->set_title("New task!");
		embed->set_description("Click on the button to close the task.");
		embed->set_author("Taskbox", bot.me.get_url(), bot.me.get_avatar_url());

		dpp::message* msg = { new dpp::message(task.id, *embed) };
		msg->set_content(source_member.get_mention() + " has opened a new task!");
		msg->add_component(
			dpp::component().add_component(
				dpp::component()
				.set_label("Close")
				.set_type(dpp::cot_button)
				.set_style(dpp::cos_danger)
				.set_id("close_task")
			));

		bot.message_create(*msg); 

		delete embed; embed = nullptr;
		delete msg; msg = nullptr;

		// updating the number of created tasks
		database_manager::update_count_tasks(event.command.guild_id);

		dpp::embed* complete_embed = { new dpp::embed(embed_manager::create_embed(complete_color, "Complete", "The task " + task.get_mention() + " has been successfully created!")) };
		bot.interaction_followup_create(event.command.token, dpp::message(event.command.channel_id, *complete_embed).set_flags(dpp::m_ephemeral), 0);

		delete complete_embed; complete_embed = nullptr;

		// log
		if (config.log_channel_id != 0) 
		{
			dpp::channel* log_channel = dpp::find_channel(config.log_channel_id);

			if (log_channel == nullptr) {
				dpp::embed* embed = {new dpp::embed(embed_manager::create_embed(ex_color, "Exception", "Failed to write a log to the log channel."))};
				bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);

				delete embed; embed = nullptr;

				return;
			}

			dpp::embed* log_embed = { new dpp::embed() };
			log_embed->set_color(dpp::colors::green);
			log_embed->set_description("Task " + task.get_mention() + " created by " + source_member.get_mention());

			bot.message_create(dpp::message(log_channel->id, *log_embed));

			delete log_embed; log_embed = nullptr;
		}
	});
}