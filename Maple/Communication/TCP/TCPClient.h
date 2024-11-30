#pragma once

#include <mutex>
#include <winsock2.h>
#include <string>
#include <vector>
#include <thread>

#include <functional>

class TCPClient
{
	static inline unsigned int constexpr BUFFER_LENGTH = 8192;
	static inline unsigned int constexpr PACKET_HEADER_SIZE = 8;
	static inline unsigned int constexpr PACKET_HEADER_SIGNATURE = 0xdeadbeef;

	static inline std::mutex sendMutex;

	bool connected = false;
	SOCKET m_socket;
	std::thread* m_receiveThread = nullptr;

	std::function<void(const std::vector<unsigned char>&)> m_ReceiveCallback;
	std::function<void()> m_DisconnectedCallback;

	std::vector<unsigned char> receiveStreamData;
	bool isReceiving = false;
	int receiveStreamLength;
	int receiveStreamRemainingLength;

	void receiveThread();
	void receive(char* buffer, int bytesReceived);
public:
	TCPClient() = default;
	TCPClient(const std::function<void(const std::vector<unsigned char>&)>& receiveCallback, const std::function<void()>& disconnectedCallback);
	~TCPClient();

	bool Connect(const std::string& host, const std::string& port);
	void Disconnect();
	void Send(const std::vector<unsigned char>& data);
};