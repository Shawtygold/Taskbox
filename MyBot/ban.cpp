#include "ban.h"

void ban(dpp::cluster& bot, const dpp::slashcommand_t& event) {

	const std::string exception_title = "<:failed:1036206712916553748> Error";
	const std::string warn_title = "Warning";
	const std::string complete_title = "Complete";
	const uint32_t warn_color = 0xFFD619;
	const uint32_t ex_color = 0xF23F42;
	const uint32_t complete_color = 0x23FD6F;

	const dpp::snowflake user_id = std::get<dpp::snowflake>(event.get_parameter("member"));
	std::string reason; try { reason = std::get<std::string>(event.get_parameter("reason")); }catch (...) { reason = "No reason provided."; }

	const dpp::snowflake guild_id = event.command.guild_id;
	const dpp::snowflake channel_id = event.command.channel_id;
	const dpp::guild* gFind = dpp::find_guild(event.command.guild_id);
	const dpp::channel* cFind = dpp::find_channel(channel_id);
	const dpp::guild_member source_member = event.command.member;
	const dpp::guild_member member_to_ban = event.command.get_resolved_member(user_id);

	const bool botPermissions = event.command.app_permissions.has(dpp::p_ban_members);

	event.thinking(true);

	// if the guild is not found
	if (gFind == nullptr) {
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, exception_title, "Something went wrong. I couldn't find the guild.")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	// if the channel is not found
	if (cFind == nullptr) {
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, exception_title, "Something went wrong. I couldn't find this channel.")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	// if user was not found on the server
	if (gFind->members.find(user_id) == gFind->members.end()) {
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(warn_color, warn_title, "Hmm, can't find the user on the server!")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}	

	// if member who invoked this command does not have ban members permission
	if (!cFind->get_user_permissions(source_member).can(dpp::p_ban_members)) {
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(warn_color, warn_title, "You do not have sufficient permissions to use this command!")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}	

	// if bot does not have ban members permission
	if (!botPermissions) {
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(warn_color, warn_title, "Hmm, looks like I'm not allowed to ban users.")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	if (source_member == member_to_ban) {
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(warn_color, warn_title, "You can't ban yourself.")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	if (user_id == bot.me.id) {
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(warn_color, warn_title, "Why do you ban me :(")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	// set audit reason
	bot.set_audit_reason(reason);

	// ban member
	bot.guild_ban_add(guild_id, user_id, 0, [&bot, event, member_to_ban, channel_id](const dpp::confirmation_callback_t& callback) -> void 
	{
		if (callback.is_error()) {
			bot.log(dpp::loglevel::ll_error, callback.get_error().message);

			dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, "Exception", callback.get_error().message)) };
			bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
			delete embed; embed = nullptr;

			return;
		}

		std::string ban_message = member_to_ban.get_mention() + " has been banned!";

		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(complete_color, "Complete", ban_message)) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);

		delete embed; embed = nullptr;
	}); 
}

