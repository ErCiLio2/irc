/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eteixeir <eteixeir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 10:54:16 by eteixeir          #+#    #+#             */
/*   Updated: 2026/11/23 16:07:22 by eteixeir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOT_HP
# define BOT_HP
# include <sstream>
# include <arpa/inet.h>
# include <iostream>
# include <netinet/in.h>
# include <stdexcept>
# include <stdlib.h>
# include <string>
# include <sys/socket.h>
# include <unistd.h>

class Bot
{
  private:
	std::string password;
	int port;
	int fd;
	
  public:
	Bot(std::string password, int port);
	~Bot();
	void init();
	void run();
};
int ft_atoi(const char *str);
#endif
