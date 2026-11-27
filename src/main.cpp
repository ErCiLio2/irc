/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eteixeir <eteixeir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 13:32:32 by eteixeir          #+#    #+#             */
/*   Updated: 2026/11/27 03:28:39 by eteixeir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <csignal>
#include <cstdlib>
#include "../inc/Server.hpp"

# define N_ARGUMENTS 3

static bool	parsePort(const std::string &str, int &port)
{
	if (str.empty() || str.size() > 5)
		return (false);
	for (size_t i = 0; i < str.size(); i++)
	{
		if (!std::isdigit(static_cast<unsigned char>(str[i])))
			return (false);
	}
	port = ft_atoi(str.c_str());
	return (port >= 1 && port <= 65535);
}
static bool isPasswordEmpty(const std::string& password)
{
    if (password.empty()) {
        return true;
    }

    for (std::string::const_iterator it = password.begin(); it != password.end(); ++it) {
        if (!std::isspace(static_cast<unsigned char>(*it))) {
            return false;
        }
    }
    return true;
}

int main(int ac, char *argv[]) {

	int	port;

	if (ac != N_ARGUMENTS) {
		std::cerr << "Error: Invalid argument(s)"<< std::endl;
		std::cerr << "Try: ./ircserv <port> <password>" << std::endl;
		return (EXIT_FAILURE);
	}
	if (!parsePort(argv[1], port)) {
		std::cerr << "Error: port must be a number between 1 and 65535" << std::endl;
		return (EXIT_FAILURE);
	}
	if (isPasswordEmpty(argv[2])) {
		std::cerr << "Error: password cannot be empty" << std::endl;
		return (EXIT_FAILURE);
	}

	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, Server::signalHandler);
	signal(SIGQUIT, Server::signalHandler);

	try {
		Server a(argv[2], port);
		a.init();
		a.run();
	}
	catch (const std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
