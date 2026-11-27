/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eteixeir <eteixeir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 18:04:57 by eteixeir          #+#    #+#             */
/*   Updated: 2026/11/23 16:08:07 by eteixeir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <iostream>
# include <vector>
# include <limits>
# include <exception>


# define CRLF "\r\n"

typedef std::vector<std::string> Params;


class Message {
    public:
        class ParserError: public std::exception {
            private:
                std::string _error;
            
            public:
                ParserError(const std::string &error) : _error("Parser Error: " + error) {};
                ~ParserError(void) throw() {};
                const char *what(void) const throw() { return (_error.c_str()); };
        };

    private:
        enum tokenFlag { Current, Next, Trailing };
        size_t              end;
        size_t              start;
        size_t              trailing;
        std::string         raw_parser;
        std::string         token;
        std::string         cr_lf;
        std::string         _command;
        Params              _params;

    private:
        void                crlf(void);
        void                params(void);
        void                command(void);
        bool                init(const std::string &raw);
        std::string         tokenize(tokenFlag flag);

    public:
        void                clear(void);
        void                parse(const std::string &raw);
        std::string         getRaw(void);
        Params              getParams(void);
        std::string         getCommand(void);
        Message(void) {}
        ~Message(void) {}

};

#endif
