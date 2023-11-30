#include "kick.h"

void kick(dpp::cluster& bot, const dpp::slashcommand_t& event) {

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
	const dpp::guild* gFind = dpp::find_guild(guild_id);
	const dpp::channel* cFind = dpp::find_channel(channel_id);
	const dpp::guild_member member_to_kick = event.command.get_resolved_member(user_id);
	const dpp::guild_member source_member = event.command.member;

	const bool botPermissions = event.command.app_permissions.has(dpp::p_kick_members);

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

	// if the user was not found on the server
	if (gFind->members.find(user_id) == gFind->members.end()) {
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(warn_color, warn_title, "Hmm, can't find the user on the server!")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	// if member who invoked this command does not have kick members permission
	if (!cFind->get_user_permissions(source_member).can(dpp::p_kick_members)) {
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(warn_color, warn_title, "You do not have sufficient permissions to use this command!")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	// if bot does not have kick members permission
	if (!botPermissions) {
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(warn_color, warn_title, "Hmm, looks like I'm not allowed to kick users.")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	if (source_member == member_to_kick) {
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(warn_color, warn_title, "You can't kick yourself.")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	if (user_id == bot.me.id) {
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(warn_color, warn_title, "Why do you kick me :(")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	// set audit reason
	bot.set_audit_reason(reason);

	// kick member
	bot.guild_member_kick(guild_id, user_id, [&bot, event, member_to_kick](const dpp::confirmation_callback_t& callback) -> void {
		if (callback.is_error()) {
			bot.log(dpp::loglevel::ll_error, callback.get_error().message);

			dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, "Exception", callback.get_error().message)) };
			bot.interaction_followup_create(event.command.token, dpp::message(event.command.channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
			delete embed; embed = nullptr;

			return;
		}

		std::string kick_message = member_to_kick.get_mention() + " has been kicked!";

		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(complete_color, "Complete", kick_message)) };
		bot.interaction_followup_create(event.command.token, dpp::message(event.command.channel_id, *embed).set_flags(dpp::m_ephemeral), 0);

		delete embed; embed = nullptr;
	});
}