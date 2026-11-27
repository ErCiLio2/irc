/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eteixeir <eteixeir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 11:00:32 by eteixeir          #+#    #+#             */
/*   Updated: 2026/11/27 03:26:51 by eteixeir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Bot.hpp"

int ft_atoi(const char *str) {
	std::stringstream ss;
	ss << str;
	int val;
	ss >> val;
	if (!ss.eof() || ss.fail() || ss.bad())
		return (-1);
	return (val);
}

Bot::Bot(std::string password, int port) : password(password), port(port), fd(-1)
{
}

Bot::~Bot()
{
	if (fd != -1)
		close(fd);
}

void Bot::init()
{
	struct sockaddr_in	in;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		throw std::runtime_error("Error creating socket");
	in.sin_family = AF_INET;
	in.sin_port = htons(port);
	in.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (connect(fd, (struct sockaddr *)&in, sizeof(in)) < 0)
		throw std::runtime_error("Error connecting to server");
	std::string auth = "PASS " + password + "\r\n"
		+ "NICK bot\r\n"
		+ "USER bot 0 * :bot\r\n"
		+ "JOIN #bot\r\n";
	send(fd, auth.c_str(), auth.size(), 0);
}

void Bot::run()
{
	char	buf[1024];

	while (true)
	{
		int n = recv(fd, buf, sizeof(buf) - 1, 0);
		if (n <= 0)
			break ;
		buf[n] = '\0';
		std::string msg(buf);
		if (msg.find("PRIVMSG #bot :!ping") != std::string::npos)
		{
			std::string reply = "PRIVMSG #bot :Pong!\r\n";
			send(fd, reply.c_str(), reply.size(), 0);
		}
		else if (msg.find("PRIVMSG #bot :!help") != std::string::npos)
		{
			std::string reply = "PRIVMSG #bot :Comandos disponiveis: !ping, !help\r\n";
			send(fd, reply.c_str(), reply.size(), 0);
		}
	}
}
