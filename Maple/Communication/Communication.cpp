#include "Communication.h"

#include <random>

#include "VirtualizerSDK.h"
#include "xorstr.hpp"

#include "../Utilities/Security/Security.h"
#include "../Logging/Logger.h"
#include "Crypto/CryptoProvider.h"
#include "Packets/PacketType.h"
#include "Packets/Requests/HandshakeRequest.h"
#include "Packets/Requests/HeartbeatRequest.h"
#include "Packets/Responses/HandshakeResponse.h"
#include "Packets/Responses/HeartbeatResponse.h"

#include "../Dependencies/Milk/MilkThread.h"
#include "Packets/Requests/AuthStreamStageOneRequest.h"
#include "../Utilities/Anticheat/AnticheatUtilities.h"
#include "Packets/Requests/AuthStreamStageTwoRequest.h"
#include "Packets/Responses/AuthStreamStageOneResponse.h"

static inline MilkThread* pingMilkThread;
static inline MilkThread* heartbeatMilkThread;
static inline MilkThread* checkerMilkThread;

[[clang::optnone]] void Communication::pingThread()
{
	//pingMilkThread->CleanCodeCave();
	//delete pingMilkThread;

	while (true)
	{
		while (!heartbeatThreadLaunched)
			Sleep(100);

		VIRTUALIZER_FISH_EXTREME_START
		if (!pingThreadLaunched)
			pingThreadLaunched = true;

		if (!connected || !handshakeSucceeded || !heartbeatThreadLaunched || !Security::CheckIfThreadIsAlive(ThreadCheckerHandle))
		{
			IntegritySignature1 -= 0x1;
			IntegritySignature2 -= 0x1;
			IntegritySignature3 -= 0x1;
		}

		tcpClient.Send({ static_cast<unsigned char>(PacketType::Ping) });

		Sleep(45000); //45 seconds
		
		VIRTUALIZER_FISH_EXTREME_END
	}
}

[[clang::optnone]] void Communication::checkerThread()
{
	//checkerMilkThread->CleanCodeCave();
	//delete checkerMilkThread;

	while (true)
	{
		VIRTUALIZER_FISH_RED_START

		if (heartbeatThreadLaunched && pingThreadLaunched)
		{
			if (!Security::CheckIfThreadIsAlive(heartbeatThreadHandle) || !Security::CheckIfThreadIsAlive(pingThreadHandle))
			{
				IntegritySignature1 -= 0x1;
				IntegritySignature2 -= 0x1;
				IntegritySignature3 -= 0x1;
			}
		}

		Sleep(15000); //15 seconds

		VIRTUALIZER_FISH_RED_END
	}
}

void Communication::heartbeatThread()
{
	//heartbeatMilkThread->CleanCodeCave();
	//delete heartbeatMilkThread;

	while (true)
	{
		if (!heartbeatThreadLaunched)
			heartbeatThreadLaunched = true;

		HeartbeatRequest heartbeatRequest = HeartbeatRequest(user->GetSessionToken());
		tcpClient.Send(heartbeatRequest.Serialize());

		Sleep(600000); // 10 minutes
	}
}

void Communication::sendAuthStreamStageTwo()
{
	VIRTUALIZER_SHARK_BLACK_START

	AuthStreamStageTwoRequest authStreamStageTwoRequest = AuthStreamStageTwoRequest(user->GetUsername(), AnticheatUtilities::GetAnticheatChecksum(), AnticheatUtilities::GetAnticheatBytes(), AnticheatUtilities::GetGameBytes());
	tcpClient.Send(authStreamStageTwoRequest.Serialize());

	VIRTUALIZER_SHARK_BLACK_END
}

void Communication::onReceive(const std::vector<unsigned char>& data)
{
	const auto type = static_cast<PacketType>(data[0]);
	const std::vector payload(data.begin() + 1, data.end());

	switch (type)
	{
		case PacketType::Handshake:
		{
			VIRTUALIZER_SHARK_BLACK_START
			
			HandshakeResponse handshakeResponse = HandshakeResponse::Deserialize(payload);

			CryptoProvider::Get().InitializeAES(handshakeResponse.GetKey(), handshakeResponse.GetIV());

			handshakeSucceeded = true;
			
			heartbeatMilkThread = new MilkThread(reinterpret_cast<uintptr_t>(heartbeatThread), true);
			heartbeatThreadHandle = heartbeatMilkThread->Start();

			pingMilkThread = new MilkThread(reinterpret_cast<uintptr_t>(pingThread), true);
			pingThreadHandle = pingMilkThread->Start();

						VIRTUALIZER_SHARK_BLACK_END
		}
			break;
		case PacketType::Heartbeat:
		{
			VIRTUALIZER_SHARK_BLACK_START
			
			HeartbeatResponse heartbeatResponse = HeartbeatResponse::Deserialize(payload);

			if (heartbeatResponse.GetResult() != HeartbeatResult::Success)
			{
				int randomAddress = rand() % (UINT_MAX - 1048576 + 1) + 1048576;
				int errorCode = 4098;
				Logger::Log(LogSeverity::Error, xorstr_("Unhandled exception at 0x%X (0x%X). Please report this."), randomAddress, errorCode);

				Security::CorruptMemory();
			}

						VIRTUALIZER_SHARK_BLACK_END
		}
			break;
		case PacketType::Ping:
			break;
		case PacketType::AuthStreamStageOne:
		{
			VIRTUALIZER_SHARK_BLACK_START

			AuthStreamStageOneResponse authStreamStageOneResponse = AuthStreamStageOneResponse::Deserialize(payload);

			if (authStreamStageOneResponse.GetShouldSend())
				auto authStreamStageTwoThread = MilkThread(reinterpret_cast<uintptr_t>(sendAuthStreamStageTwo));

			VIRTUALIZER_SHARK_BLACK_END
		}
			break;
		default:
		{
			VIRTUALIZER_SHARK_BLACK_START
				
			IntegritySignature1 -= 0x1;
			IntegritySignature2 -= 0x1;
			IntegritySignature3 -= 0x1;

			VIRTUALIZER_SHARK_BLACK_END
		}
	}
}

void Communication::onDisconnect()
{
	VIRTUALIZER_SHARK_BLACK_START
		
	IntegritySignature1 -= 0x1;
	IntegritySignature2 -= 0x1;
	IntegritySignature3 -= 0x1;

	int randomAddress = rand() % (UINT_MAX - 1048576 + 1) + 1048576;
	int errorCode = 4096;
	Logger::Log(LogSeverity::Error, xorstr_("Unhandled exception at 0x%X (0x%X). Please report this."), randomAddress, errorCode);

	Security::CorruptMemory();

		VIRTUALIZER_SHARK_BLACK_END
}

bool Communication::Connect()
{
	VIRTUALIZER_TIGER_WHITE_START
	
	if (connected)
	{
		IntegritySignature1 -= 0x1;
		IntegritySignature2 -= 0x1;
		IntegritySignature3 -= 0x1;

		return true;
	}

	tcpClient = TCPClient(&onReceive, &onDisconnect);
	if (!tcpClient.Connect(xorstr_("198.251.89.179"), xorstr_("9999")))
		return false;

	connected = true;

	checkerMilkThread = new MilkThread(reinterpret_cast<uintptr_t>(checkerThread), true);
	ThreadCheckerHandle = checkerMilkThread->Start();

	HandshakeRequest handshakeRequest = HandshakeRequest();
	tcpClient.Send(handshakeRequest.Serialize());

	VIRTUALIZER_TIGER_WHITE_END

	return true;
}

void Communication::Disconnect()
{
	VIRTUALIZER_SHARK_BLACK_START

	connected = false;
	
	tcpClient.Disconnect();

	VIRTUALIZER_SHARK_BLACK_END
}

[[clang::optnone]] void Communication::SendAnticheat()
{
	VIRTUALIZER_TIGER_LITE_START

	AuthStreamStageOneRequest authStreamStageOneRequest = AuthStreamStageOneRequest(AnticheatUtilities::GetAnticheatChecksum());
	tcpClient.Send(authStreamStageOneRequest.Serialize());

	VIRTUALIZER_TIGER_LITE_END
}

bool Communication::GetIsConnected()
{
	bool ret = connected;

	return ret;
}

bool Communication::GetIsHandshakeSucceeded()
{
	bool ret = handshakeSucceeded;

	return ret;
}

bool Communication::GetIsHeartbeatThreadLaunched()
{
	bool ret = heartbeatThreadLaunched;

	return ret;
}

User* Communication::GetUser()
{
	VIRTUALIZER_SHARK_BLACK_START

	User* ret = user;

	VIRTUALIZER_SHARK_BLACK_END

	return ret;
}

[[clang::optnone]] void Communication::SetUser(User* user)
{
	VIRTUALIZER_FISH_WHITE_START
	
	delete Communication::user;
	Communication::user = user;

	VIRTUALIZER_FISH_WHITE_END
}
