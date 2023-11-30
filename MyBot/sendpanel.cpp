#include "sendpanel.h"

void send_panel(dpp::cluster& bot, const dpp::slashcommand_t& event) {

	const std::string exception_title = "Exception";
	const std::string warn_title = "Warning";
	const std::string complete_title = "Complete";
	const uint32_t warn_color = dpp::colors::yellow;
	const uint32_t ex_color = dpp::colors::red;
	const uint32_t complete_color = dpp::colors::green;

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

	// configure existence check
	if (!database_manager::find_config(guild_id)) 
	{
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(warn_color, warn_title, "You haven't set the configuration yet. To do so, use the ``/setconfig`` command")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	config config = database_manager::get_config(guild_id);

	std::vector<unsigned __int64> support_roles{config.support_roles};

	std::vector<dpp::snowflake> roles = source_member.get_roles();

	bool has_role = false;

	// find support role in source member roles
	for (auto iter{ support_roles.begin() }; iter != support_roles.end(); iter++) 
	{
		if (std::find(roles.begin(), roles.end(), *iter) != roles.end()) {
			has_role = true;
			break;
		}
	}

	if (!has_role && !cFind->get_user_permissions(source_member).can(dpp::permissions::p_administrator))
	{
		dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(warn_color, warn_title, "You do not have sufficient permissions to use this command!")) };
		bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed).set_flags(dpp::m_ephemeral), 0);
		delete embed; embed = nullptr;

		return;
	}

	dpp::embed* panel = { new dpp::embed() };
	panel->set_color(0x9246FF);
	panel->set_title("Tasks");
	panel->set_description("To create a task, click on the button");
	panel->set_footer(dpp::embed_footer().set_text("Taskbox").set_icon(bot.me.get_avatar_url()));

	// add button to the message
	dpp::message* msg = { new dpp::message(channel_id, *panel) };
	msg->add_component(
		dpp::component().add_component(
			dpp::component()
			.set_label("Create task")
			.set_type(dpp::cot_button)
			.set_emoji(dpp::unicode_emoji::envelope_with_arrow)
			.set_style(dpp::cos_primary)
			.set_id("create_task")
		));

	bot.message_create(*msg);

	dpp::embed* embed = { new dpp::embed(embed_manager::create_embed(complete_color, complete_title, "Panel has been successfully sent!"))};
	//event reply
	bot.interaction_followup_create(event.command.token, dpp::message(channel_id, *embed), 0);

	delete panel; panel = nullptr;
	delete msg; msg = nullptr;
	delete embed; embed = nullptr;

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
		log_embed->set_color(0x9246FF);
		log_embed->set_description("Panel has been successfully sent!");

		bot.message_create(dpp::message(log_channel->id, *log_embed));

		delete log_embed; log_embed = nullptr;
	}
}