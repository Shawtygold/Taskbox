#pragma once

#include <dpp/dpp.h>
#include <dpp/unicode_emoji.h>
#include "embed_manager.h"
#include "database_manager.h"


void send_panel(dpp::cluster& bot, const dpp::slashcommand_t& event);
