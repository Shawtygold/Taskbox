#pragma once

struct task 
{
public:
	unsigned __int64 id;
	unsigned __int64 channel_id;
	unsigned __int64 parent_channel_id;
	unsigned __int64 guild_id;
	unsigned __int64 creator_id;

	task();
};