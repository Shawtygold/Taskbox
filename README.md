<div align="center"><img src="https://github.com/Shawtygold/Taskbox/blob/master/DiscordBotIcon.png" width="200" height="200"/>
<h1>Taskbox Discord Bot</1>
</div>

## :pencil: About this project

A Discord bot built with C++ using the D++ library. The primary functionality is creating Discord channels as tasks for tracking purposes. It can be used for more convenient organisation of employees' work on the Discord server. Additionally, the bot includes moderation commands such as `/ban`, `/kick`, and `/timeout`.

## 📖 Functional
### How to work with a bot?
- Create a configuration `/setconfig` command
- Send the interaction panel to any discord channel `/sendpanel` command
- Create a task by clicking on the button
- Discuss the task with the supports (their roles are specified in the configuration)
- Close the task using the message in the task channel
- Delete a task

### Additional functionality
- The bot implements the function of logging interactions with tasks. To view logs, you need to specify the logging channel in the configuration (`/setconfig` command)
- `/ban`, `/kick` and `/timeout` commands for naughty users

## 📚 Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository.
2. Create a new branch (`git checkout -b feature/your-feature`).
3. Commit your changes (`git commit -m 'Add your feature'`).
4. Push to the branch (`git push origin feature/your-feature`).
5. Create a pull request.

## 📄 License

Distributed under the MIT License. See `LICENSE` for more information.
