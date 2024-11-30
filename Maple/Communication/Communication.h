#pragma once

#include "User.h"
#include "TCP/TCPClient.h"
#include "Packets/PacketSerializer.h"

class Communication
{
	static inline PacketSerializer& serializer = PacketSerializer::Get();

	static inline User* user;
	static inline TCPClient tcpClient;
	
	static inline bool connected = false;
	static inline bool handshakeSucceeded = false;
	static inline bool heartbeatThreadLaunched = false;
	static inline bool pingThreadLaunched = false;
	static inline HANDLE pingThreadHandle;
	static inline HANDLE heartbeatThreadHandle;

#define SEND(x) \
	if (auto serialized = serializer.Serialize(x); serialized.has_value()) \
		tcpClient.Send(*serialized); \
	else \
		Security::CorruptMemory(); \

	static void pingThread();
	static void heartbeatThread();
	static void checkerThread();
	static void sendAuthStreamStageTwo();

	static void onReceive(const std::vector<unsigned char>& data);
	static void onDisconnect();
public:
	static inline HANDLE ThreadCheckerHandle;
	static inline unsigned int IntegritySignature1 = 0xdeadbeef;
	static inline unsigned int IntegritySignature2 = 0xefbeadde;
	static inline unsigned int IntegritySignature3 = 0xbeefdead;

	static bool Connect();
	static void Disconnect();
	static void SendAnticheat();

	static bool GetIsConnected();
	static bool GetIsHandshakeSucceeded();
	static bool GetIsHeartbeatThreadLaunched();
	static User* GetUser();
	static void SetUser(User* user);
};
