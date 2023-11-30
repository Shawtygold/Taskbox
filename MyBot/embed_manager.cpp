#include "embed_manager.h"

dpp::embed embed_manager::create_embed(const uint32_t color, const std::string title, const std::string description) {
	dpp::embed embed = dpp::embed()
		.set_color(color)
		.set_title(title)
		.set_description(description);

	return embed;
}