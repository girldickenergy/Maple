#include "TCPClient.h"

#include <ws2tcpip.h>
#include "../../Utilities/Security/Security.h"
#include <xorstr.hpp>

void TCPClient::receiveThread()
{
    while (connected)
    {
        char buffer[BUFFER_LENGTH];

        auto handle = Security::HdnGetModuleBase(xorstr_("ws2.dll"));
        auto func = Security::HdnGetProcAddress(handle, xorstr_("recv"));
        typedef int(__stdcall* recvType)(SOCKET, char*, int, int);
        const int bytesReceived = reinterpret_cast<recvType>(func)(m_socket, buffer, BUFFER_LENGTH, 0);

        if (bytesReceived <= 0)
        {
            if (disconnectedCallback)
                disconnectedCallback();

            Disconnect();

            return;
        }

		receive(buffer, bytesReceived);
    }
}

void TCPClient::receive(char* buffer, int bytesReceived)
{
    if (!isReceiving && *reinterpret_cast<unsigned int*>(buffer) == PACKET_HEADER_SIGNATURE)
    {
        isReceiving = true;
        receiveStreamLength = *reinterpret_cast<int*>(buffer + sizeof(unsigned int)) + PACKET_HEADER_SIZE;
        receiveStreamRemainingLength = receiveStreamLength;
    }

    receiveStreamRemainingLength -= (std::min)(bytesReceived, receiveStreamLength);
    receiveStreamData.insert(receiveStreamData.end(), buffer, buffer + (std::min)(bytesReceived, receiveStreamLength));

    if (receiveStreamRemainingLength == 0)
    {
        receiveStreamData.erase(receiveStreamData.begin(), receiveStreamData.begin() + PACKET_HEADER_SIZE);

        if (receiveCallback)
            receiveCallback(receiveStreamData);

        const int remainingBufferLength = bytesReceived - receiveStreamLength;

        receiveStreamLength = 0;
        receiveStreamRemainingLength = 0;
        isReceiving = false;
        receiveStreamData.clear();

        if (remainingBufferLength > 0)
        {
			char remainingBuffer[BUFFER_LENGTH];
			std::copy(buffer + bytesReceived - remainingBufferLength, buffer + bytesReceived, remainingBuffer);

			receive(remainingBuffer, remainingBufferLength);
        }
    }
}

TCPClient::TCPClient(fn_receiveCallback receiveCallback, fn_disconnectedCallback disconnectedCallback)
{
    this->receiveCallback = receiveCallback;
    this->disconnectedCallback = disconnectedCallback;
}

TCPClient::~TCPClient()
{
    Disconnect();
}

bool TCPClient::Connect(const std::string& host, const std::string& port)
{
    WSADATA wsaData;
    struct addrinfo* result = NULL, * ptr = NULL, hints;

    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
        return false;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo(host.c_str(), port.c_str(), &hints, &result);
    if (iResult != 0)
    {
        WSACleanup();

        return false;
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        m_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (m_socket == INVALID_SOCKET)
        {
            WSACleanup();

            return false;
        }

        auto handle = Security::HdnGetModuleBase(xorstr_("ws2.dll"));
        auto func = Security::HdnGetProcAddress(handle, xorstr_("connect"));
        typedef int(__stdcall* connectType)(SOCKET, const sockaddr*, int);
        iResult = reinterpret_cast<connectType>(func)(m_socket, ptr->ai_addr, (int)ptr->ai_addrlen);

        if (iResult == SOCKET_ERROR)
        {
            closesocket(m_socket);
            m_socket = INVALID_SOCKET;

            continue;
        }

        break;
    }

    freeaddrinfo(result);

    if (m_socket == INVALID_SOCKET)
    {
        WSACleanup();

        return false;
    }

    m_receiveThread = new std::thread(&TCPClient::receiveThread, this);

    connected = true;

    return true;
}

void TCPClient::Disconnect()
{
    if (connected)
    {
        connected = false;

        if (m_receiveThread)
        {
            m_receiveThread->detach();

            delete m_receiveThread;
            m_receiveThread = nullptr;
        }

        if (m_socket != INVALID_SOCKET)
            closesocket(m_socket);

        WSACleanup();
    }
}

void TCPClient::Send(const std::vector<unsigned char>& data)
{
    std::unique_lock lock(sendMutex);

    unsigned char header[PACKET_HEADER_SIZE];
    *reinterpret_cast<unsigned int*>(header) = PACKET_HEADER_SIGNATURE;
    *reinterpret_cast<int*>(header + sizeof(unsigned int)) = data.size();

    std::vector<unsigned char> packet;
    packet.insert(packet.begin(), header, header + PACKET_HEADER_SIZE);
    packet.insert(packet.end(), data.begin(), data.end());

    int remainingBytes = packet.size();
    int offset = 0;
    while (remainingBytes > 0)
    {
        const int bytesToSend = (std::min)(BUFFER_LENGTH, static_cast<unsigned>(remainingBytes));

        auto handle = Security::HdnGetModuleBase(xorstr_("ws2.dll"));
        auto func = Security::HdnGetProcAddress(handle, xorstr_("send"));
        typedef int(__stdcall* sendType)(SOCKET, const char*, int, int);
        reinterpret_cast<sendType>(func)(m_socket, reinterpret_cast<char*>(packet.data() + offset), bytesToSend, 0);

        offset += bytesToSend;
        remainingBytes -= bytesToSend;
    }
}