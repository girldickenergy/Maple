#pragma once

#include <winsock2.h>
#include <string>
#include <vector>
#include <thread>

class TCPClient
{
	static inline unsigned int constexpr BUFFER_LENGTH = 4096;
	static inline unsigned int constexpr PACKET_HEADER_SIZE = 8;
	static inline unsigned int constexpr PACKET_HEADER_SIGNATURE = 0xdeadbeef;

	bool connected = false;
	SOCKET m_socket;
	std::thread* m_receiveThread = nullptr;

	typedef void (*fn_receiveCallback)(const std::vector<unsigned char>& data);
	fn_receiveCallback receiveCallback = nullptr;

	typedef void (*fn_disconnectedCallback)();
	fn_disconnectedCallback disconnectedCallback = nullptr;

	std::vector<unsigned char> receiveStreamData;
	bool isReceiving = false;
	int receiveStreamLength;
	int receiveStreamRemainingLength;

	void receiveThread();
public:
	TCPClient() = default;
	TCPClient(fn_receiveCallback receiveCallback, fn_disconnectedCallback disconnectedCallback);
	~TCPClient();

	bool Connect(const std::string& host, const std::string& port);
	void Disconnect();
	void Send(const std::vector<unsigned char>& data);
};