/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eteixeir <eteixeir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 11:40:26 by eteixeir          #+#    #+#             */
/*   Updated: 2026/11/27 03:27:32 by eteixeir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Channel.hpp"

Client::Client(int fd) {
	registered = 0;
	already_sent = 0;
	my_fd = fd;
	old_line_continuation = false;
}

Client::Client() {
	registered = 0;
	already_sent = 0;
	my_fd = -1;
	old_line_continuation = false;
}

Client::Client(const Client &other)
    : old_line_continuation(other.old_line_continuation),
	  my_fd(other.my_fd),
      registered(other.registered),
      already_sent(other.already_sent),
      nick(other.nick),
      username(other.username),
      hostname(other.hostname),
      realname(other.realname),
      servername(other.servername),
      recv_buffer(other.recv_buffer),
      send_buffer(other.send_buffer),
      my_channels(other.my_channels),
      inbox(other.inbox)
{
}

Client & Client::operator=(const Client &other) {
	if (this != &other) {
		nick = other.nick;
		username = other.username;
		hostname = other.hostname;
		realname = other.realname;
		servername = other.servername;
		recv_buffer = other.recv_buffer;
		send_buffer = other.send_buffer;
		my_channels = other.my_channels;
		inbox = other.inbox;
		registered = other.registered;
		already_sent = other.already_sent;
		my_fd = other.my_fd;
		old_line_continuation = other.old_line_continuation;
	}
	return (*this);
}

Client::~Client(void) {
}

void Client::setNick(const std::string &str) {
	this->nick = str;
	registered |= NICK_MASK;
}

void Client::setUsername(const std::string &str) {
	this->username = str;
	registered |= USER_MASK;
}

void Client::setRealName(const std::string &str) {
	this->realname = str;
	registered |= REAL_NAME_MASK;
}

void Client::setServerName(const std::string &str) {
	this->servername = str;
}

void Client::setHostname(const std::string &str) {
	this->hostname = str;
}

void Client::passwordWasProvided(bool yes) {
	if (yes)
		registered |= PASS_MASK;
}

const std::string &Client::getNick(void) const { return (nick); }

int	  Client::getMyFd(void) const{return (my_fd); }

std::set<Channel*> &Client::getMyChannels(){return(my_channels);}

std::set<Channel*> &Client::getMyInbox(){return (inbox); }

const std::string &Client::getUser() const { return (username); }

const std::string &Client::getHost() const { return (hostname); }

const std::string &Client::getRealName() const { return (realname); }

const std::string &Client::getServerName() const { return (servername); }


const std::string &Client::getBuffer(void) const {
	return (recv_buffer);
}

std::string &Client::getOutputBuffer(void) {
	return (send_buffer);
}

void Client::insert(char c) {
	recv_buffer.append(1, c);
}

void Client::insert(const std::string &str) {
	recv_buffer.append(str);
}

void Client::clearInputBuffer(void) {
	recv_buffer.clear();
}


void Client::clearOutputBuffer(void) {
	send_buffer.clear();
	already_sent = 0;
}

bool Client::isRegistered() const {
 return (registered == (PASS_MASK | NICK_MASK | USER_MASK | REAL_NAME_MASK));
}

std::string Client::extractMessage() {
	while (true) {
		size_t pos = recv_buffer.find_first_of(CR LF);
		if (pos == std::string::npos) {
			if (recv_buffer.size() > MAX_INDEX_SIZE) {
				old_line_continuation = true;
				recv_buffer.clear();
			}
			return ("");
		}
		size_t end = pos + 1;
		if (recv_buffer[pos] == '\r' && end < recv_buffer.size() && recv_buffer[end] == '\n')
			++end;
		if (old_line_continuation || pos > MAX_INDEX_SIZE) {
			old_line_continuation = false;
			recv_buffer.erase(0, end);
			continue ;
		}
		std::string temp = recv_buffer.substr(0, end);
		recv_buffer.erase(0, end);
		return (temp);
	}
}

void	Client::unRegister() {
	this->registered ^= this->registered;
}

Client Client::createClient(int fd) {
	return Client(fd);
}

void Client::setOutputBuffer(const std::string &str) {
	this->send_buffer = str;
	already_sent = 0;
}

ssize_t	Client::textMe() {
	return (static_cast<ssize_t>(send_buffer.size()));
}

ssize_t	Client::textMe(const std::string &str) {
	std::string msg = str;
	if (msg.size() > MAX_MESSAGE_SIZE)
		msg = msg.substr(0, MAX_INDEX_SIZE) + "\r\n";
	this->send_buffer += msg;
	return (static_cast<ssize_t>(msg.size()));
}


void Client::sendInviteToInbox(Channel *channel) {
	this->inbox.insert(channel);
}

void Client::registerMyChannel(Channel *channel) {
	this->my_channels.insert(channel);
}



void Client::removeChannelFromInbox(Channel *channel) {
	this->inbox.erase(channel);
}

void Client::removeChannelFromRegister(Channel *channel) {
	this->my_channels.erase(channel);
}
