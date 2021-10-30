#pragma once
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#ifdef WIN32
#pragma comment(lib, "Ws2_32.lib")
#include <io.h>
#include <process.h>
#include <WS2tcpip.h>
#include <winsock.h>
#include <WinSock2.h>
typedef unsigned int uint;
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif // WIN32
#include <vector>
#include <errno.h>
#include <thread>
#include "client_observer.h"
#include "pipe_ret_t.h"
#include <ThemidaSDK.h>
#include "../../Utilities/Security/xorstr.hpp"

class TcpClient
{
private:
	int m_sockfd = 0;
	bool stop = false;
	struct sockaddr_in m_server;
	std::vector<client_observer_t> m_subscibers;
	std::thread* m_receiveTask = nullptr;

	void publishServerMsg(const char* msg, size_t msgSize);
	void publishServerDisconnected(const pipe_ret_t& ret);
	void ReceiveTask();
	void terminateReceiveThread();

public:
	~TcpClient();
	pipe_ret_t connectTo(const std::string& address, int port);
	pipe_ret_t sendMsg(const char* msg, size_t size);
	pipe_ret_t sendBytes(std::vector<unsigned char> bytes);

	void subscribe(const client_observer_t& observer);
	void unsubscribeAll();
	void publish(const char* msg, size_t msgSize);

	pipe_ret_t finish();
};