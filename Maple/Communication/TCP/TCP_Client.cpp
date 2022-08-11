#pragma once

#include "TCP_Client.h"

#pragma warning(disable : 4996)
#pragma optimize("", off)
typedef int (WINAPI *RecvPrototype) (SOCKET, char*, int, int);
typedef int (WINAPI *SendPrototype) (SOCKET, const char*, int, int);
pipe_ret_t TcpClient::connectTo(const std::string& address, int port)
{
	m_sockfd = 0;
	pipe_ret_t ret;

#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
		perror(xor("error"));
#endif // WIN32

	m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sockfd == -1) { //socket failed
		ret.success = false;
		ret.msg = strerror(errno);
		return ret;
	}

#ifdef WIN32
	int inetSuccess = inet_pton(AF_INET, address.c_str(), &m_server.sin_addr.s_addr);
#else
	int inetSuccess = inet_aton(address.c_str(), &m_server.sin_addr);
#endif // WIN32

	if (!inetSuccess) {
		struct hostent* host;
		struct in_addr** addrList;
		if ((host = gethostbyname(address.c_str())) == NULL) {
			ret.success = false;
			ret.msg = xor("Failed to resolve hostname");
			return ret;
		}
		addrList = (struct in_addr**)host->h_addr_list;
		m_server.sin_addr = *addrList[0];
	}
	m_server.sin_family = AF_INET;
	m_server.sin_port = htons(port);

	int connectRet = connect(m_sockfd, (struct sockaddr*) & m_server, sizeof(m_server));
	if (connectRet == -1) {
		ret.success = false;
		ret.msg = strerror(errno);
		return ret;
	}
	m_receiveTask = new std::thread(&TcpClient::ReceiveTask, this);
	ret.success = true;
	return ret;
}

pipe_ret_t TcpClient::sendMsg(const char* msg, size_t size)
{
	pipe_ret_t ret;
	VM_FISH_RED_START
	int numBytesSent = send(m_sockfd, msg, size , 0);
	VM_FISH_RED_END
	if (numBytesSent < 0) { // send failed
		ret.success = false;
		ret.msg = strerror(errno);
		return ret;
	}
	if ((uint)numBytesSent < size) { // not all bytes were sent
		ret.success = false;
		char msg[100];
		sprintf(msg, xor("Only %d bytes out of %lu was sent to client"), numBytesSent, size);
		ret.msg = msg;
		return ret;
	}
	ret.success = true;
	return ret;
}

pipe_ret_t TcpClient::sendBytes(std::vector<unsigned char> bytes)
{
	std::string msg;
	for (int i = 0; i < bytes.size(); i++) {
		byte b = bytes[i];
		msg += (char)b;
	}
	return sendMsg(msg.c_str(), bytes.size());
}

void TcpClient::subscribe(const client_observer_t& observer)
{
	m_subscibers.push_back(observer);
}

void TcpClient::unsubscribeAll()
{
	m_subscibers.clear();
}

void TcpClient::publishServerMsg(const char* msg, size_t msgSize)
{
	for (uint i = 0; i < m_subscibers.size(); i++) {
		if (m_subscibers[i].incoming_packet_func != NULL) {
			(*m_subscibers[i].incoming_packet_func)(msg, msgSize);
		}
	}
}

void TcpClient::publishServerDisconnected(const pipe_ret_t& ret)
{
	for (uint i = 0; i < m_subscibers.size(); i++) {
		if (m_subscibers[i].disconnected_func != NULL) {
			(*m_subscibers[i].disconnected_func)(ret);
		}
	}
}

void TcpClient::ReceiveTask()
{
	char* msg = static_cast<char*>(malloc(5000)); // smaller page of memory here, we won't expect huge streams
	char* msg_orig = msg;
	while (!stop) {
		memset(msg, 0, sizeof msg);
		int size;
		VM_FISH_RED_START
		int numOfBytesReceived = recv(m_sockfd, (char*)&size, 4, 0);
		VM_FISH_RED_END
		//int numOfBytesReceived = recv(m_sockfd, (char*)&size, 4, 0);
		int orig_size = size;
		if (numOfBytesReceived < 1) {
			pipe_ret_t ret;
			ret.success = false;
			stop = true;
			if (numOfBytesReceived == 0) { //server closed connection
				ret.msg = xor("scc");
			}
			else {
				ret.msg = strerror(errno);
			}
			publishServerDisconnected(ret);
			finish();
			break;
		}
		else {
			while (true)
			{
				VM_FISH_RED_START
				numOfBytesReceived = recv(m_sockfd, msg, size, 0);
				VM_FISH_RED_END
				//numOfBytesReceived = recv(m_sockfd, msg, size, 0);
				if (numOfBytesReceived < 1) {
					pipe_ret_t ret;
					ret.success = false;
					stop = true;
					if (numOfBytesReceived == 0) { //server closed connection
						ret.msg = xor("scc");
					}
					else {
						ret.msg = strerror(errno);
					}
					publishServerDisconnected(ret);
					finish();
					break;
				}
				else {
					size -= numOfBytesReceived;
					msg += numOfBytesReceived;
					if (size == 0) {
						publishServerMsg(msg_orig, orig_size);
						msg = msg_orig;
						break;
					}
				}
			}
		}
	}
}

pipe_ret_t TcpClient::finish()
{
	stop = true;
	terminateReceiveThread();
	pipe_ret_t ret;
#ifdef WIN32
	if (closesocket(m_sockfd) == -1) { // close failed
#else
	if (close(m_sockfd) == -1) { // close failed
#endif // WIN32
		ret.success = false;
		ret.msg = strerror(errno);
		return ret;
	}
	ret.success = true;
	return ret;
	}

void TcpClient::terminateReceiveThread()
{
	if (m_receiveTask != nullptr) {
		m_receiveTask->detach();
		delete m_receiveTask;
		m_receiveTask = nullptr;
	}
}

TcpClient::~TcpClient()
{
	terminateReceiveThread();
}