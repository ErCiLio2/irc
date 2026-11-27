*This project has been created as part of the 42 curriculum by eteixeir, dmanico, mmuzembela.*

# ft_irc

## Description

ft_irc is our C++98 implementation of an Internet Relay Chat (IRC) server for the 42 curriculum. It lets several clients connect to the same server, exchange messages, and talk in shared channels.

While building it, we work with TCP sockets, non-blocking I/O multiplexing with `poll()`, client/server communication, and the rules that IRC clients and servers use to exchange messages.

The server keeps track of connected clients and channels, interprets incoming commands, and forwards messages to the right users.

We also built a bonus IRC bot (`ircbot`) that connects to the server like any other client, joins the `#bot` channel, listens for messages, and replies to supported commands.

### Project Goals

- Get practical experience with TCP/IP and socket programming.
- Build an IRC server that follows the required C++98 standard.
- Support several clients connected at the same time using `poll()` and non-blocking sockets.
- Parse IRC messages and handle the required commands.
- Keep track of channel membership, operators, and channel modes (`i`, `t`, `k`, `o`, `l`).
- Keep the code split into clear, maintainable components.
- Build and test a bonus bot that connects separately to the server.

---

## Features

### Core Features

- TCP server using IPv4 sockets and non-blocking I/O (`poll()`).
- Multiple simultaneous client connections.
- Client registration using `PASS`, `NICK`, and `USER`.
- Channel creation and management.
- Joining and leaving channels.
- Private messages (`PRIVMSG` and `NOTICE`) between users and channels.
- IRC command parsing and processing (including partial message buffering).
- Client and channel state management.
- `PING`/`PONG` and `QUIT` handling.
- Error handling, numeric replies, and graceful signal shutdown (`SIGINT`, `SIGQUIT`).

### Channel Management

- `JOIN`
- `PART`
- `PRIVMSG` / `NOTICE`
- `KICK`
- `INVITE`
- `TOPIC`
- `MODE` (`+i/-i`, `+t/-t`, `+k/-k`, `+o/-o`, `+l/-l`)

> Available commands and permission checks are limited to what we implemented for this project.

### IRC Bot (Bonus)

The project includes a standalone IRC bot (`ircbot`) capable of:

- Connecting and authenticating to the IRC server over TCP.
- Automatically joining the `#bot` channel as `bot`.
- Listening for channel messages and responding to commands (`!ping` and `!help`).

---

## Technical Choices

### Programming Language

- C++98

We use C++98 to match the requirements of the 42 `ft_irc` subject.

### Networking

- TCP/IP (IPv4)
- POSIX sockets and I/O multiplexing:
  - `socket()`, `setsockopt()`, `fcntl()`
  - `bind()`, `listen()`, `accept()`, `connect()`
  - `poll()`
  - `send()`, `recv()`, `close()`

### Server Architecture

The main responsibilities are separated into dedicated components:

- **Server (`Server/`):** Manages the listening socket, `poll()` loop, connected clients, channels, and command execution.
- **Client (`Client/`):** Represents an IRC client, its connection state, authentication status, and input buffer.
- **Channel (`Channel/`):** Manages channel members, operators, invitations, topics, and modes.
- **Parser (`Parser/`):** Parses raw IRC protocol lines into prefixes, commands, and parameters (`Message`).
- **Utils (`Utils/`):** Builds standardized IRC numeric replies and error messages (`Replies`).
- **Bot (`Bot/`):** Standalone IRC client that connects to the server and responds to commands in `#bot`.

Keeping these parts separate makes the code easier to follow and gives each component a clear job.

---

## Project Structure

```text
ft_irc/
├── Makefile
├── README.md
├── main.cpp
├── Bot/
│   ├── Bot.cpp
│   ├── Bot.hpp
│   └── main.cpp
├── Channel/
│   ├── Channel.cpp
│   └── Channel.hpp
├── Client/
│   ├── Client.cpp
│   └── Client.hpp
├── Parser/
│   ├── Message.cpp
│   └── Message.hpp
├── Server/
│   ├── Server.cpp
│   └── Server.hpp
└── Utils/
    ├── Replies.cpp
    └── Replies.hpp
```

---

## Instructions

### Requirements

To compile and run the project, you need:

- A C++ compiler with C++98 support (`c++`).
- `make`.
- A Unix-like operating system with POSIX socket support.

### Compilation

Clone the repository:

```bash
git clone <repository-url>
cd ft_irc
```

Compile the IRC server (`ircserv`):

```bash
make
```

Compile both the IRC server (`ircserv`) and the bonus IRC bot (`ircbot`):

```bash
make bonus
```

### Clean Build Files

Remove object files:

```bash
make clean
```

Remove all generated files, including executables (`ircserv` and `ircbot`):

```bash
make fclean
```

Recompile the project:

```bash
make re
```

---

## Usage

### Start the IRC Server

```bash
./ircserv <port> <password>
```

Example:

```bash
./ircserv 6667 mypassword
```

Arguments:

- `<port>`: TCP port on which the IRC server listens (`1`–`65535`).
- `<password>`: Password required for client registration.

### Connect Using an IRC Client

You can use an IRC client such as `irssi`, `hexchat`, or `nc` (Netcat) for testing.

Example using Netcat:

```bash
nc -C 127.0.0.1 6667
```

After connecting, send the registration commands:

```text
PASS mypassword
NICK TestUser
USER testuser 0 * :Test User
```

After registration, try the commands supported by our server to join channels and exchange messages:

```text
JOIN #general
PRIVMSG #general :Hello everyone!
```

### Run the IRC Bot (Bonus)

With `ircserv` already running, start the bot in another terminal using the same port and password:

```bash
./ircbot <port> <password>
```

Example:

```bash
./ircbot 6667 mypassword
```

The bot connects to `127.0.0.1`, registers as `bot`, and joins `#bot`. From any connected client, join `#bot` and interact with it:

```text
JOIN #bot
PRIVMSG #bot :!ping
PRIVMSG #bot :!help
```

---

## IRC Protocol Overview

IRC clients and servers communicate by sending text-based protocol messages over a TCP connection, terminated by `\r\n`.

Most IRC messages follow this structure:

```text
[:prefix] COMMAND [parameters] [:trailing parameter]
```

Example:

```text
:nickname!user@host PRIVMSG #general :Hello everyone!
```

### Supported IRC Commands

| Command | Description |
|---|---|
| `PASS` | Provides the server connection password. |
| `NICK` | Sets or changes the client nickname. |
| `USER` | Specifies the username and real name during registration. |
| `JOIN` | Joins one or more channels (creating them if they do not exist). |
| `PART` | Leaves one or more channels. |
| `PRIVMSG` | Sends a message to a user or a channel. |
| `NOTICE` | Sends a notice message without triggering automatic replies. |
| `KICK` | Forcibly removes a user from a channel (channel operator only). |
| `INVITE` | Invites a user to a channel. |
| `TOPIC` | Views or changes the channel topic. |
| `MODE` | Views or changes channel modes (`i`, `t`, `k`, `o`, `l`). |
| `PING` | Tests connection liveness; the server replies with `PONG`. |
| `QUIT` | Disconnects the client from the server. |

---

## Resources

These references were used to learn about the IRC protocol, check command behavior, and implement socket multiplexing in C++98.

### IRC Protocol

- [RFC 1459 - Internet Relay Chat Protocol](https://www.rfc-editor.org/info/rfc1459) — The original IRC protocol specification, including message format and core server/client behavior.
- [RFC 2812 - Internet Relay Chat: Client Protocol](https://www.rfc-editor.org/info/rfc2812) — Updated client protocol specification and numeric replies reference.
- [Modern IRC Documentation - Living Specification](https://modern.ircdocs.horse/) — A practical reference for how contemporary IRC clients and servers interact.
- [IRC Commands Reference](https://dd.ircdocs.horse/refs/commands/) — Reference for checking command syntax, parameters, and numeric replies.

### Network Programming & C++98

- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) — Classic guide to POSIX sockets, non-blocking I/O, and `poll()`.
- [Wireshark Wiki - IRC](https://wiki.wireshark.org/IRC) — Useful for inspecting IRC traffic and packet captures.
- [ft_irc: Channels and Command Management](https://medium.com/@mohamedsarda/ft-irc-channels-and-command-management-ff1ff3758a0b) — Article discussing channel handling and command management in an `ft_irc` implementation.
- [cppreference - C++98 Standard Library](https://en.cppreference.com/w/cpp) — Reference for C++98 containers, streams, and standard library behavior.

### AI Usage

AI tools were used as a learning and debugging aid during the development of the project, specifically for the following tasks and parts:

- **Concepts and Protocol Research:** Clarifying RFC 1459 / RFC 2812 message formats, numeric reply codes (`RPL_*` and `ERR_*`), and the behavior of POSIX calls such as `poll()`, `fcntl()`, `setsockopt()`, `send()`, and `recv()`.
- **Server Architecture and Debugging:** Discussing edge cases in non-blocking socket buffers (such as handling partial commands split across multiple `recv()` calls and buffered `POLLOUT` writes in `Server/`) and reviewing channel mode transitions (`+i`, `+t`, `+k`, `+o`, `+l`).
- **Documentation:** Reviewing and structuring the `README.md` documentation to ensure clarity and compliance with the 42 curriculum requirements.

All code design decisions, implementation, integration, and testing were carried out and verified by the team.
