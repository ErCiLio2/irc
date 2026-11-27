/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eteixeir <eteixeir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 16:04:41 by eteixeir          #+#    #+#             */
/*   Updated: 2026/11/23 16:05:12 by eteixeir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <set>
#include <stdint.h>
#include <map>
#include <iostream>
#include <iomanip>
#include <list>
#include <sys/types.h>
#include <sys/socket.h>

# define NICK_MASK 1
# define USER_MASK 2
# define PASS_MASK 4
# define REAL_NAME_MASK 8

# define MAX_MESSAGE_SIZE 512
# define MAX_INDEX_SIZE 510
# define CR "\r"
# define LF "\n"

class Channel;

class Client {
	private:
		bool old_line_continuation;
		int			my_fd;
		uint8_t		registered;
		size_t		already_sent;
		std::string nick;
		std::string username;
		std::string hostname;
		std::string	realname;
		std::string	servername;
		std::string recv_buffer;
		std::string send_buffer;
		std::set<Channel*> my_channels;
		std::set<Channel *> inbox;
		
	public:
		typedef std::set<Channel *>::iterator iterator;

	public:
		Client();
		Client(int fd);
		Client(const Client &other);
		static Client createClient(int fd);
		Client &operator=(const Client &other);
		~Client(void);

		void setNick(const std::string &str);
		void setFd(int fd);
		void setUsername(const std::string &str);
		void setHostname(const std::string &str);
		void setRealName(const std::string &str);
		void setServerName(const std::string &str);
		void passwordWasProvided(bool yes = true);
		void setOutputBuffer(const std::string &str);

		const std::string &getNick(void) const;
		const std::string &getUser(void) const;
		const std::string &getHost(void) const;
		int	getMyFd(void) const;
		std::set<Channel*> &getMyChannels();
		std::set<Channel*> &getMyInbox();
		const std::string &getRealName(void) const;
		const std::string &getServerName(void) const;
		const std::string &getBuffer(void) const;
		std::string &getOutputBuffer(void);

	//Channel management.
	public:
		void sendInviteToInbox(Channel *channel);
		void registerMyChannel(Channel *channel);
	
		void removeChannelFromInbox(Channel *channel);
		void removeChannelFromRegister(Channel *Channel);
	
	public: // buffer management
		void		clearOutputBuffer();
		ssize_t		textMe();
		ssize_t		textMe(const std::string &raw);
		void		clearInputBuffer();
		void		insert(char c);
		void		insert(const std::string &str);
		std::string getMessage();

	public:
		void unRegister();
		bool isRegistered() const;
		std::string extractMessage();
	};

#endif
