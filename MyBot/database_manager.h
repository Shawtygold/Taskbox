#pragma once
#include <stdio.h>
#include "sqlite3.h"
#include <iostream>
#include <string>
#include <vector>
#include "config.h"
#include <iostream>
#include <sstream>

class database_manager {
public:
	static bool create_db();
	static bool create_tables();

	//configs
	static bool add_config(unsigned __int64 open_category_id, unsigned __int64 closed_category_id, unsigned __int64 log_channel_id, unsigned __int64 guild_id, unsigned __int64 support_role1, unsigned __int64 support_role2, unsigned __int64 support_role3, unsigned __int64 support_role4, unsigned __int64 support_role5);
	static config get_config(unsigned __int64 guild_id);
	static bool find_config(unsigned __int64 guild_id);
	static bool delete_config(unsigned __int64 guild_id);

	//tasks
	static bool add_task(unsigned __int64 channel_id, unsigned __int64 parent_channel_id, unsigned __int64 guild_id, unsigned __int64 creator_id);
	static bool delete_task(unsigned __int64 channel_id);
	static bool find_task(unsigned __int64 channel_id);
	static unsigned __int64 get_task_creator_id(unsigned __int64 channel_id);
	static void update_count_tasks(unsigned __int64 guild_id);

	//task archive
	static bool archive_task(unsigned __int64 channel_id, unsigned __int64 parent_channel_id, unsigned __int64 guild_id, unsigned __int64 creator_id);
	static bool delete_archived_task(unsigned __int64 channel_id);
	static bool find_archived_task(unsigned __int64 channel_id);
	static unsigned __int64 get_archived_task_creator_id(unsigned __int64 channel_id);
};
