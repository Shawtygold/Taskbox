#pragma once
#include <dpp/dpp.h>

class embed_manager {
public:	
	static dpp::embed create_embed(const uint32_t color, const std::string title, const std::string description);
};