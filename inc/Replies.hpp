/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Replies.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eteixeir <eteixeir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/23 09:39:59 by eteixeir          #+#    #+#             */
/*   Updated: 2026/11/27 03:32:19 by eteixeir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REPLIES_HPP
# define REPLIES_HPP

# include <string>
# include <sstream>
# include <iomanip>
# include <map>
# include <vector>
# include <set>
# include <poll.h>
#include <algorithm>
#include <cctype>
# include <cerrno>
# include <cstring>
# include <unistd.h>
# include <fcntl.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <stdexcept>
# include <stdlib.h>
# include <signal.h>

#	define SERVER_NAME "irc.webserver.c++98"

enum NumericReplies {
	/* ERROS */
	ERR_NOSUCHNICK        = 401,
	ERR_NOSUCHCHANNEL     = 403,
	ERR_CANNOTSENDTOCHAN  = 404,
	ERR_NOORIGIN          = 409,
	ERR_NORECIPIENT       = 411,
	ERR_NOTEXTTOSEND      = 412,
	ERR_UNKNOWNCOMMAND    = 421,
	ERR_NOMOTD            = 422,
	ERR_NONICKNAMEGIVEN   = 431,
	ERR_ERRONEUSNICK      = 432,
	ERR_NICKNAMEINUSE     = 433,
	ERR_USERNOTINCHANNEL  = 441,
	ERR_NOTONCHANNEL      = 442,
	ERR_USERONCHANNEL     = 443,
	ERR_NOTREGISTERED     = 451,
	ERR_NEEDMOREPARAMS    = 461,
	ERR_ALREADYREGISTRED  = 462,
	ERR_PASSWDMISMATCH    = 464,
	ERR_KEYSET            = 467,
	ERR_CHANNELISFULL     = 471,
	ERR_UNKNOWNMODE       = 472,
	ERR_INVITEONLYCHAN    = 473,
	ERR_BADCHANNELKEY     = 475,
	ERR_CHANOPRIVSNEEDED  = 482,

	/* INFORMATIVOS */
	RPL_WELCOME           = 1,
	RPL_YOURHOST          = 2,
	RPL_CREATED           = 3,
	RPL_MYINFO            = 4,

	RPL_CHANNELMODEIS     = 324,
	RPL_NOTOPIC           = 331,
	RPL_TOPIC             = 332,
	RPL_INVITING          = 341,
	RPL_NAMREPLY          = 353,
	RPL_ENDOFNAMES        = 366,

	RPL_MOTD              = 372,
	RPL_MOTDSTART         = 375,
	RPL_ENDOFMOTD         = 376
};

std::string formatNumericReply(NumericReplies code, const std::string &target,
	const std::string &arg1 = "", const std::string &arg2 = "");

#endif
