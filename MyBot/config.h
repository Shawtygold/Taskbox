#pragma once
#include <dpp/dpp.h>

struct config {
public:
	config();
	config(unsigned __int64 id, unsigned __int64 open_category_id, unsigned __int64 closed_category_id, unsigned __int64 log_channel_id, unsigned __int64 count_tasks, unsigned __int64 guild_id, std::vector<unsigned __int64> support_roles);
	unsigned __int64 id;
	unsigned __int64 open_category_id;
	unsigned __int64 closed_category_id;
	unsigned __int64 log_channel_id;
	unsigned __int64 count_tasks;
	unsigned __int64 guild_id;
	unsigned __int64 task_channels_type = 0;
	std::vector<unsigned __int64> support_roles;
};