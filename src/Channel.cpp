/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eteixeir <eteixeir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 19:14:31 by eteixeir          #+#    #+#             */
/*   Updated: 2026/11/27 03:29:33 by eteixeir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Channel.hpp"

Channel Channel::createChannel(const std::string &channel_name, Client* op) {
	//Maybe I need to verify the channel's name and the client op or throw an exception
	if (channel_name.empty())
		throw std::runtime_error("Channel operator or channel name can not be null");
	return Channel(channel_name, op);
}

Channel::Channel() {
	channel_mode = 0;
	members_limit = -1;	
}

Channel::Channel(const Channel &other)
    : name(other.name),
      topic(other.topic),
      channel_mode(other.channel_mode),
      members_limit(other.members_limit),
      members(other.members),
      invites(other.invites),
      operators(other.operators)
{
}

Channel::Channel(const std::string &channel_name, Client* op) {
	this->name = channel_name;
	operators.insert(op);
	members.insert(op);
	this->channel_mode = 0;
	members_limit = -1;
}

/***
 * 
 *  o CANAL SO MORRE(delete) QUANDO nao ha usuarios, logo vou assumir que o delete ou o destructor so vai ser chamado nestes casos
 *  Caso isso nao seja verdade, ha um problema
 * 
*/

Channel::~Channel() {
}

std::string Channel::getName() const {
	return (this->name);
}

std::string Channel::getPassword() const {
	return (this->password);
}

size_t Channel::getLimit(void) const {
	if (channel_mode & Mode::L)
		return (members_limit);
	return (-1);
}

std::string	Channel::getTopic() const {
	return (this->topic);
}

uint16_t Channel::getModes() const {
	return (this->channel_mode);
}

void Channel::setModes(uint16_t modes) {
	this->channel_mode = modes;
}

void	Channel::setLimit(int limit) {
	if (limit <= 0)
		return ;
	this->channel_mode |= Mode::L;
	this->members_limit = limit;
}

void	Channel::setTopic(std::string const &str) {
	this->topic = str;
}

void	Channel::setPassword(std::string const &str) {
	this->password = str;
}

void	Channel::setTopic(Client *user, std::string const &str) {
	if (operators.count(user) || (members.count(user) && !(channel_mode & Mode::T)))
		this->topic = str;
	else
		throw std::runtime_error("Error 482"); // Pensei no seguinte, eu lanco o numero do erro, tu convertes para inteiro, e tomas as decisoes. Pode ser wi(eteixeir)?
}

void	Channel::setName(std::string const &str) {
	this->name = str;
}

void	Channel::addMember(Client* user) {
	if ( (channel_mode & Mode::L) && members.size() >= members_limit) {
		std::cerr << (channel_mode & Mode::L) << std::endl;
		throw std::runtime_error("TOO_MANY_MEMBERS");
	}
	if ((channel_mode & Mode::I) && invites.count(user) < 1)
		throw std::runtime_error("THE USER HASH NOT INVATATION return 473");
	Channel *tmp = this;
	members.insert(user);
	if (invites.count(user)) {
		user->removeChannelFromInbox(tmp);
		invites.erase(user);
	}
	user->registerMyChannel(tmp);
}

void	Channel::addMember(Client* user, const std::string &pwd) {
	if ((channel_mode & Mode::K) && pwd  != password) //475)
		throw std::runtime_error("THE USER HASH NOT INVATATION INVALID PASSWORD return 475");
	addMember(user);
}

void 	Channel::modifyMemberRole(Client* user, bool promote) {
	iterator el;

	el = members.find(user);
	if (el == members.end())
		return ; //Or throw an exception
	if (promote)
		operators.insert(*el);
	else if (operators.find(user) != operators.end())
		operators.erase(user);
}

void 	Channel::toggleMode(uint16_t mode, bool reverse) {
	if (reverse)
		this->channel_mode &= (~mode);
	else
		this->channel_mode |= mode;
}

void 	Channel::kickMember(Client *&_operator, Client* user) {
	if (operators.count(_operator) > 0)
		members.erase(user);
	//Or throw an exception
}

void	Channel::inviteClient(Client* client) {
	this->invites.insert(client);
	Channel *tmp = this;
	client->sendInviteToInbox(tmp);
}

void	Channel::sendMultCastMessage(Client* sender, const std::string &message) {

	iterator begin = members.begin();
	iterator end = members.end();
	while (begin != end) {
		if (*begin != sender)
			(*begin)->textMe(message);
		++begin;
	}
}

std::set <Client*> &Channel::getMembersList() {
	return (this->members);
}

std::set <Client*> &Channel::getInvitationList() {
	return (this->invites);
}

std::set <Client*> &Channel::getMembersOperatorsList() {
	return (this->operators);
}

size_t	Channel::membersSize() const {
	return (members.size());
}

/**
 * 
 * Only the server can use these functions
 * 
 */

void	Channel::clearInviteList() {
	iterator it;
	Channel	*tmp = this;

	it = invites.begin();
	while (it != invites.end()) {
		Client *client = *it;
		client->removeChannelFromInbox(tmp);
		++it;
	}
}

void Channel::clearMemberAndOperatorsList() {
	iterator it;
	Channel	*tmp = this;

	it = members.begin();
	while (it != members.end()) {
		Client *client = *it;
		client->removeChannelFromRegister(tmp);
		++it;
	}

	it = operators.begin();
	while (it != operators.end()) {
		Client *client = *it;
		client->removeChannelFromRegister(tmp);
		++it;
	}
}

void	Channel::eraseClient(Client *client) {
	this->members.erase(client);
	this->operators.erase(client);
}

void	Channel::eraseFromInvatation(Client *client) {
	this->invites.erase(client);
}

bool	Channel::hasClient(Client *user) {
	return this->members.count(user);
}

bool Channel::isOperator(Client *user) const {
	return (operators.count(user));
}

bool Channel::isMember(Client *user) const {
	return (members.count(user));
}

bool Channel::isInInvites(Client *user) const {
	return (invites.count(user));
}
