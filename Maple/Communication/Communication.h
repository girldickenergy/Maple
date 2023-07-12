#pragma once

#include "TCP/TCPClient.h"
#include "User.h"

class Communication
{
    static inline User* user;
    static inline TCPClient tcpClient;

#ifdef NO_BYPASS
    static inline bool connected = true;
    static inline bool handshakeSucceeded = true;
    static inline bool heartbeatThreadLaunched = true;
    static inline bool pingThreadLaunched = true;
#else
    static inline bool connected = false;
    static inline bool handshakeSucceeded = false;
    static inline bool heartbeatThreadLaunched = false;
    static inline bool pingThreadLaunched = false;
#endif

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
