/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eteixeir <eteixeir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 14:26:46 by eteixeir          #+#    #+#             */
/*   Updated: 2026/11/27 03:32:35 by eteixeir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Replies.hpp"
# include "Channel.hpp"
# include "Client.hpp"
# include "Message.hpp"

class Server
{
  public:
	typedef std::map<int, Client>::iterator client_iterator;
	typedef std::map<std::string, Channel>::iterator channel_iterator;

	private:
		std::string							password_server;
		int									port_server;
		int									serverSocket;
		Message								message_server;
		std::map<int, Client>				clients;
		std::map<std::string, Channel>		channels;
		std::vector<struct pollfd>			pollFds;
		std::map<int, std::string>			sendBuffers;
		std::map<std::string, std::string>	channelKeys;
		std::map<std::string, size_t>		channelLimits;
		std::set<std::string>				topicRestricted;
		std::set<std::string>				inviteOnly;
		static volatile sig_atomic_t		reciveSignal;

		Server(const Server &value);

		std::string	channelModes(const std::string &name, bool isMember);
		void		eraseChannel(const std::string &name);
		void		tryRegisterWelcome(int fd, bool wasRegistered);
		void		acceptNewClient(void);
		bool		handleClientRead(size_t index);
		bool		handleClientWrite(size_t index);

		void		handleQuit(int fd, const Params &c);
		void		handlePing(int fd, const Params &c);
		void		handlePass(int fd, const Params &c);
		void		handleNick(int fd, const Params &c);
		void		handleUser(int fd, const Params &c);
		void		handlePrivmsg(int fd, const Params &c, bool isNotice);
		void		handleInvite(int fd, const Params &c);
		void		handleKick(int fd, const Params &c);
		void		handleTopic(int fd, const Params &c);
		void		handleMode(int fd, const Params &c);
		void		handleNames(int fd, const Params &c);
		void		handlePart(int fd, const Params &c);
		void		handleJoin(int fd, const Params &c);

	public:
		Server(std::string password, int port);
		~Server();

		void		init(void);
		void		run(void);
		void		disconnectClient(size_t index);
		void		checkComands(int fd, Params c);
		void		extractMessage(int fd);
		void		sendToClient(int fd, const std::string message);
		Client		*getClientByFd(int fd);
		Client		*getClientByNick(const std::string &nick);
		int			getClientbyName(const std::string &name);
		bool		getChannelbyName(const std::string &name);
		void		removeClientFromServer(Client &data);
		void		removeChannelFromServer(Channel &data);
		static void	signalHandler(int sig);


	public:
		void sendTopicAllClients(const Params &c, Channel *ch, Client *requester);
		void sendNickToAllChannels(std::set<Channel *> &chs, Client *requester, std::string &msg);
};

int ft_atoi(const char *str);

#endif
