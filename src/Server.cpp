/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eteixeir <eteixeir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/12 14:30:25 by eteixeir          #+#    #+#             */
/*   Updated: 2026/11/27 03:29:45 by eteixeir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"

int ft_atoi(const char *str) {
	std::stringstream ss;
	ss << str;
	int val;
	ss >> val;
	if (!ss.eof() || ss.fail() || ss.bad())
		return (-1);
	return (val);
}

bool equalsIgnoreCase(const std::string& s1, const std::string& s2) {
    if (s1.length() != s2.length()) return false;
    
    for (size_t i = 0; i < s1.length(); ++i) {
        if (::tolower(static_cast<unsigned char>(s1[i])) != ::tolower(static_cast<unsigned char>(s2[i]))) {
            return false;
        }
    }
    return true;
}

volatile sig_atomic_t Server::reciveSignal = 0;

Server::Server(std::string password, int port)
	: password_server(password), port_server(port), serverSocket(-1)
{
}

Server::~Server()
{
	clients.clear();
	for (size_t i = 0; i < pollFds.size(); i++)
		close(pollFds[i].fd);
	if (serverSocket >= 0 && (pollFds.empty() || pollFds[0].fd != serverSocket))
		close(serverSocket);
}

int Server::getClientbyName(const std::string &name) {
	client_iterator it;

	it = clients.begin();
	while (it != clients.end())
	{
		if (equalsIgnoreCase(it->second.getNick(), name)) return (it->first);
		++it;
	}
	return (-1);
}

bool Server::getChannelbyName(const std::string &name) {
	channel_iterator it;

	it = channels.begin();
	while (it != channels.end())
	{
		if (equalsIgnoreCase(it->second.getName(), name)) return (true);
		++it;
	}
	return (false);
}

static Server::channel_iterator findChannel(std::map<std::string, Channel> &channels, const std::string &name)
{
	for (Server::channel_iterator it = channels.begin(); it != channels.end(); ++it)
	{
		if (equalsIgnoreCase(it->first, name))
			return (it);
	}
	return (channels.end());
}

void Server::signalHandler(int sig)
{
	(void)sig;
	reciveSignal = 1;
}

static inline void	setReuseAddr(int socket)
{
	int	opt;

	opt = 1;
	if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("setsockopt failure");
}

static inline void	nonBlocking(int socket)
{
	if (fcntl(socket, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error(std::string("fcntl failure: ") + strerror(errno));
}

static std::string toUpper(std::string str)
{
	for (size_t i = 0; i < str.size(); i++)
		str[i] = std::toupper(static_cast<unsigned char>(str[i]));
	return (str);
}

static bool isEmptyText(const std::string &str)
{
	if (str.empty())
		return (true);
	if (str == "\n" || str == "\r\n")
		return (true);
	return (false);
} 

static bool isSpecialChar(char c)
{
	if (c == '[' || c == ']' || c == '\\' || c == '`')
		return (true);
	if (c == '_' || c == '^' || c == '{' || c == '|' || c == '}')
		return (true);
	return (false);
}

static bool isValidNick(const std::string &nick)
{
	if (nick.empty() || nick.size() > 30)
		return (false);
	unsigned char first = static_cast<unsigned char>(nick[0]);
	if (!std::isalpha(first) && !isSpecialChar(nick[0]))
		return (false);
	for (size_t i = 1; i < nick.size(); i++)
	{
		unsigned char ch = static_cast<unsigned char>(nick[i]);
		if (!std::isalnum(ch) && !isSpecialChar(nick[i]) && nick[i] != '-')
			return (false);
	}
	return (true);
}

static std::string buildClientPrefix(Client &cl)
{
	return (":" + cl.getNick() + "!" + cl.getUser() + "@" + cl.getHost());
}

static inline struct pollfd	addNewSocket(int socket)
{
	struct pollfd	po;

	po.fd = socket;
	po.events = POLLIN;
	po.revents = 0;
	return (po);
}

void Server::init(void)
{
	struct sockaddr_in	addr;

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0)
		throw std::runtime_error("Error creating socket");
	setReuseAddr(serverSocket);
	nonBlocking(serverSocket);
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(static_cast<unsigned short>(port_server));
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(serverSocket, reinterpret_cast<struct sockaddr *>(&addr),
			sizeof(addr)) < 0)
		throw std::runtime_error(std::string("bind failure: ") + strerror(errno));
	if (listen(serverSocket, SOMAXCONN) < 0)
		throw std::runtime_error(std::string("listen failure: ") + strerror(errno));
	pollFds.push_back(addNewSocket(serverSocket));
	std::cout << "Server listening on port " << port_server << std::endl;
}

void Server::disconnectClient(size_t index)
{
	int value = pollFds[index].fd;
	sendBuffers.erase(value);
	if (clients.count(value))
		removeClientFromServer(clients[value]); // Correcao 1
	close(value);
	clients.erase(value); // Correcao 2
	pollFds.erase(pollFds.begin() + index);
	std::cout << "Disconnected client " << value << std::endl;
}


Client *Server::getClientByFd(int fd)
{
	if (clients.count(fd))
		return (&clients[fd]);
	return (NULL);
}

Client *Server::getClientByNick(const std::string &nick)
{
	client_iterator it = clients.begin();
	while (it != clients.end())
	{
		if (equalsIgnoreCase(it->second.getNick(), nick))
			return (&it->second);
		++it;
	}
	return (NULL);
}

void Server::eraseChannel(const std::string &name)
{
	channelKeys.erase(name);
	channelLimits.erase(name);
	topicRestricted.erase(name);
	inviteOnly.erase(name);
	removeChannelFromServer(channels[name]); // essa function tbm poderia ja remover o channel, Teixeira vai fazer amanha
	channels.erase(name);
}

std::string Server::channelModes(const std::string &name, bool isMember)
{
	std::string args;

	std::string modes = "+";
	if (inviteOnly.count(name))
		modes += "i";
	if (topicRestricted.count(name))
		modes += "t";
	if (channelKeys.count(name)) {
		modes += "k";
		if (isMember)
			args += " " + channels[name].getPassword();
	}
	if (channelLimits.count(name)) {
		modes += "l";
		if (isMember) {
			std::stringstream ss;
			ss << channels[name].getLimit();
			args += " " + ss.str();
		}
	}
	modes += args;
	return (modes);
}

void Server::removeChannelFromServer(Channel &channel) {
	channel.clearInviteList();
	channel.clearMemberAndOperatorsList();
}

void Server::removeClientFromServer(Client &client)
{
	std::string quitMsg = buildClientPrefix(client) + " QUIT :Client disconnected\r\n";
	std::set<Channel*> &clientChannels = client.getMyChannels();
	std::set<Channel*> &inbox = client.getMyInbox();
	Client::iterator it = clientChannels.begin();

	while (it != clientChannels.end())
	{
		Channel *ch = *it;
		ch->eraseClient(&client);
		if (ch->membersSize() == 0)
			eraseChannel(ch->getName());
		else
		{
			std::set<Client*> &members = ch->getMembersList();
			for (Channel::iterator mIt = members.begin(); mIt != members.end(); ++mIt)
				sendToClient((*mIt)->getMyFd(), quitMsg);
		}
		++it;
	}
	clientChannels.clear();
	it = inbox.begin();
	while (it != inbox.end())
	{
		(*it)->eraseFromInvatation(&client);
		++it;
	}
	inbox.clear();
}

void Server::sendToClient(int fd, const std::string message)
{
	if (!clients.count(fd))
		return ;
	clients[fd].textMe(message);
	std::string &out = clients[fd].getOutputBuffer();

	for (size_t i = 0; i < pollFds.size(); i++)
	{
		if (pollFds[i].fd == fd)
		{
			if (!out.empty())
				pollFds[i].events |= POLLOUT;
			else
				pollFds[i].events &= ~POLLOUT;
			return ;
		}
	}
}

void Server::tryRegisterWelcome(int fd, bool wasRegistered)
{
	Client &cl = clients[fd];
	if (wasRegistered || !cl.isRegistered())
		return ;

	std::string nick = cl.getNick();
	std::string fullId = nick + "!" + cl.getUser() + "@" + cl.getHost();
	sendToClient(fd, formatNumericReply(RPL_WELCOME, nick, fullId));
	sendToClient(fd, formatNumericReply(RPL_YOURHOST, nick));
	sendToClient(fd, formatNumericReply(RPL_CREATED, nick, "today"));
	sendToClient(fd, formatNumericReply(RPL_MYINFO, nick));
}

void Server::handleQuit(int fd, const Params &c)
{
	std::string reason = "Client Quit";
	if (!c.empty() && !isEmptyText(c[0]))
		reason = c[0];

	Client *quitter = &clients[fd];
	std::string quitMsg = buildClientPrefix(*quitter) + " QUIT :" + reason + "\r\n";
	std::set<Channel*> &chans = quitter->getMyChannels();
	for (std::set<Channel*>::iterator it = chans.begin(); it != chans.end(); ++it)
	{
		Channel *ch = *it;
		ch->eraseClient(quitter);
		if (ch->membersSize() == 0)
			eraseChannel(ch->getName());
		else
		{
			std::set<Client*> &members = ch->getMembersList();
			for (Channel::iterator mIt = members.begin(); mIt != members.end(); ++mIt)
				sendToClient((*mIt)->getMyFd(), quitMsg);
		}
	}
	chans.clear();

	for (size_t i = 0; i < pollFds.size(); i++)
	{
		if (pollFds[i].fd == fd)
		{
			disconnectClient(i);
			return ;
		}
	}
}

void Server::handlePing(int fd, const Params &c)
{
	if (c.empty() || isEmptyText(c[0]))
	{
		sendToClient(fd, formatNumericReply(ERR_NOORIGIN, clients[fd].getNick()));
		return ;
	}
	std::string pong = ":" + std::string(SERVER_NAME) + " PONG " + std::string(SERVER_NAME) + " :" + c[0] + CR LF;
	sendToClient(fd, pong);
}

void Server::handlePass(int fd, const Params &c)
{
	Client &cl = clients[fd];
	if (cl.isRegistered())
	{
		sendToClient(fd, formatNumericReply(ERR_ALREADYREGISTRED, cl.getNick()));
		return ;
	}
	if (c.empty() || isEmptyText(c[0]))
	{
		sendToClient(fd, formatNumericReply(ERR_NEEDMOREPARAMS, cl.getNick(), "PASS"));
		return ;
	}
	if (c[0] != password_server)
	{
		sendToClient(fd, formatNumericReply(ERR_PASSWDMISMATCH, cl.getNick()));
		return ;
	}
	bool wasReg = cl.isRegistered();
	cl.passwordWasProvided();
	tryRegisterWelcome(fd, wasReg);
}

void Server::handleNick(int fd, const Params &c)
{
	Client &cl = clients[fd];
	if (c.empty() || isEmptyText(c[0]))
	{
		sendToClient(fd, formatNumericReply(ERR_NONICKNAMEGIVEN, cl.getNick()));
		return ;
	}
	if (!isValidNick(c[0]))
	{
		sendToClient(fd, formatNumericReply(ERR_ERRONEUSNICK, cl.getNick(), c[0]));
		return ;
	}
	Client *existing = getClientByNick(c[0]);
	if (existing && existing != &cl)
	{
		sendToClient(fd, formatNumericReply(ERR_NICKNAMEINUSE, cl.getNick(), c[0]));
		return ;
	}
	bool wasReg = cl.isRegistered();

	if (!wasReg)
		cl.setNick(c[0]);
	else {
		std::string nick_message = buildClientPrefix(cl) + " NICK :" + c[0] + "\r\n";;
		std::cerr << nick_message << std::endl; // Logs
		std::set<Channel *> &chs = cl.getMyChannels();
		sendNickToAllChannels(chs, &cl, nick_message);
		cl.textMe(nick_message);
		cl.setNick(c[0]);
	}
	tryRegisterWelcome(fd, wasReg);
}

void Server::handleUser(int fd, const Params &c)
{
	Client &cl = clients[fd];
	if (cl.isRegistered())
	{
		sendToClient(fd, formatNumericReply(ERR_ALREADYREGISTRED, cl.getNick()));
		return ;
	}
	if (c.size() < 4 || isEmptyText(c[3]))
	{
		sendToClient(fd, formatNumericReply(ERR_NEEDMOREPARAMS, cl.getNick(), "USER"));
		return ;
	}
	bool wasReg = cl.isRegistered();
	cl.setUsername(c[0]);
	cl.setServerName(c[2]);
	cl.setRealName(c[3]);
	tryRegisterWelcome(fd, wasReg);
}

void Server::handlePrivmsg(int fd, const Params &c, bool isNotice)
{
	Client *sender = &clients[fd];
	std::string cmdName = toUpper(message_server.getCommand());
	if (c.empty() || isEmptyText(c[0]))
	{
		if (!isNotice)
			sendToClient(fd, formatNumericReply(ERR_NORECIPIENT, sender->getNick(), cmdName));
		return ;
	}
	if (c.size() < 2 || isEmptyText(c[1]))
	{
		if (!isNotice)
			sendToClient(fd, formatNumericReply(ERR_NOTEXTTOSEND, sender->getNick()));
		return ;
	}

	std::string msg = buildClientPrefix(*sender) + " " + cmdName + " " + c[0] + " :" + c[1] + "\r\n";
    if (msg.size() > 512)
    {
        msg = msg.substr(0, 510) + "\r\n";
    }
	if (c[0][0] == '#')
	{
		channel_iterator chIt = findChannel(channels, c[0]);
		if (chIt == channels.end())
		{
			if (!isNotice)
				sendToClient(fd, formatNumericReply(ERR_NOSUCHCHANNEL, sender->getNick(), c[0]));
			return ;
		}
		Channel *ch = &chIt->second;
		if (!ch->hasClient(sender))
		{
			if (!isNotice)
				sendToClient(fd, formatNumericReply(ERR_CANNOTSENDTOCHAN, sender->getNick(), c[0]));
			return ;
		}
		ch->sendMultCastMessage(sender, msg);
		return ;
	}

	Client *target = getClientByNick(c[0]);
	if (!target)
	{
		if (!isNotice)
			sendToClient(fd, formatNumericReply(ERR_NOSUCHNICK, sender->getNick(), c[0]));
		return ;
	}
	sendToClient(target->getMyFd(), msg);
}

void Server::handleInvite(int fd, const Params &c)
{
	Client *requester = &clients[fd];
	if (c.size() < 2)
	{
		sendToClient(fd, formatNumericReply(ERR_NEEDMOREPARAMS, requester->getNick(), "INVITE"));
		return ;
	}
	channel_iterator it = findChannel(channels, c[1]);
	std::cout << "Client: " << c[0] << std::endl;
	std::cout << "Channel: " << c[1] << std::endl;
	if (it == channels.end())
	{
		sendToClient(fd, formatNumericReply(ERR_NOSUCHCHANNEL, requester->getNick(), c[1]));
		return ;
	}
	Channel *ch = &it->second;
	if (!ch->hasClient(requester))
	{
		sendToClient(fd, formatNumericReply(ERR_NOTONCHANNEL, requester->getNick(), c[1]));
		return ;
	}
	if (!ch->getMembersOperatorsList().count(requester))
	{
		sendToClient(fd, formatNumericReply(ERR_CHANOPRIVSNEEDED, requester->getNick(), c[1]));
		return ;
	}
	Client *target = getClientByNick(c[0]);
	if (!target)
	{
		sendToClient(fd, formatNumericReply(ERR_NOSUCHNICK, requester->getNick(), c[0]));
		return ;
	}
	if (ch->hasClient(target))
	{
		sendToClient(fd, formatNumericReply(ERR_USERONCHANNEL, requester->getNick(), c[0], c[1]));
		return ;
	}
	ch->inviteClient(target);
	/* By eteixeir*/
	std::string invite_message = buildClientPrefix(*requester) + " INVITE " + c[0] + " " + c[1] + "\r\n";
	target->textMe(invite_message);
	sendToClient(fd, formatNumericReply(RPL_INVITING, requester->getNick(), c[0], c[1]));
}

void Server::handleKick(int fd, const Params &c)
{
	Client *requester = &clients[fd];
	if (c.size() < 2 || isEmptyText(c[0]) || isEmptyText(c[1]))
	{
		sendToClient(fd, formatNumericReply(ERR_NEEDMOREPARAMS, requester->getNick(), "KICK"));
		return ;
	}
	channel_iterator it = findChannel(channels, c[0]);
	if (it == channels.end())
	{
		sendToClient(fd, formatNumericReply(ERR_NOSUCHCHANNEL, requester->getNick(), c[0]));
		return ;
	}
	std::string chanKey = it->first;
	Channel *ch = &it->second;
	if (!ch->hasClient(requester))
	{
		sendToClient(fd, formatNumericReply(ERR_NOTONCHANNEL, requester->getNick(), c[0]));
		return ;
	}
	if (!ch->getMembersOperatorsList().count(requester))
	{
		sendToClient(fd, formatNumericReply(ERR_CHANOPRIVSNEEDED, requester->getNick(), c[0]));
		return ;
	}
	Client *target = getClientByNick(c[1]);
	if (!target || !ch->hasClient(target))
	{
		sendToClient(fd, formatNumericReply(ERR_USERNOTINCHANNEL, requester->getNick(), c[1], c[0]));
		return ;
	}
	std::string reason = target->getNick();
	if (c.size() >= 3 && !isEmptyText(c[2]))
		reason = c[2];
	std::string kickMsg = buildClientPrefix(*requester) + " KICK " + c[0] + " " + target->getNick() + " :" + reason + "\r\n";
	std::set<Client*> &members = ch->getMembersList();
	for (Channel::iterator mIt = members.begin(); mIt != members.end(); ++mIt)
		sendToClient((*mIt)->getMyFd(), kickMsg);
	ch->eraseClient(target);
	target->removeChannelFromRegister(ch);
	if (ch->membersSize() == 0)
		eraseChannel(chanKey);
}

/** 
 * 
 * 
 * 
 * 
 * BY eteixeir 
 * 
 * */

void Server::sendNickToAllChannels(std::set<Channel *> &chs, Client *requester, std::string &msg) {
	std::set<Channel *>::iterator it = chs.begin();

	while (it != chs.end()) {
		(*it)->sendMultCastMessage(requester, msg);
		++it;
	}
}

/** 
 * 
 *  BY eteixeir
 * 
 * 
*/
void Server::sendTopicAllClients(const Params &c, Channel *ch, Client *requester) {
	std::string topic_message = buildClientPrefix(*requester) + " TOPIC " + c[0] + " :" + ch->getTopic() + "\r\n";
	std::set<Client*> &members = ch->getMembersList();
	for (Channel::iterator mIt = members.begin(); mIt != members.end(); ++mIt)
		sendToClient((*mIt)->getMyFd(), topic_message);
}

void Server::handleTopic(int fd, const Params &c)
{
	Client *requester = &clients[fd];
	if (c.empty() || isEmptyText(c[0]))
	{
		sendToClient(fd, formatNumericReply(ERR_NEEDMOREPARAMS, requester->getNick(), "TOPIC"));
		return ;
	}
	channel_iterator it = findChannel(channels, c[0]);
	if (it == channels.end())
	{
		sendToClient(fd, formatNumericReply(ERR_NOSUCHCHANNEL, requester->getNick(), c[0]));
		return ;
	}
	Channel *ch = &it->second;
	if (c.size() == 1)
	{
		if (ch->getTopic().empty())
			sendToClient(fd, formatNumericReply(RPL_NOTOPIC, requester->getNick(), c[0]));
		else
			sendToClient(fd, formatNumericReply(RPL_TOPIC, requester->getNick(), c[0], ch->getTopic()));
		return ;
	}
	if (!ch->hasClient(requester))
	{
		sendToClient(fd, formatNumericReply(ERR_NOTONCHANNEL, requester->getNick(), c[0]));
		return ;
	}
	try
	{
		ch->setTopic(requester, c[1]);
		sendTopicAllClients(c, ch, requester);
	}
	catch (std::exception &)
	{
		sendToClient(fd, formatNumericReply(ERR_CHANOPRIVSNEEDED, requester->getNick(), c[0]));
	}
}

void Server::handleMode(int fd, const Params &c)
{
	Client *requester = &clients[fd];
	if (c.empty() || isEmptyText(c[0]))
	{
		sendToClient(fd, formatNumericReply(ERR_NEEDMOREPARAMS, requester->getNick(), "MODE"));
		return ;
	}
	channel_iterator it = findChannel(channels, c[0]);
	if (it == channels.end())
	{
		sendToClient(fd, formatNumericReply(ERR_NOSUCHCHANNEL, requester->getNick(), c[0]));
		return ;
	}
	std::string chanKey = it->first;
	Channel *ch = &it->second;
	if (c.size() < 2)
	{
		sendToClient(fd, formatNumericReply(RPL_CHANNELMODEIS, requester->getNick(), c[0], channelModes(chanKey, ch->hasClient(requester))));
		return ;
	}
	if (!ch->getMembersOperatorsList().count(requester))
	{
		sendToClient(fd, formatNumericReply(ERR_CHANOPRIVSNEEDED, requester->getNick(), c[0]));
		return ;
	}

	bool reverse = false;
	size_t argIdx = 2;
	std::string appliedModes;
	std::string appliedArgs;
	char lastSign = '\0';

	for (size_t i = 0; i < c[1].size(); ++i)
	{
		char mode = c[1][i];
		if (mode == '+')
		{
			reverse = false;
			continue ;
		}
		if (mode == '-')
		{
			reverse = true;
			continue ;
		}

		char currentSign = reverse ? '-' : '+';

		if (mode == 'i')
		{
			ch->toggleMode(Channel::Mode::I, reverse);
			if (reverse)
				inviteOnly.erase(chanKey);
			else
				inviteOnly.insert(chanKey);
			if (lastSign != currentSign)
			{
				appliedModes += currentSign;
				lastSign = currentSign;
			}
			appliedModes += 'i';
		}
		else if (mode == 't')
		{
			ch->toggleMode(Channel::Mode::T, reverse);
			if (reverse)
				topicRestricted.erase(chanKey);
			else
				topicRestricted.insert(chanKey);
			if (lastSign != currentSign)
			{
				appliedModes += currentSign;
				lastSign = currentSign;
			}
			appliedModes += 't';
		}
		else if (mode == 'k')
		{
			if (reverse)
			{
				ch->toggleMode(Channel::Mode::K, true);
				channelKeys.erase(chanKey);
				if (lastSign != currentSign)
				{
					appliedModes += currentSign;
					lastSign = currentSign;
				}
				appliedModes += 'k';
			}
			else if (argIdx < c.size() && !isEmptyText(c[argIdx]))
			{
				std::string key = c[argIdx++];
				ch->toggleMode(Channel::Mode::K, false);
				channelKeys[chanKey] = key;
				ch->setPassword(key);
				if (lastSign != currentSign)
				{
					appliedModes += currentSign;
					lastSign = currentSign;
				}
				appliedModes += 'k';
				appliedArgs += " " + key;
			}
			else
				sendToClient(fd, formatNumericReply(ERR_NEEDMOREPARAMS, requester->getNick(), "MODE"));
		}
		else if (mode == 'l')
		{
			if (reverse)
			{
				ch->toggleMode(Channel::Mode::L, true);
				channelLimits.erase(chanKey);
				if (lastSign != currentSign)
				{
					appliedModes += currentSign;
					lastSign = currentSign;
				}
				appliedModes += 'l';
			}
			else if (argIdx < c.size() && !isEmptyText(c[argIdx]))
			{
				std::string limStr = c[argIdx++];
				int lim = ft_atoi(limStr.c_str());
				if (lim > 0)
				{
					ch->toggleMode(Channel::Mode::L, false);
					ch->setLimit(lim);
					channelLimits[chanKey] = static_cast<size_t>(lim);
					if (lastSign != currentSign)
					{
						appliedModes += currentSign;
						lastSign = currentSign;
					}
					appliedModes += 'l';
					appliedArgs += " " + limStr;
				}
			}
			else
				sendToClient(fd, formatNumericReply(ERR_NEEDMOREPARAMS, requester->getNick(), "MODE"));
		}
		else if (mode == 'o')
		{
			if (argIdx < c.size() && !isEmptyText(c[argIdx]))
			{
				std::string targetNick = c[argIdx++];
				Client *target = getClientByNick(targetNick);
				if (!target)
					sendToClient(fd, formatNumericReply(ERR_NOSUCHNICK, requester->getNick(), targetNick));
				else if (!ch->isMember(target))
					sendToClient(fd, formatNumericReply(ERR_USERNOTINCHANNEL, requester->getNick(), targetNick, c[0]));
				else
				{
					ch->modifyMemberRole(target, !reverse);
					if (lastSign != currentSign)
					{
						appliedModes += currentSign;
						lastSign = currentSign;
					}
					appliedModes += 'o';
					appliedArgs += " " + target->getNick();
				}
			}
			else
				sendToClient(fd, formatNumericReply(ERR_NEEDMOREPARAMS, requester->getNick(), "MODE"));
		}
		else
		{
			sendToClient(fd, formatNumericReply(ERR_UNKNOWNMODE, requester->getNick(), std::string(1, mode)));
		}
	}

	if (!appliedModes.empty())
	{
		std::string modeMsg = buildClientPrefix(*requester) + " MODE " + c[0] + " " + appliedModes + appliedArgs + "\r\n";
		std::set<Client*> &members = ch->getMembersList();
		for (Channel::iterator mIt = members.begin(); mIt != members.end(); ++mIt)
			sendToClient((*mIt)->getMyFd(), modeMsg);
	}
}

void Server::handlePart(int fd, const Params &c)
{
	Client *client = &clients[fd];
	if (c.empty() || isEmptyText(c[0]))
	{
		sendToClient(fd, formatNumericReply(ERR_NEEDMOREPARAMS, client->getNick(), "PART"));
		return ;
	}
	std::string reason;
	if (c.size() >= 2 && !isEmptyText(c[1]))
		reason = c[1];

	std::stringstream ss(c[0]);
	std::string name;
	while (std::getline(ss, name, ','))
	{
		if (name.empty())
			continue ;
		channel_iterator it = findChannel(channels, name);
		if (it == channels.end())
		{
			sendToClient(fd, formatNumericReply(ERR_NOSUCHCHANNEL, client->getNick(), name));
			continue ;
		}
		std::string chanKey = it->first;
		Channel *ch = &it->second;
		if (!ch->hasClient(client))
		{
			sendToClient(fd, formatNumericReply(ERR_NOTONCHANNEL, client->getNick(), name));
			continue ;
		}
		std::string partMsg = buildClientPrefix(*client) + " PART " + name;
		if (!reason.empty())
			partMsg += " :" + reason;
		partMsg += "\r\n";

		std::set<Client*> &members = ch->getMembersList();
		for (Channel::iterator mIt = members.begin(); mIt != members.end(); ++mIt)
			sendToClient((*mIt)->getMyFd(), partMsg);

		ch->eraseClient(client);
		client->removeChannelFromRegister(ch);
		if (ch->membersSize() == 0)
			eraseChannel(chanKey);
	}
}

static void sendNamesReply(Server &srv, int fd, Client *joiner, Channel *ch, const std::string &chanName)
{
	std::string namesList;
	std::set<Client*> &members = ch->getMembersList();
	for (Channel::iterator mIt = members.begin(); mIt != members.end(); ++mIt)
	{
		if (!namesList.empty())
			namesList += " ";
		if (ch->isOperator(*mIt))
			namesList += "@";
		namesList += (*mIt)->getNick();
	}
	srv.sendToClient(fd, formatNumericReply(RPL_NAMREPLY, joiner->getNick(), chanName, namesList));
	srv.sendToClient(fd, formatNumericReply(RPL_ENDOFNAMES, joiner->getNick(), chanName));
}

void Server::handleJoin(int fd, const Params &c)
{
	Client *joiner = &clients[fd];
	if (c.empty() || isEmptyText(c[0]))
	{
		sendToClient(fd, formatNumericReply(ERR_NEEDMOREPARAMS, joiner->getNick(), "JOIN"));
		return ;
	}
	if (c[0][0] != '#' || c[0].size() < 2)
	{
		sendToClient(fd, formatNumericReply(ERR_NOSUCHCHANNEL, joiner->getNick(), c[0]));
		return ;
	}

	std::string joinMsg = buildClientPrefix(*joiner) + " JOIN " + c[0] + "\r\n";
	channel_iterator it = findChannel(channels, c[0]);
	if (it == channels.end())
	{
		Channel channel = Channel::createChannel(c[0], joiner);
		channels.insert(channels.end(), std::make_pair(c[0], channel));
		topicRestricted.insert(c[0]);
		Channel *newCh = &channels[c[0]];
		joiner->registerMyChannel(newCh);
		sendToClient(fd, joinMsg);
		sendNamesReply(*this, fd, joiner, newCh, c[0]);
		return ;
	}

	std::string chanKey = it->first;
	Channel *ch = &it->second;
	if (ch->hasClient(joiner))
		return ;
	if (channelKeys.count(chanKey))
	{
		if (c.size() < 2 || c[1] != channelKeys[chanKey])
		{
			sendToClient(fd, formatNumericReply(ERR_BADCHANNELKEY, joiner->getNick(), c[0]));
			return ;
		}
	}
	try
	{
		ch->addMember(joiner);
		std::set<Client*> &members = ch->getMembersList();
		for (Channel::iterator mIt = members.begin(); mIt != members.end(); ++mIt)
			sendToClient((*mIt)->getMyFd(), joinMsg);
		if (!ch->getTopic().empty())
			sendToClient(fd, formatNumericReply(RPL_TOPIC, joiner->getNick(), c[0], ch->getTopic()));
		sendNamesReply(*this, fd, joiner, ch, c[0]);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		std::string err = e.what();
		if (err.find("TOO_MANY_MEMBERS") != std::string::npos)
			sendToClient(fd, formatNumericReply(ERR_CHANNELISFULL, joiner->getNick(), c[0]));
		else if (err.find("473") != std::string::npos)
			sendToClient(fd, formatNumericReply(ERR_INVITEONLYCHAN, joiner->getNick(), c[0]));
	}
}

void Server::checkComands(int fd, Params c)
{
	std::string cmd = message_server.getCommand();
	if (cmd.empty())
		return ;

	if (equalsIgnoreCase(cmd, "quit"))
		handleQuit(fd, c);
	else if (equalsIgnoreCase(cmd, "ping"))
		handlePing(fd, c);
	else if (equalsIgnoreCase(cmd, "pass"))
		handlePass(fd, c);
	else if (equalsIgnoreCase(cmd, "nick"))
		handleNick(fd, c);
	else if (equalsIgnoreCase(cmd, "user"))
		handleUser(fd, c);
	else if (!clients[fd].isRegistered())
	{
		if (!equalsIgnoreCase(cmd, "notice"))
			sendToClient(fd, formatNumericReply(ERR_NOTREGISTERED, clients[fd].getNick()));
	}
	else if (equalsIgnoreCase(cmd, "privmsg"))
		handlePrivmsg(fd, c, false);
	else if (equalsIgnoreCase(cmd, "notice"))
		handlePrivmsg(fd, c, true);
	else if (equalsIgnoreCase(cmd, "invite"))
		handleInvite(fd, c);
	else if (equalsIgnoreCase(cmd, "kick"))
		handleKick(fd, c);
	else if (equalsIgnoreCase(cmd, "topic"))
		handleTopic(fd, c);
	else if (equalsIgnoreCase(cmd, "mode"))
		handleMode(fd, c);
	else if (equalsIgnoreCase(cmd, "part"))
		handlePart(fd, c);
	else if (equalsIgnoreCase(cmd, "join"))
		handleJoin(fd, c);
	else
		sendToClient(fd, formatNumericReply(ERR_UNKNOWNCOMMAND, clients[fd].getNick(), cmd));
}

void Server::extractMessage(int fd)
{
	std::string msg;

	while (clients.count(fd) && !(msg = clients[fd].extractMessage()).empty())
	{
		if (msg == "\n" || msg == "\r" || msg == "\r\n" || msg.find('\0') != std::string::npos)
			continue ;

		bool parseFailed = false;
		try
		{
			message_server.parse(msg);
		}
		catch (std::exception &)
		{
			parseFailed = true;
		}

		if (parseFailed && message_server.getCommand().empty())
		{
			size_t endPos = msg.find_first_of(" \r\n");
			std::string rawCmd = msg;
			if (endPos != std::string::npos)
				rawCmd = msg.substr(0, endPos);
			if (!rawCmd.empty() && rawCmd[0] != ':' && clients[fd].isRegistered())
				sendToClient(fd, formatNumericReply(ERR_UNKNOWNCOMMAND, clients[fd].getNick(), rawCmd));
			message_server.clear();
			continue ;
		}

		Params c = message_server.getParams();
		size_t colonPos = msg.find(" :");
		if (colonPos != std::string::npos)
		{
			std::string realTrailing = msg.substr(colonPos + 2);
			while (!realTrailing.empty() && (realTrailing[realTrailing.size() - 1] == '\r' || realTrailing[realTrailing.size() - 1] == '\n'))
				realTrailing.erase(realTrailing.size() - 1);
			if (!c.empty() && c.back() == "\n")
				c.back() = realTrailing;
			else if (c.empty() && !realTrailing.empty())
				c.push_back(realTrailing);
		}

		checkComands(fd, c);
		message_server.clear();
	}
}

void Server::acceptNewClient(void)
{
	struct sockaddr_in clientAddr;
	socklen_t addrLen = sizeof(clientAddr);
	int newSocket = accept(serverSocket, reinterpret_cast<struct sockaddr *>(&clientAddr), &addrLen);
	if (newSocket < 0)
		return ;
	try
	{
		nonBlocking(newSocket);
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		close(newSocket);
		return ;
	}
	Client newClient = Client::createClient(newSocket);
	newClient.setHostname(inet_ntoa(clientAddr.sin_addr));
	clients[newSocket] = newClient;
	pollFds.push_back(addNewSocket(newSocket));
}

bool Server::handleClientRead(size_t index)
{
	char buffer[1024];
	int clientFd = pollFds[index].fd;
	int reciveBytes = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
	if (reciveBytes <= 0)
	{
		disconnectClient(index);
		return (false);
	}
	buffer[reciveBytes] = '\0';
	clients.at(clientFd).insert(std::string(buffer, reciveBytes));
	extractMessage(clientFd);
	if (!clients.count(clientFd))
		return (false);
	return (true);
}

bool Server::handleClientWrite(size_t index)
{
	int fd = pollFds[index].fd;
	if (!clients.count(fd))
	{
		pollFds[index].events &= ~POLLOUT;
		return (true);
	}
	std::string &out = clients[fd].getOutputBuffer();
	if (out.empty())
	{
		pollFds[index].events &= ~POLLOUT;
		return (true);
	}
	ssize_t sent = send(fd, out.c_str(), out.size(), MSG_DONTWAIT);
	if (sent < 0)
	{
		disconnectClient(index);
		return (false);
	}
	out.erase(0, static_cast<size_t>(sent));
	if (out.empty())
		pollFds[index].events &= ~POLLOUT;
	return (true);
}

void Server::run(void)
{
	while (!reciveSignal)
	{
		for (size_t i = 0; i < pollFds.size(); i++)
		{
			int fd = pollFds[i].fd;
			if (fd != serverSocket && clients.count(fd))
			{
				if (!clients[fd].getOutputBuffer().empty())
					pollFds[i].events |= POLLOUT;
				else
					pollFds[i].events &= ~POLLOUT;
			}
		}
		int ret = poll(&pollFds[0], pollFds.size(), -1);
		if (ret < 0)
		{
			if (errno == EINTR)
				continue ;
			throw std::runtime_error(std::string("poll failure: ") + strerror(errno));
		}
		for (size_t i = 0; i < pollFds.size(); i++)
		{
			if (pollFds[i].revents & (POLLHUP | POLLERR))
			{
				disconnectClient(i);
				--i;
				continue ;
			}
			if (pollFds[i].revents & POLLIN)
			{
				if (pollFds[i].fd == serverSocket)
					acceptNewClient();
				else if (!handleClientRead(i))
				{
					--i;
					continue ;
				}
			}
			if (pollFds[i].revents & POLLOUT)
			{
				if (!handleClientWrite(i))
				{
					--i;
					continue ;
				}
			}
		}
	}
	std::cout << std::endl << "Shutting down server" << std::endl;
}
