#include "Communication.h"

#include <random>

#include "ThemidaSDK.h"

#include "../Utilities/Security/Security.h"
#include "../Logging/Logger.h"
#include "../Utilities/Security/xorstr.hpp"
#include "Crypto/CryptoProvider.h"
#include "Packets/PacketType.h"
#include "Packets/Requests/HandshakeRequest.h"
#include "Packets/Requests/HeartbeatRequest.h"
#include "Packets/Responses/HandshakeResponse.h"
#include "Packets/Responses/HeartbeatResponse.h"

#include "../Dependencies/Milk/MilkThread.h"

#pragma optimize("", off)
static inline MilkThread* pingMilkThread;
static inline MilkThread* heartbeatMilkThread;
static inline MilkThread* checkerMilkThread;

void Communication::pingThread()
{
	pingMilkThread->CleanCodeCave();
	delete pingMilkThread;

	while (true)
	{
		VM_SHARK_BLACK_START

		if (!connected || !handshakeSucceeded || !heartbeatThreadLaunched || !Security::CheckIfThreadIsAlive(ThreadCheckerHandle))
		{
			IntegritySignature1 -= 0x1;
			IntegritySignature2 -= 0x1;
			IntegritySignature3 -= 0x1;
		}

		tcpClient.Send({ static_cast<unsigned char>(PacketType::Ping) });

		Sleep(45000); //45 seconds
		
		VM_SHARK_BLACK_END
	}
}

void Communication::checkerThread()
{
	checkerMilkThread->CleanCodeCave();
	delete checkerMilkThread;

	while (true)
	{
		VM_SHARK_BLACK_START

		if (heartbeatThreadLaunched)
		{
			if (!Security::CheckIfThreadIsAlive(heartbeatThreadHandle) || !Security::CheckIfThreadIsAlive(pingThreadHandle))
			{
				IntegritySignature1 -= 0x1;
				IntegritySignature2 -= 0x1;
				IntegritySignature3 -= 0x1;
			}
		}

		Sleep(15000); //15 seconds

		VM_SHARK_BLACK_END
	}
}

void Communication::heartbeatThread()
{
	heartbeatMilkThread->CleanCodeCave();
	delete heartbeatMilkThread;

	while (true)
	{
		VM_SHARK_BLACK_START
		STR_ENCRYPT_START

		int codeIntegrityVar = 0x671863E2;
		CHECK_CODE_INTEGRITY(codeIntegrityVar, 0x40CD69D0)
		if (codeIntegrityVar != 0x40CD69D0)
		{
			IntegritySignature1 -= 0x1;
			IntegritySignature2 -= 0x1;
			IntegritySignature3 -= 0x1;
		}

		int debuggerVar = 0xD0A7E6;
		CHECK_DEBUGGER(debuggerVar, 0x3E839EE3)
		if (debuggerVar != 0x3E839EE3)
		{
			IntegritySignature1 -= 0x1;
			IntegritySignature2 -= 0x1;
			IntegritySignature3 -= 0x1;
		}

		if (IntegritySignature1 != 0xdeadbeef || IntegritySignature2 != 0xefbeadde || IntegritySignature3 != 0xbeefdead)
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> crashRNG(1, 2);

			// 50% chance to crash right here
			if (crashRNG(gen) == 1)
			{
				int randomAddress = rand() % (UINT_MAX - 1048576 + 1) + 1048576;
				int errorCode = 4099;
				Logger::Log(LogSeverity::Error, xorstr_("Unhandled exception at 0x%X (0x%X). Please report this."), randomAddress, errorCode);

				Security::CorruptMemory();
			}
		}

		HeartbeatRequest heartbeatRequest = HeartbeatRequest(user->GetSessionToken());
		tcpClient.Send(heartbeatRequest.Serialize());

		Sleep(600000); // 10 minutes

		STR_ENCRYPT_END
		VM_SHARK_BLACK_END
	}
}

void Communication::onReceive(const std::vector<unsigned char>& data)
{
	VM_SHARK_BLACK_START
	STR_ENCRYPT_START

	int codeIntegrityVar = 0x671863E2;
	CHECK_CODE_INTEGRITY(codeIntegrityVar, 0x40CD69D0)
	if (codeIntegrityVar != 0x40CD69D0)
	{
		IntegritySignature1 -= 0x1;
		IntegritySignature2 -= 0x1;
		IntegritySignature3 -= 0x1;
	}

	int debuggerVar = 0xD0A7E6;
	CHECK_DEBUGGER(debuggerVar, 0x3E839EE3)
	if (debuggerVar != 0x3E839EE3)
	{
		IntegritySignature1 -= 0x1;
		IntegritySignature2 -= 0x1;
		IntegritySignature3 -= 0x1;
	}

	if (IntegritySignature1 != 0xdeadbeef || IntegritySignature2 != 0xefbeadde || IntegritySignature3 != 0xbeefdead)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> crashRNG(1, 10);

		// 10% chance to crash right here
		if (crashRNG(gen) == 3)
		{
			int randomAddress = rand() % (UINT_MAX - 1048576 + 1) + 1048576;
			int errorCode = 4097;
			Logger::Log(LogSeverity::Error, xorstr_("Unhandled exception at 0x%X (0x%X). Please report this."), randomAddress, errorCode);

			Security::CorruptMemory();
		}
	}

	const auto type = static_cast<PacketType>(data[0]);
	const std::vector payload(data.begin() + 1, data.end());

	STR_ENCRYPT_END
	VM_SHARK_BLACK_END

	switch (type)
	{
		case PacketType::Handshake:
		{
			VM_SHARK_BLACK_START
			STR_ENCRYPT_START

			HandshakeResponse handshakeResponse = HandshakeResponse::Deserialize(payload);

			CryptoProvider::GetInstance()->InitializeAES(handshakeResponse.GetKey(), handshakeResponse.GetIV());

			handshakeSucceeded = true;
			
			heartbeatMilkThread = new MilkThread(reinterpret_cast<uintptr_t>(heartbeatThread), true);
			heartbeatThreadHandle = heartbeatMilkThread->Start();

			heartbeatThreadLaunched = true;

			pingMilkThread = new MilkThread(reinterpret_cast<uintptr_t>(pingThread), true);
			pingThreadHandle = pingMilkThread->Start();

			STR_ENCRYPT_END
			VM_SHARK_BLACK_END
		}
			break;
		case PacketType::Heartbeat:
		{
			VM_SHARK_BLACK_START
			STR_ENCRYPT_START

			HeartbeatResponse heartbeatResponse = HeartbeatResponse::Deserialize(payload);

			if (heartbeatResponse.GetResult() != HeartbeatResult::Success)
			{
				int randomAddress = rand() % (UINT_MAX - 1048576 + 1) + 1048576;
				int errorCode = 4098;
				Logger::Log(LogSeverity::Error, xorstr_("Unhandled exception at 0x%X (0x%X). Please report this."), randomAddress, errorCode);

				Security::CorruptMemory();
			}

			STR_ENCRYPT_END
			VM_SHARK_BLACK_END
		}
			break;
		case PacketType::Ping:
			break;
		default:
		{
			VM_SHARK_BLACK_START

			IntegritySignature1 -= 0x1;
			IntegritySignature2 -= 0x1;
			IntegritySignature3 -= 0x1;

			VM_SHARK_BLACK_END
		}
	}
}

void Communication::onDisconnect()
{
	VM_SHARK_BLACK_START
	STR_ENCRYPT_START

	IntegritySignature1 -= 0x1;
	IntegritySignature2 -= 0x1;
	IntegritySignature3 -= 0x1;

	int randomAddress = rand() % (UINT_MAX - 1048576 + 1) + 1048576;
	int errorCode = 4096;
	Logger::Log(LogSeverity::Error, xorstr_("Unhandled exception at 0x%X (0x%X). Please report this."), randomAddress, errorCode);

	Security::CorruptMemory();

	STR_ENCRYPT_END
	VM_SHARK_BLACK_END
}

bool Communication::Connect()
{
	VM_FISH_RED_START
	STR_ENCRYPT_START

	if (connected)
	{
		IntegritySignature1 -= 0x1;
		IntegritySignature2 -= 0x1;
		IntegritySignature3 -= 0x1;

		return true;
	}

	tcpClient = TCPClient(&onReceive, &onDisconnect);
	if (!tcpClient.Connect(xorstr_("127.0.0.1"), xorstr_("9999")))
		return false;

	connected = true;

	checkerMilkThread = new MilkThread(reinterpret_cast<uintptr_t>(checkerThread), true);
	ThreadCheckerHandle = checkerMilkThread->Start();

	HandshakeRequest handshakeRequest = HandshakeRequest();
	tcpClient.Send(handshakeRequest.Serialize());

	STR_ENCRYPT_END
	VM_FISH_RED_END

	return true;
}

void Communication::Disconnect()
{
	VM_SHARK_BLACK_START

	connected = false;
	
	tcpClient.Disconnect();

	VM_SHARK_BLACK_END
}

bool Communication::GetIsConnected()
{
	VM_SHARK_BLACK_START

	return connected;

	VM_SHARK_BLACK_END
}

bool Communication::GetIsHandshakeSucceeded()
{
	VM_SHARK_BLACK_START

	return handshakeSucceeded;

	VM_SHARK_BLACK_END
}

bool Communication::GetIsHeartbeatThreadLaunched()
{
	VM_SHARK_BLACK_START

	return heartbeatThreadLaunched;

	VM_SHARK_BLACK_END
}

User* Communication::GetUser()
{
	VM_SHARK_BLACK_START

	return user;

	VM_SHARK_BLACK_END
}

void Communication::SetUser(User* user)
{
	VM_SHARK_BLACK_START
	
	delete Communication::user;
	Communication::user = user;

	VM_SHARK_BLACK_END
}
#pragma optimize("", on)
