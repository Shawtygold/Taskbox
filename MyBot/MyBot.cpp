#include "MyBot.h"
#include "bot.h"

/* Be sure to place your token in the line below.
 * Follow steps here to get a token:
 * https://dpp.dev/creating-a-bot-application.html
 * When you invite the bot, be sure to invite it with the 
 * scopes 'bot' and 'applications.commands', e.g.
 * https://discord.com/oauth2/authorize?client_id=940762342495518720&scope=bot+applications.commands&permissions=139586816064
 */

bool read_token(std::string& tokenStr, const std::string path);
std::string BOT_TOKEN;

int main()
{
	if (!read_token(BOT_TOKEN, "C:\\Users\\user\\Documents\\file.txt")) {
		std::cout << "Token was not readed!" << std::endl;
		return -1;
	}

	Bot bot(BOT_TOKEN);
	bot.start();

	return 0;
}

bool read_token(std::string& tokenStr, const std::string path) {
	std::ifstream ifstr(path);
	if (!ifstr.is_open()) {
		std::cout << "File with token is not opened!" << std::endl;
		return false;
	}
	
	std::getline(ifstr, tokenStr);
	ifstr.close();
	return true;
}
