#include "delete_task.h"

void delete_task(dpp::cluster& bot, const dpp::button_click_t& event) {

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

	const bool client_permissions = event.command.app_permissions.has(dpp::p_manage_channels, dpp::p_manage_roles);

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
	if (!client_permissions) 
	{
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(warn_color, warn_title, "Hmm, looks like I'm not allowed to manage channels and manage roles.")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	// if the task is not closed
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

	dpp::embed* embed = { new dpp::embed() };
	embed->set_color(dpp::colors::red);
    embed->set_description("The task will be deleted in a few seconds.");

	bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed), [&bot, event, source_member, config, cFind](const dpp::confirmation_callback_t& callback) -> void 
	{
		if (callback.is_error()) 
		{
			bot.log(dpp::loglevel::ll_error, callback.get_error().message);

			dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, "Callback error", callback.get_error().message)) };
			bot.interaction_followup_create(event.command.token, dpp::message(event.command.channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
			delete embed; embed = nullptr;

			return;
		}

		// thread sleep 1.5 sec
		Sleep(1500);

		// delete task
		bot.channel_delete(cFind->id, [&bot, config, cFind, source_member, event](const dpp::confirmation_callback_t& callback) -> void 
		{
			if (callback.is_error()) 
			{
				bot.log(dpp::loglevel::ll_error, callback.get_error().message);

				dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, "Callback error", callback.get_error().message)) };
				bot.interaction_followup_create(event.command.token, dpp::message(event.command.channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
				delete embed; embed = nullptr;

				return;
			}

			// task deletion from the database occurs in the on_channel_deleted event!

			// log
			if (config.log_channel_id != 0) 
			{
				dpp::channel* log_channel = dpp::find_channel(config.log_channel_id);

				if (log_channel == nullptr)
					return;

				dpp::embed* log_embed = { new dpp::embed() };
				log_embed->set_color(dpp::colors::red);
				log_embed->set_description("Task **" + cFind->name + "** deleted by " + source_member.get_mention());

				bot.message_create(dpp::message(log_channel->id, *log_embed));

				delete log_embed; log_embed = nullptr;
			}
		}); 
	});

	delete embed; embed = nullptr;
}