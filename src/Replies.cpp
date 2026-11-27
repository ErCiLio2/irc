/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Replies.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eteixeir <eteixeir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/23 09:40:26 by eteixeir          #+#    #+#             */
/*   Updated: 2026/11/27 03:29:17 by eteixeir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Replies.hpp"

std::string formatNumericReply(NumericReplies code, const std::string &target,
	const std::string &arg1, const std::string &arg2)
{
	std::ostringstream oss;
	
	std::string nick = target.empty() ? "*" : target;

	oss << ":" << SERVER_NAME << " "
		<< std::setw(3) << std::setfill('0') << static_cast<int>(code)
		<< " " << nick;

	switch (code)
	{
		// --- INFORMATIVOS (RPL) ---
		case RPL_WELCOME:
			oss << " :Welcome to the Internet Relay Network " << arg1;
			break;
		case RPL_YOURHOST:
			oss << " :Your host is " << SERVER_NAME << ", running version 1.0";
			break;
		case RPL_CREATED:
			oss << " :This server was created " << arg1;
			break;
		case RPL_MYINFO:
			oss << " " << SERVER_NAME << " 1.0 o itkol";
			break;
		case RPL_CHANNELMODEIS:
			oss << " " << arg1 << " " << arg2;
			break;
		case RPL_NOTOPIC:
			oss << " " << arg1 << " :No topic is set";
			break;
		case RPL_TOPIC:
			oss << " " << arg1 << " :" << arg2;
			break;
		case RPL_INVITING:
			oss << " " << arg1 << " " << arg2;
			break;
		case RPL_NAMREPLY:
			oss << " = " << arg1 << " :" << arg2;
			break;
		case RPL_ENDOFNAMES:
			oss << " " << arg1 << " :End of /NAMES list";
			break;
		case RPL_MOTD:
			oss << " :- " << arg1;
			break;
		case RPL_MOTDSTART:
			oss << " :- " << SERVER_NAME << " Message of the day - ";
			break;
		case RPL_ENDOFMOTD:
			oss << " :End of /MOTD command";
			break;

		// --- ERROS (ERR) ---
		case ERR_NOSUCHNICK:
			oss << " " << arg1 << " :No such nick/channel";
			break;
		case ERR_NOSUCHCHANNEL:
			oss << " " << arg1 << " :No such channel";
			break;
		case ERR_CANNOTSENDTOCHAN:
			oss << " " << arg1 << " :Cannot send to channel";
			break;
		case ERR_NOORIGIN:
			oss << " :No origin specified";
			break;
		case ERR_NORECIPIENT:
			oss << " :No recipient given (" << arg1 << ")";
			break;
		case ERR_NOTEXTTOSEND:
			oss << " :No text to send";
			break;
		case ERR_UNKNOWNCOMMAND:
			oss << " " << arg1 << " :Unknown command";
			break;
		case ERR_NOMOTD:
			oss << " :MOTD File is missing";
			break;
		case ERR_NONICKNAMEGIVEN:
			oss << " :No nickname given";
			break;
		case ERR_ERRONEUSNICK:
			oss << " " << arg1 << " :Erroneous nickname";
			break;
		case ERR_NICKNAMEINUSE:
			oss << " " << arg1 << " :Nickname is already in use";
			break;
		case ERR_USERNOTINCHANNEL:
			oss << " " << arg1 << " " << arg2 << " :They aren't on that channel";
			break;
		case ERR_NOTONCHANNEL:
			oss << " " << arg1 << " :You're not on that channel";
			break;
		case ERR_USERONCHANNEL:
			oss << " " << arg1 << " " << arg2 << " :is already on channel";
			break;
		case ERR_NOTREGISTERED:
			oss << " :You have not registered";
			break;
		case ERR_NEEDMOREPARAMS:
			oss << " " << arg1 << " :Not enough parameters";
			break;
		case ERR_ALREADYREGISTRED:
			oss << " :You may not reregister";
			break;
		case ERR_PASSWDMISMATCH:
			oss << " :Password incorrect";
			break;
		case ERR_KEYSET:
			oss << " " << arg1 << " :Channel key already set";
			break;
		case ERR_CHANNELISFULL:
			oss << " " << arg1 << " :Cannot join channel (+l)";
			break;
		case ERR_UNKNOWNMODE:
			oss << " " << arg1 << " :is unknown mode char to me";
			break;
		case ERR_INVITEONLYCHAN:
			oss << " " << arg1 << " :Cannot join channel (+i)";
			break;
		case ERR_BADCHANNELKEY:
			oss << " " << arg1 << " :Cannot join channel (+k)";
			break;
		case ERR_CHANOPRIVSNEEDED:
			oss << " " << arg1 << " :You're not channel operator";
			break;
	}
	oss << "\r\n";
	return (oss.str());
}
