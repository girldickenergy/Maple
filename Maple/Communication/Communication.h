#pragma once

#include "User.h"
#include "TCP/TCPClient.h"
#include "Packets/PacketSerializer.h"
#include "Singleton.h"
#include "Crypto/Mha256.h"

#include "entt.hpp"

class Communication : public Singleton<Communication>
{
	std::unordered_map<uint32_t, std::function<void(entt::meta_any)>> m_PacketHandlers;

	PacketSerializer& m_Serializer;
	Mha256 m_Mha256;

	User* m_User;
	TCPClient m_TcpClient;
	
	bool m_Connected;
	bool m_HandshakeSucceeded;
	bool m_HeartbeatThreadLaunched;
	bool m_PingThreadLaunched;
	HANDLE pingThreadHandle;
	HANDLE heartbeatThreadHandle;

	void sendHandshake();

	void PingThread();
	void HeartbeatThread();
	void CheckerThread();

	void OnReceive(const std::vector<unsigned char>& data);
	void OnDisconnect();
public:
	Communication(singletonLock);

	HANDLE ThreadCheckerHandle;
	unsigned int IntegritySignature1 = 0xdeadbeef;
	unsigned int IntegritySignature2 = 0xefbeadde;
	unsigned int IntegritySignature3 = 0xbeefdead;

	bool Connect();
	void Disconnect();

	bool GetIsConnected();
	bool GetIsHandshakeSucceeded();
	bool GetIsHeartbeatThreadLaunched();
	User* GetUser();
	void SetUser(User* user);
};
