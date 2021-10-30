#pragma once

#include "MatchedClient.h"
#include "User.h"
#include "TCP/TCP_Client.h"
#include "Wrapper/RSADecrypt.h"

class Communication
{
	static inline HANDLE pingThreadHandle;
	static inline HANDLE heartbeatThreadHandle;

	static void pingThread();
	static void heartbeatThread();
	static void checkerThread();
	
	static void onIncomingMessage(const char* msg, size_t size);
	static void onDisconnection(const pipe_ret_t& ret);
public:
	static inline RSADecrypt RSA = RSADecrypt();
	static inline TcpClient TCPClient = TcpClient();
	static inline MatchedClient* MatchedClient = nullptr;

	static inline User* CurrentUser = new User("", "", "", "");
	static inline bool EstablishedConnection = false;

	static inline bool HandshakeSucceeded = false;
	static inline bool HeartbeatThreadLaunched = false;

	static inline HANDLE ThreadCheckerHandle;
	
	static void ConnectToServer();
};
