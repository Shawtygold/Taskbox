<div align="center"><img src="https://github.com/Shawtygold/Taskbox/blob/master/DiscordBotIcon.png" width="200" height="200"/>
<h1>Taskbox Discord Bot</1>
</div>

### :pencil: About this project
- This project was created by [shawtygold](https://github.com/Shawtygold).
- This repository is my course project and perhaps it will be updated as new ideas become available :)

### 📚 Libraries and version
These are libraries that we're using in this project:
- [Dpp](https://github.com/brainboxdotcc/DPP) - version `10.0.22`
- [Sqlite](https://www.sqlite.org/cintro.html)
- [Template](https://github.com/brainboxdotcc/windows-bot-template)

Bot current version: 
- Version - `1.1`

### 📖 Functional
#### How to work with a bot?
- Create a configuration `/setconfig` command
- Send the interaction panel to any discord channel `/sendpanel` command
- Create a task by clicking on the button
- Discuss the task with the supports (their roles are specified in the configuration)
- Close the task using the message in the task channel
- Delete a task

#### Additional functionality
- The bot implements the function of logging interactions with tasks. To view logs, you need to specify the logging channel in the configuration (`/setconfig` command)
- The bot also contains a `/ban`, `/kick` and `/timeout` commands

 ### 📜 Some notes
 **This project supports:**
 - Microsoft Windows 10-11 `x64`
 - Visual Studio platform `x64`
 - C++ 20


### 💿 Download and using
 - `sqlite3.c` file has been added to gitignore, to make the bot work you need to download [archive](https://www.sqlite.org/2023/sqlite-amalgamation-3440200.zip) with sqlite files and add `sqlite3.c` to the project. Then call the `database::create_tables` method in `Bot::on_ready` event. After the first run, remove `database::create_tables` from the event.

 ### 🤝 Support me
 - Please leave me a star if you like it. Thank you very much!
