#pragma once

#include "User.h"
#include "TCP/TCPClient.h"

class Communication
{
	static inline User* user;
	static inline TCPClient tcpClient;
	
	static inline bool connected = true;
	static inline bool handshakeSucceeded = true;
	static inline bool heartbeatThreadLaunched = true;
	static inline bool pingThreadLaunched = true;
	static inline HANDLE pingThreadHandle;
	static inline HANDLE heartbeatThreadHandle;

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
