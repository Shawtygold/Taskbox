#pragma once

#include <iostream>
#include <dpp/dpp.h>
#include <dpp/unicode_emoji.h>
#include "ban.h"
#include "kick.h"
#include "timeout.h"
#include "setconfig.h"
#include "sendpanel.h"
#include "create_task.h"
#include "close_task.h"
#include "open_task.h"
#include "delete_task.h"

class Bot {

public:
	Bot(const std::string& token);
	void start();

private:

	dpp::cluster bot;

	void setup_bot();
	void register_commands();
	void command_handler(const dpp::slashcommand_t& event);
	void on_ready(const dpp::ready_t& event);
	void on_button_click(const dpp::button_click_t& event);
	void on_channel_delete(const dpp::channel_delete_t& event);
};