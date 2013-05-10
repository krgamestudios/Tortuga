/* Copyright: (c) Kayne Ruse 2013
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 
 * 3. This notice may not be removed or altered from any source
 * distribution.
*/
#include "network.hpp"

#include <stdexcept>
#include <cstdio>

/* TCPSocket definition
*/

TCPSocket::TCPSocket() {
	sock = INVALID_SOCKET;
}

TCPSocket::TCPSocket(const char* ip, int port) {
	Open(ip, port);
}

TCPSocket::~TCPSocket() {
	Close();
}

void TCPSocket::Open(const char* ip, int port) {
	addrinfo *ptr = nullptr, hints;
	char buf[100];
	sprintf(buf, "%d",port); //std compliant itoa()

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if (getaddrinfo(ip, buf, &hints, &ptr)) {
		throw(std::runtime_error("TCPSocket failed to access address info"));
	}

	sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (sock == INVALID_SOCKET) {
		freeaddrinfo(ptr);
		throw(std::runtime_error("Failed to create a TCPSocket"));
	}

	bool connected = false;
	for(addrinfo *it = ptr; it; it = it->ai_next) {
		if (!connect(sock, it->ai_addr, it->ai_addrlen)) {
			connected = true;
			break;
		}
	}

	freeaddrinfo(ptr);

	if (!connected) {
		closesocket(sock);
		sock = INVALID_SOCKET;
		throw(std::runtime_error("Failed to connect a TCPSocket"));
	}
}

void TCPSocket::Close() {
	closesocket(sock);
	sock = INVALID_SOCKET;
}

int TCPSocket::Send(const void* data, int len, int flags) {
	if (sock == INVALID_SOCKET) {
		throw(std::runtime_error("Failed to send, TCPSocket is invalid"));
	}
	int ret = send(sock, (const char*)data, len, flags);
	if (ret == SOCKET_ERROR) {
		Close();
		throw(std::runtime_error("Failed to send, unknown error, TCPSocket automatically closed"));
	}
	return ret;
}

int TCPSocket::Recv(void* data, int maxlen, int flags) {
	if (sock == INVALID_SOCKET) {
		throw(std::runtime_error("Failed to receive, TCPSocket is invalid"));
	}
	int ret = recv(sock, (char*)data, maxlen, flags);
	if (ret == SOCKET_ERROR) {
		Close();
		throw(std::runtime_error("Failed to receive, unknown error, TCPSocket automatically closed"));
	}
	return ret;
}

/* TCPServerSocket definition
*/

TCPServerSocket::TCPServerSocket() {
	sock = INVALID_SOCKET;
}

TCPServerSocket::TCPServerSocket(int port) {
	Open(port);
}

TCPServerSocket::~TCPServerSocket() {
	Close();
}

void TCPServerSocket::Open(int port) {
	addrinfo *ptr = nullptr, hints;
	char buf[100];
	sprintf(buf, "%d",port); //std compliant itoa()

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(nullptr, buf, &hints, &ptr)) {
		throw(std::runtime_error("TCPServerSocket failed to access address info"));
	}

	sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (sock == INVALID_SOCKET) {
		freeaddrinfo(ptr);
		throw(std::runtime_error("Failed to create a TCPServerSocket"));
	}

	if (bind(sock, ptr->ai_addr, ptr->ai_addrlen) == SOCKET_ERROR) {
		closesocket(sock);
		freeaddrinfo(ptr);
		throw(std::runtime_error("Failed to bind TCPServerSocket"));
	}

	freeaddrinfo(ptr);
}

void TCPServerSocket::Close() {
	closesocket(sock);
}

int TCPServerSocket::Accept(TCPSocket* s, int uSeconds) {
	if (listen(sock, SOMAXCONN) == SOCKET_ERROR) {
		throw(std::runtime_error("TCPServerSocket: listen() error"));
	}

	//file descriptor sets are to prevent blocking
	fd_set readfds;
	timeval tv = {0, uSeconds};

	FD_ZERO(&readfds);
	FD_SET(sock, &readfds);

	if (select(0, &readfds, nullptr, nullptr, &tv) == SOCKET_ERROR) {
		throw(std::runtime_error("TCPServerSocket: select() error"));
	}

	//I don't want this to block
	if (FD_ISSET(sock, &readfds)) {
		if ((s->sock = accept(sock, nullptr, nullptr)) == INVALID_SOCKET) {
			throw(std::runtime_error("TCPServerSocket: accept() error"));
		}
		return 1;
	}
	return 0;
}
