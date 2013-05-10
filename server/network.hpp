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
#ifndef NETWORK_HPP_
#define NETWORK_HPP_

#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#include <winsock2.h>
#include <ws2tcpip.h>

void NetworkInit();
void NetworkQuit();

class TCPSocket {
public:
	TCPSocket();
	TCPSocket(const char* ip, int port);
	~TCPSocket();

	/* param 1: ip address to connect to
	 * param 2: port to open the server socket on
	*/
	void Open(const char* ip, int port);
	void Close();

	/* Send() and Receive()
	 * param 1: data to be sent/received
	 * param 2: length/maxlength of the data
	 * param 3: flags to the internal function
	 * return:
	 *     the amount of data sent/received (not necessarily the same value as len/maxlen)
	*/
	int Send(const void* data, int len, int flags = 0);
	int Recv(void* data, int maxlen, int flags = 0);
private:
	SOCKET sock;
	friend class TCPServerSocket;
};

class TCPServerSocket {
public:
	TCPServerSocket();
	TCPServerSocket(int port);
	~TCPServerSocket();

	void Open(int port);
	void Close();

	int Accept(TCPSocket*, int uSeconds = 0);
private:
	SOCKET sock;
};

//TODO: Write the UDP systems
//
//class UDPRemote {
//public:
//	UDPRemote();
//	UDPRemote(const char* ip, int port);
//
//	/* param 1: ip of the remote to connect to, null to clear
//	 * param 2: port of the remote to connect to
//	*/
//	void Set(const char* ip, int port);
//	//TODO: Get?
//private:
//	sockaddr addr;
//	friend class UDPSocket;
//};
//
//class UDPSocket {
//public:
//	UDPSocket();
//	UDPSocket(int port);
//	~UDPSocket();
//
//	int Open(int port);
//	void Close();
//
//	int Send(const void* data, int len, UDPRemote* rem, int flags = 0);
//	int Recv(void* data, int maxlen, UDPRemote* rem, int flags = 0);
//private:
//	SOCKET sock;
//};

#endif
