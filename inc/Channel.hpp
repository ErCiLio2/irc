/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eteixeir <eteixeir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 19:14:34 by eteixeir          #+#    #+#             */
/*   Updated: 2026/11/23 16:06:34 by eteixeir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <stdint.h>
#include <set>
#include <string>
#include "Client.hpp"

class Channel {
	private:
		std::string name;
		std::string	topic;
		std::string	password;
		uint16_t	channel_mode;
		size_t		members_limit;
		std::set <Client*> members; //I will search, it beate
		std::set <Client*> invites;
		std::set<Client*> operators;

	public:
		typedef std::set<Client*>::iterator iterator;

	public:
		Channel();
		Channel(const std::string &channel_name, Client* op);
		Channel(const Channel &other);
		std::set <Client*> &getMembersList();
		std::set <Client*> &getInvitationList();
		std::set <Client*> &getMembersOperatorsList();

	public:
		void	addMember(Client *user, const std::string &pwd);
		void 	addMember(Client* user); // Or throw an exception like with an exception
		void 	modifyMemberRole(Client* user, bool promote = true);
		void 	toggleMode(uint16_t mode, bool reverse = false);
		void 	kickMember(Client *&_operator, Client* user);
		void	inviteClient(Client* client);
		void	sendMultCastMessage(Client* sender, const std::string &message);
		size_t	membersSize() const;
		bool	hasClient(Client *user);

	public:
		bool isOperator(Client *user) const;
		bool isMember(Client *user) const;
		bool isInInvites(Client *user) const;

	public:
		void	eraseClient(Client *client);
		void	eraseFromInvatation(Client *client);

	public:
		void setModes(uint16_t modes);
		void setLimit(int limit);
		void setTopic(std::string const &str);
		void setTopic(Client *user, std::string const &str);
		void setName(std::string const &str);
		void setPassword(const std::string &str);

	public:
		uint16_t getModes() const;
		std::string getName() const;
		std::string getTopic() const;
		size_t	getLimit() const;
		std::string getPassword() const;
	public:
		void clearInviteList();
		void clearMemberAndOperatorsList();
		
	public:
		~Channel(void);
		static Channel createChannel(const std::string &channel_name, Client* op);

	public:
		struct Mode {
		    enum {
		        T = 1,
		        K = 2,
		        O = 4,
		        L = 8,
		        I = 16
		    };
		};
};

#endif
