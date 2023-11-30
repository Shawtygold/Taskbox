#include "database_manager.h"

unsigned __int64 convert_to_ui64(const unsigned char* value);

bool database_manager::create_db() {
	sqlite3* db;

	int result = sqlite3_open("test.db", &db);

	if (result != SQLITE_OK) {
		return false;
	}

	return true;
}

bool database_manager::create_tables() {

	sqlite3* db;

	char* err_msg = 0;
	int rc = sqlite3_open("test.db", &db);
	if (rc != SQLITE_OK)
	{
		sqlite3_close(db);
		return false;
	}

	const char* sql1 = "DROP TABLE IF EXISTS tasks;"
		"CREATE TABLE tasks(id INTEGER PRIMARY KEY AUTOINCREMENT, channel_id TEXT, parent_channel_id TEXT, guild_id TEXT, creator_id TEXT);";

	const char* sql2 = "DROP TABLE IF EXISTS task_archive;"
		"CREATE TABLE task_archive(id INTEGER PRIMARY KEY AUTOINCREMENT, channel_id TEXT, parent_channel_id TEXT, guild_id TEXT, creator_id TEXT);";

	const char* sql3 = "DROP TABLE IF EXISTS configs;"
		"CREATE TABLE configs(id INTEGER PRIMARY KEY AUTOINCREMENT, open_category_id TEXT, closed_category_id TEXT, log_channel_id TEXT, count_tasks TEXT, guild_id TEXT, support_role1 TEXT, support_role2 TEXT, support_role3 TEXT, support_role4 TEXT, support_role5 TEXT);";

	rc = sqlite3_exec(db, sql1, 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		sqlite3_close(db);
		return false;
	}

	rc = sqlite3_exec(db, sql2, 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		sqlite3_close(db);
		return false;
	}

	rc = sqlite3_exec(db, sql3, 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		sqlite3_close(db);
		return false;
	}

	sqlite3_close(db);
	return true;
}


bool database_manager::add_config(unsigned __int64 open_category_id, unsigned __int64 closed_category_id, unsigned __int64 log_channel_id, unsigned __int64 guild_id, unsigned __int64 support_role1, unsigned __int64 support_role2, unsigned __int64 support_role3, unsigned __int64 support_role4, unsigned __int64 support_role5) {
	
	bool result = false;

	sqlite3* db;
	sqlite3_stmt* res;

	// find config
	if (find_config(guild_id)) {

		// delete config
		if (!delete_config(guild_id)) {
			return false;
		}
	}

	if (sqlite3_open("test.db", &db) != SQLITE_OK)
	{
		sqlite3_close(db);
		return 1;
	}

	const char* sql = "INSERT INTO configs (open_category_id, closed_category_id, log_channel_id, count_tasks, guild_id, support_role1, support_role2, support_role3, support_role4, support_role5) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
	
	if (sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK)
	{
		// convert to const char*
		std::string val1 = std::to_string(open_category_id);
		const char* value1 = val1.data();

		// convert to const char*
		std::string val2 = std::to_string(closed_category_id);
		const char* value2 = val2.data();

		// convert to const char*
		std::string val3 = std::to_string(log_channel_id);
		const char* value3 = val3.data();

		// convert to const char*
		std::string val4 = std::to_string(0);
		const char* value4 = val4.data();

		// convert to const char*
		std::string val5 = std::to_string(guild_id);
		const char* value5= val5.data();

		// convert to const char*
		std::string val6 = std::to_string(support_role1);
		const char* value6 = val6.data();

		// convert to const char*
		std::string val7 = std::to_string(support_role2);
		const char* value7 = val7.data();

		// convert to const char*
		std::string val8 = std::to_string(support_role3);
		const char* value8 = val8.data();

		// convert to const char*
		std::string val9 = std::to_string(support_role4);
		const char* value9 = val9.data();

		// convert to const char*
		std::string val10 = std::to_string(support_role5);
		const char* value10 = val10.data();

		// bind parameters
		sqlite3_bind_text(res, 1, value1, -1, SQLITE_STATIC);
		sqlite3_bind_text(res, 2, value2, -1, SQLITE_STATIC);
		sqlite3_bind_text(res, 3, value3, -1, SQLITE_STATIC);
		sqlite3_bind_text(res, 4, value4, -1, SQLITE_STATIC);
		sqlite3_bind_text(res, 5, value5, -1, SQLITE_STATIC);
		sqlite3_bind_text(res, 6, value6, -1, SQLITE_STATIC);
		sqlite3_bind_text(res, 7, value7, -1, SQLITE_STATIC);
		sqlite3_bind_text(res, 8, value8, -1, SQLITE_STATIC);
		sqlite3_bind_text(res, 9, value9, -1, SQLITE_STATIC);
		sqlite3_bind_text(res, 10, value10, -1, SQLITE_STATIC);

		// step
		int step = sqlite3_step(res);

		if (step == SQLITE_DONE)
			result = true;
	}
	else
	{
		fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_finalize(res);
	sqlite3_close(db);
	return result;
}

bool database_manager::delete_config(unsigned __int64 guild_id) {

	bool result = false;

	sqlite3* db;
	sqlite3_stmt* res;

	if (sqlite3_open("test.db", &db) != SQLITE_OK) {
		sqlite3_close(db);
		return false;
	}

	const char* sql = "DELETE FROM configs WHERE guild_id=@guild_id";

	if (sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK) {

		// convert guild id to const char*
		std::string val = std::to_string(guild_id);
		const char* value = val.data();

		sqlite3_bind_text(res, 1, value, -1, SQLITE_STATIC);

		if (sqlite3_step(res) == SQLITE_DONE)
			result = true;
	}
	else
	{
		fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_close(db);
	sqlite3_finalize(res);
	return result;
	
}

config database_manager::get_config(unsigned __int64 guild_id) {

	config config{};

	sqlite3* db;  
	sqlite3_stmt* res;

	if (sqlite3_open("test.db", &db) != SQLITE_OK)
	{
		sqlite3_close(db);
		return config;
	}

	const char* sql = "SELECT * FROM configs WHERE guild_id=@guild_id";

	if (sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK)
	{
		// convert guild_id to const char*
		std::string val = std::to_string(guild_id);
		const char* value = val.data();

		sqlite3_bind_text(res, 1, value, -1, SQLITE_STATIC);

		if (sqlite3_step(res) == SQLITE_ROW)
		{
			// id
			unsigned __int64 id = { convert_to_ui64(sqlite3_column_text(res, 0)) };

			// open_category_id
			unsigned __int64 open_category_id = { convert_to_ui64(sqlite3_column_text(res, 1)) };

			// closed_category_id
			unsigned __int64 closed_category_id = { convert_to_ui64(sqlite3_column_text(res, 2)) };

			// log_channel_id
			unsigned __int64 log_channel_id = { convert_to_ui64(sqlite3_column_text(res, 3)) };

			// count_tasks
			unsigned __int64 count_tasks = { convert_to_ui64(sqlite3_column_text(res, 4)) };

			// guild_id
			unsigned __int64 guild_id = { convert_to_ui64(sqlite3_column_text(res, 5)) };

			// support_role 1
			unsigned __int64 support_role_1 = { convert_to_ui64(sqlite3_column_text(res, 6)) };
			
			// support_role 2
			unsigned __int64 support_role_2 = { convert_to_ui64(sqlite3_column_text(res, 7)) };

			// support_role 3
			unsigned __int64 support_role_3 = { convert_to_ui64(sqlite3_column_text(res, 8)) };

			// support_role 4
			unsigned __int64 support_role_4 = { convert_to_ui64(sqlite3_column_text(res, 9)) };

			// support_role 5
			unsigned __int64 support_role_5 = { convert_to_ui64(sqlite3_column_text(res, 10)) };

			config.id = id;
			config.open_category_id = open_category_id;
			config.closed_category_id = closed_category_id;
			config.log_channel_id = log_channel_id;
			config.count_tasks = count_tasks;
			config.guild_id = guild_id;

			std::vector<unsigned __int64> support_roles{};

			if (support_role_1 != 0)
				support_roles.push_back(support_role_1);
			if (support_role_2 != 0)
				support_roles.push_back(support_role_2);
			if (support_role_3 != 0)
				support_roles.push_back(support_role_3);
			if (support_role_4 != 0)
				support_roles.push_back(support_role_4);
			if (support_role_5 != 0)
				support_roles.push_back(support_role_5);

			config.support_roles = support_roles;
		}
	}
	else
	{
		fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_finalize(res);
	sqlite3_close(db);
	return config;
}

bool database_manager::find_config(unsigned __int64 guild_id) {

	bool exist = false;

	sqlite3* db;
	sqlite3_stmt* res;

	if (sqlite3_open("test.db", &db) != SQLITE_OK) {
		sqlite3_close(db);
		return false;
	}
	
	const char* sql = "SELECT * FROM configs WHERE guild_id=@guild_id";

	if (sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK) {

		// convert guild_id to const char* 
		std::string val = std::to_string(guild_id);
		const char* value = val.data();

		sqlite3_bind_text(res, 1, value, -1, SQLITE_STATIC);

		if (sqlite3_step(res) == SQLITE_ROW)
			exist = true;
	}
	else
	{
		fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_finalize(res);
	sqlite3_close(db);
	return exist;
}

bool database_manager::find_task(unsigned __int64 channel_id) {

	bool exist = false;

	sqlite3* db;
	sqlite3_stmt* res;

	if (sqlite3_open("test.db", &db) != SQLITE_OK) {
		sqlite3_close(db);
		return false;
	}

	const char* sql = "SELECT * FROM tasks WHERE channel_id=@channel_id";

	if (sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK) {

		// convert channel_id to const char*
		std::string val = std::to_string(channel_id);
		const char* value = val.data();

		sqlite3_bind_text(res, 1, value, -1, SQLITE_STATIC);

		if (sqlite3_step(res) == SQLITE_ROW)
			exist = true;
	}
	else
	{
		fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_finalize(res);
	sqlite3_close(db);
	return exist;
}

bool database_manager::add_task(unsigned __int64 channel_id, unsigned __int64 parent_channel_id, unsigned __int64 guild_id, unsigned __int64 creator_id) {

	bool result = false;

	sqlite3* db;
	sqlite3_stmt* res;

	if (sqlite3_open("test.db", &db) != SQLITE_OK) {
		sqlite3_close(db);
		return false;
	}

	const char* sql = "INSERT INTO tasks (channel_id, parent_channel_id, guild_id, creator_id) VALUES (?, ?, ?, ?)";

	if (sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK) {

		std::string val = std::to_string(channel_id);
		const char* value = val.data();

		std::string val1 = std::to_string(parent_channel_id);
		const char* value1 = val1.data();

		std::string val2 = std::to_string(guild_id);
		const char* value2 = val2.data();

		std::string val3 = std::to_string(creator_id);
		const char* value3 = val3.data();

		// bind parameters
		sqlite3_bind_text(res, 1, value, -1, SQLITE_STATIC);
		sqlite3_bind_text(res, 2, value1, -1, SQLITE_STATIC);
		sqlite3_bind_text(res, 3, value2, -1, SQLITE_STATIC);
		sqlite3_bind_text(res, 4, value3, -1, SQLITE_STATIC);

		if (sqlite3_step(res) == SQLITE_DONE)
			result = true;
	}
	else
	{
		fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_finalize(res);
	sqlite3_close(db);
	return result;
}

bool database_manager::delete_task(unsigned __int64 channel_id) {

	bool result = false;

	sqlite3* db;
	sqlite3_stmt* res;

	if (sqlite3_open("test.db", &db) != SQLITE_OK) {
		sqlite3_close(db);
		return false;
	}

	const char* sql = "DELETE FROM tasks WHERE channel_id=@channel_id";

	if (sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK) {

		// convert channel_id to const char*
		std::string val = std::to_string(channel_id);
		const char* value = val.data();

		sqlite3_bind_text(res, 1, value, -1, SQLITE_STATIC);

		if (sqlite3_step(res) == SQLITE_DONE)
			result = true;
	}
	else
	{
		fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_finalize(res);
	sqlite3_close(db);
	return result;
}

unsigned __int64 database_manager::get_task_creator_id(unsigned __int64 channel_id) {

	unsigned __int64 creator_id = 0;

	sqlite3* db;
	sqlite3_stmt* res;

	if (sqlite3_open("test.db", &db) != SQLITE_OK) {
		sqlite3_close(db);
		return 0;
	}

	const char* sql = "SELECT creator_id FROM tasks WHERE channel_id=@channel_id";

	if (sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK) {

		// convert channel_id to const char*
		std::string val = std::to_string(channel_id);
		const char* value = val.data();

		sqlite3_bind_text(res, 1, value, -1, SQLITE_STATIC);

		if (sqlite3_step(res) == SQLITE_ROW) {
			creator_id = { convert_to_ui64(sqlite3_column_text(res, 0)) };
		}
	}
	else {
		fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_close(db);
	sqlite3_finalize(res);
	return creator_id;
}

void database_manager::update_count_tasks(unsigned __int64 guild_id) {

	unsigned __int64 count_tasks{};

    // convert guild_id to const char*
	std::string val = std::to_string(guild_id);
	const char* guild_id1 = val.data();

	sqlite3* db;
	sqlite3_stmt* res;

	if (sqlite3_open("test.db", &db) != SQLITE_OK) {
		sqlite3_close(db);
		return;
	}

	// SELECT
	const char* sql = "SELECT count_tasks FROM configs WHERE guild_id=@guild_id";

	if (sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK) {

		sqlite3_bind_text(res, 1, guild_id1, -1, SQLITE_STATIC);

		if (sqlite3_step(res) == SQLITE_ROW) {

			// Increment count_tasks
			count_tasks = { convert_to_ui64(sqlite3_column_text(res, 0)) };
			count_tasks++;
		}
	}
	else {
		fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(res);
		sqlite3_close(db);
		return;
	}

	sqlite3_finalize(res);


	sqlite3_stmt* res1;

	//UPDATE
	const char* sql1 = "UPDATE configs SET count_tasks =@count_tasks WHERE guild_id=@guild_id";

	if (sqlite3_prepare_v2(db, sql1, -1, &res1, 0) == SQLITE_OK) {

		// convert count_tasks to const char*
		std::string val1 = std::to_string(count_tasks);
		const char* count_tasks1 = val1.data();

		sqlite3_bind_text(res1, 1, count_tasks1, -1, SQLITE_STATIC);
		sqlite3_bind_text(res1, 2, guild_id1, -1, SQLITE_STATIC);

		if (sqlite3_step(res1) != SQLITE_DONE)
			fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
	}
	else 
	{
		fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_close(db);
	sqlite3_finalize(res1);
}


bool database_manager::find_archived_task(unsigned __int64 channel_id) {

	bool exist = false;

	sqlite3* db;
	sqlite3_stmt* res;

	if (sqlite3_open("test.db", &db) != SQLITE_OK) {
		sqlite3_close(db);
		return false;
	}

	const char* sql = "SELECT * FROM task_archive WHERE channel_id=@channel_id";

	if (sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK) {

		// convert channel_id to const char*
		std::string val = std::to_string(channel_id);
		const char* value = val.data();

		sqlite3_bind_text(res, 1, value, -1, SQLITE_STATIC);

		if (sqlite3_step(res) == SQLITE_ROW)
			exist = true;
	}
	else
	{
		fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_finalize(res);
	sqlite3_close(db);
	return exist;
}

bool database_manager::archive_task(unsigned __int64 channel_id, unsigned __int64 parent_channel_id, unsigned __int64 guild_id, unsigned __int64 creator_id) {

	bool result = false;

	sqlite3* db;
	sqlite3_stmt* res;

	if (sqlite3_open("test.db", &db) != SQLITE_OK) {
		sqlite3_close(db);
		return false;
	}

	const char* sql = "INSERT INTO task_archive (channel_id, parent_channel_id, guild_id, creator_id) VALUES (?, ?, ?, ?)";

	if (sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK) {

		// convert to const char*
		std::string val = std::to_string(channel_id);
		const char* value = val.data();

		// convert to const char*
		std::string val1 = std::to_string(parent_channel_id);
		const char* value1 = val1.data();

		// convert to const char*
		std::string val2 = std::to_string(guild_id);
		const char* value2 = val2.data();

		// convert to const char*
		std::string val3 = std::to_string(creator_id);
		const char* value3 = val3.data();

		// bind parameters
		sqlite3_bind_text(res, 1, value, -1, SQLITE_STATIC);
		sqlite3_bind_text(res, 2, value1, -1, SQLITE_STATIC);
		sqlite3_bind_text(res, 3, value2, -1, SQLITE_STATIC);
		sqlite3_bind_text(res, 4, value3, -1, SQLITE_STATIC);

		if (sqlite3_step(res) == SQLITE_DONE)
			result = true;
	}
	else
	{
		fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_finalize(res);
	sqlite3_close(db);
	return result;
}

bool database_manager::delete_archived_task(unsigned __int64 channel_id) {

	bool result = false;

	sqlite3* db;
	sqlite3_stmt* res;

	if (sqlite3_open("test.db", &db) != SQLITE_OK) {
		sqlite3_close(db);
		return false;
	}

	const char* sql = "DELETE FROM task_archive WHERE channel_id=@channel_id";

	if (sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK) {

		// convert channel_id to const char*
		std::string val = std::to_string(channel_id);
		const char* value = val.data();

		sqlite3_bind_text(res, 1, value, -1, SQLITE_STATIC);

		if (sqlite3_step(res) == SQLITE_DONE)
			result = true;
	}
	else
	{
		fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_finalize(res);
	sqlite3_close(db);
	return result;
}

unsigned __int64 database_manager::get_archived_task_creator_id(unsigned __int64 channel_id) {

	unsigned __int64 creator_id = 0;

	sqlite3* db;
	sqlite3_stmt* res;

	if (sqlite3_open("test.db", &db) != SQLITE_OK) {
		sqlite3_close(db);
		return 0;
	}

	const char* sql = "SELECT creator_id FROM task_archive WHERE channel_id=@channel_id";

	if (sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK) {

		// convert channel_id to const char*
		std::string val = std::to_string(channel_id);
		const char* value = val.data();

		sqlite3_bind_text(res, 1, value, -1, SQLITE_STATIC);

		if (sqlite3_step(res) == SQLITE_ROW) {
			creator_id = { convert_to_ui64(sqlite3_column_text(res, 0)) };
		}
	}
	else {
		fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_close(db);
	sqlite3_finalize(res);
	return creator_id;
}

unsigned __int64 convert_to_ui64(const unsigned char* value) {

	std::stringstream strValue;
	strValue << value;
	unsigned __int64 result;
	strValue >> result;

	return result;
}