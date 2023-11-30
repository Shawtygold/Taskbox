#include "bot.h"

Bot::Bot(const std::string& token) : bot(token, dpp::i_all_intents)
{
	bot.on_log(dpp::utility::cout_logger());
}

void Bot::start()
{
	setup_bot();
	bot.start(dpp::st_wait);
}


void Bot::setup_bot() 
{
	bot.on_slashcommand([this](const dpp::slashcommand_t& event) {
		command_handler(event);
	});

	bot.on_ready([this](const dpp::ready_t& event) {
		on_ready(event);
	});	

	bot.on_button_click([this](const dpp::button_click_t& event) {
		on_button_click(event);
	});

	bot.on_channel_delete([this](const dpp::channel_delete_t& event) {
		on_channel_delete(event);
	});
}

void Bot::register_commands()
{
	// ban command
	dpp::slashcommand ban_command("ban", "Ban a user.", bot.me.id);
	ban_command.add_option(dpp::command_option(dpp::co_user, "member", "Member to ban.", true));
	ban_command.add_option(dpp::command_option(dpp::co_string, "reason", "The reason for the ban."));

	// kick command
	dpp::slashcommand kick_command("kick", "Kick a user.", bot.me.id);
	kick_command.add_option(dpp::command_option(dpp::co_user, "member", "Member to kick.", true));
	kick_command.add_option(dpp::command_option(dpp::co_string, "reason", "The reason for the kick."));

	// timeout command
	dpp::slashcommand timeout_command("timeout", "Timout a user.", bot.me.id);
	timeout_command.add_option(dpp::command_option(dpp::co_user, "member", "Member to timeout.", true));
	timeout_command.add_option(dpp::command_option(dpp::co_string, "duration", "Duration.", true)
		.add_choice(dpp::command_option_choice("1 min", "60"))
		.add_choice(dpp::command_option_choice("5 min", "300"))
		.add_choice(dpp::command_option_choice("10 min", "600"))
		.add_choice(dpp::command_option_choice("30 min", "1800"))
		.add_choice(dpp::command_option_choice("1 hour", "3600"))
		.add_choice(dpp::command_option_choice("2 hour", "7200"))
		.add_choice(dpp::command_option_choice("5 hour", "18000"))
		.add_choice(dpp::command_option_choice("10 hour", "36000"))
		.add_choice(dpp::command_option_choice("1 day", "86400"))
		.add_choice(dpp::command_option_choice("1 week", "604800"))
		.add_choice(dpp::command_option_choice("2 week", "1209600"))
		.add_choice(dpp::command_option_choice("3 week", "1814400")));
	timeout_command.add_option(dpp::command_option(dpp::co_string, "reason", "The reason for the timeout.", false));

	// set config command
	dpp::slashcommand setconfig_command("setconfig", "Sets the task configuration.", bot.me.id);
	setconfig_command.add_option(dpp::command_option(dpp::co_role, "support_role1", "The role of which the task will be accessed.", true));
	setconfig_command.add_option(dpp::command_option(dpp::co_channel, "open_task_category", "Category where open tasks will be placed.", false));
	setconfig_command.add_option(dpp::command_option(dpp::co_channel, "closed_task_category", "Category where closed tasks will be placed.", false));
	setconfig_command.add_option(dpp::command_option(dpp::co_channel, "log_channel", "Channel where logs will be sent.", false));
	setconfig_command.add_option(dpp::command_option(dpp::co_role, "support_role2", "The role of which the task will be accessed.", false));
	setconfig_command.add_option(dpp::command_option(dpp::co_role, "support_role3", "The role of which the task will be accessed.", false));
	setconfig_command.add_option(dpp::command_option(dpp::co_role, "support_role4", "The role of which the task will be accessed.", false));
	setconfig_command.add_option(dpp::command_option(dpp::co_role, "support_role5", "The role of which the task will be accessed.", false));

	// send panel command
	dpp::slashcommand sendpanel_command("sendpanel", "Sends a panel to this channel.", bot.me.id);

	// create commands
	bot.global_bulk_command_create(std::vector{ ban_command, kick_command, timeout_command, setconfig_command, sendpanel_command });
}

void Bot::on_ready(const dpp::ready_t& event) 
{
	if (dpp::run_once<struct register_bot_commands>()) {
		register_commands();
	}
}

void Bot::command_handler(const dpp::slashcommand_t& event) 
{
	if (event.command.get_command_name() == "ban") {
		ban(bot, event);
	}
	else if (event.command.get_command_name() == "kick") {
		kick(bot, event);
	}
	else if (event.command.get_command_name() == "timeout") {
		timeout(bot, event);
	}
	else if (event.command.get_command_name() == "setconfig") {
		setconfig(bot, event);
	}
	else if (event.command.get_command_name() == "sendpanel") {
		send_panel(bot, event);
	}
}

void Bot::on_button_click(const dpp::button_click_t& event) 
{
	if (event.custom_id == "create_task") {
		create_task(bot, event);
	}
	else if (event.custom_id == "close_task") {
		close_task(bot, event);
	}
	else if (event.custom_id == "open_task") {
		open_task(bot, event);
	}
	else if (event.custom_id == "delete_task") {
		delete_task(bot, event);
	}
}

void Bot::on_channel_delete(const dpp::channel_delete_t& event)
{
	if (database_manager::find_task(event.deleted->id)) {
		database_manager::delete_task(event.deleted->id);
	}
	else if (database_manager::find_archived_task(event.deleted->id)) {
		database_manager::delete_archived_task(event.deleted->id);
	}
}
