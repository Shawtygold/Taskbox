#include "information.h"

void info(dpp::cluster& bot, const dpp::slashcommand_t& event) {

	const std::string exception_title = "Exception";
	const std::string warn_title = "Warning";
	const std::string complete_title = "Complete";
	const uint32_t warn_color = 0xFFD619;
	const uint32_t ex_color = 0xF23F42;
	const uint32_t complete_color = 0x23FD6F;


	const dpp::snowflake guild_id = event.command.guild_id;
	const dpp::snowflake channel_id = event.command.channel_id;
	const dpp::guild* gFind = dpp::find_guild(event.command.guild_id);
	const dpp::channel* cFind = dpp::find_channel(channel_id);

	const bool botPermissions = event.command.app_permissions.has(dpp::p_send_messages);

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

	if (!botPermissions) {
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(warn_color, warn_title, "Hmm, looks like I'm not allowed to send messages")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	dpp::embed* embed = { new dpp::embed() };
	embed->set_color(0x9246FF);
	embed->set_title("Taskbox Stats");
	embed->add_field("\n", "");
	embed->add_field("Version", "Taskbox 1.1\nC++ 20\nDPP 10.0.22", true);
	embed->add_field("", "", true);
	embed->add_field("Developer", "shawtygold\nshawtygoldz@gmail.com", true);
	embed->add_field("\n", "");
	embed->set_footer(dpp::embed_footer().set_icon(bot.me.get_avatar_url()).set_text("Taskbox - prod. by shawtygold"));

	bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed), [&bot, event, cFind](const dpp::confirmation_callback_t& callback) -> void
	{
			if (callback.is_error()) {
				if (callback.is_error())
				{
					bot.log(dpp::loglevel::ll_error, callback.get_error().message);

					dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(ex_color, "Callback error", callback.get_error().message)) };
					bot.interaction_followup_create(event.command.token, dpp::message(event.command.channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
					delete embed; embed = nullptr;

					return;
				}
			}
	});

	delete embed; embed = nullptr;
}