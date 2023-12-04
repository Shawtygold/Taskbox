#include "setconfig.h"

void setconfig(dpp::cluster& bot, const dpp::slashcommand_t& event) {

	const std::string exception_title = "Exception";
	const std::string warn_title = "Warning";
	const std::string complete_title = "Complete";
	const uint32_t warn_color = dpp::colors::yellow;
	const uint32_t ex_color = dpp::colors::red;
	const uint32_t complete_color = dpp::colors::green;
	
	dpp::snowflake open_task_category_id; try { open_task_category_id = std::get<dpp::snowflake>(event.get_parameter("open_task_category")); } catch (...) { open_task_category_id = 0; };
	dpp::snowflake closed_task_category_id; try { closed_task_category_id = std::get<dpp::snowflake>(event.get_parameter("closed_task_category")); } catch (...) { closed_task_category_id = 0; }
	dpp::snowflake log_channel_id; try { log_channel_id = std::get<dpp::snowflake>(event.get_parameter("log_channel")); } catch (...) { log_channel_id = 0; }
	const dpp::snowflake support_role1 = std::get<dpp::snowflake>(event.get_parameter("support_role1"));
	dpp::snowflake support_role2; try { support_role2 = std::get<dpp::snowflake>(event.get_parameter("support_role2")); } catch (...) { support_role2 = 0; }
	dpp::snowflake support_role3; try { support_role3 = std::get<dpp::snowflake>(event.get_parameter("support_role3")); } catch (...) { support_role3 = 0; }
	dpp::snowflake support_role4; try { support_role4 = std::get<dpp::snowflake>(event.get_parameter("support_role4")); } catch (...) { support_role4 = 0; }
	dpp::snowflake support_role5; try { support_role5 = std::get<dpp::snowflake>(event.get_parameter("support_role5")); } catch (...) { support_role5 = 0; }

	const dpp::snowflake guild_id = event.command.guild_id;
	const dpp::snowflake channel_id = event.command.channel_id;
	const dpp::guild* gFind = dpp::find_guild(guild_id);
	const dpp::channel* cFind = dpp::find_channel(channel_id);
	const dpp::guild_member source_member = event.command.member;

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

	// if source member does not have manage guild permission
	if (!cFind->get_user_permissions(source_member).can(dpp::permissions::p_manage_guild))
	{
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(warn_color, warn_title, "In order to use this command you must have manage guild permission.")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	// add config to the database
	if (!database_manager::add_config(open_task_category_id, closed_task_category_id, log_channel_id, guild_id, support_role1, support_role2, support_role3, support_role4, support_role5))
	{
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, exception_title, "Something went wrong. Couldn't add the config to the database.")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	dpp::embed* complete_embed = {new dpp::embed(embed_manager::create_embed(complete_color, complete_title, "Configuration successfully registered!")) };
	bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *complete_embed).set_flags(dpp::m_ephemeral), 0);

	delete complete_embed; complete_embed = nullptr;

	// log
	if (log_channel_id != 0) 
	{
		dpp::channel* log_channel = dpp::find_channel(log_channel_id);

		if (log_channel == nullptr)
		{
			dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, "Exception", "Failed to write a log to the log channel.")) };
			bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);

			delete embed; embed = nullptr;

			return;
		}

		dpp::embed* log_embed = { new dpp::embed() };
		log_embed->set_color(0x9246FF);
		log_embed->set_description("Configuration successfully registered by " + source_member.get_mention() + "!");

		bot.message_create(dpp::message(log_channel->id, *log_embed));

		delete log_embed; log_embed = nullptr;
	}
}