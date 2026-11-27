/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eteixeir <eteixeir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 18:17:44 by eteixeir          #+#    #+#             */
/*   Updated: 2026/11/27 03:29:05 by eteixeir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Message.hpp"

//<message>  ::= <command> <params> <crlf>
void Message::parse(const std::string &raw) {
    if (!this->init(raw)) {
        return ;
    }

    command();
    params();
    crlf();
}

//<command>  ::= <letter> { <letter> }
void Message::command(void) {
    std::string line = tokenize(Current);

    if (line[0] == ' ') {
        throw ParserError("COMMAND With Leading Whitespace\n");
    }
    
    for (size_t i = 0; i < line.length(); ++i) {
        if (!isalpha(line[i])) {
            throw ParserError("COMMAND With Non-Alpha Characters\n");
        }
    }

    _command = line;
    tokenize(Next);
}

//<params>   ::= <SPACE> [ ':' <trailing> | <middle> <params> ]
void Message::params(void) {
    std::string line = tokenize(Current);

    if (line == CRLF || line.empty()) {
        return ;
    }

    if (line[0] == ':') {
        line = &tokenize(Trailing)[1];
        tokenize(Next);

        if (!line.empty()) {
            _params.push_back(line);
        }

        return ;
    }

    _params.push_back(line);
    tokenize(Next);
    params();
}

//<crlf>     ::= CR LF
void Message::crlf(void) {
    if (tokenize(Current) == CRLF) {
        return ;
    }

    throw ParserError("CRLF Terminator Not Found\n");
}

bool    Message::init(const std::string &raw) {
    if (raw.empty() || raw[0] == ' ') {
        return (false);
    }

    this->start = 0;
    this->trailing = 0;
    this->cr_lf = "";
    this->raw_parser = raw;
    this->end = raw.find(" ");
    tokenize(Next);

    return (true);
}

std::string   Message::tokenize(tokenFlag flag) {
    size_t pos = 0;
    std::string delimiter = " ";

    if (flag == Current) {
        return (this->token);
    }
    
    if (!this->cr_lf.empty()) {
        token = cr_lf;
        return (this->cr_lf);
    }

    if (flag == Trailing) {
        this->token = raw_parser.substr(trailing);
    } else if (end == std::string::npos) {
        this->token = raw_parser.substr(start);
    } else {
        token = raw_parser.substr(start, end - start);
    }

    trailing = start;
    pos = token.find_first_of(CRLF);
    if (pos != std::string::npos) {
        cr_lf = CRLF;
        token = token.substr(0, pos);
        return (this->token);
    } else if (end == std::string::npos) {
        return (this->cr_lf);
    }

    start = delimiter.length() + end;
    end =  raw_parser.find(delimiter, start);

    return (this->token); 
}

void Message::clear(void) {
    this->end = 0;
    this->start = 0;
    this->trailing = 0;
    this->raw_parser = "";
    this->cr_lf = "";
    this->_command = "";
    this->_params.clear();
}

Params      Message::getParams(void) {
    return (_params); 
}

std::string Message::getCommand(void) {
    return (_command);
}

std::string Message::getRaw(void) {
    return (raw_parser);
}
