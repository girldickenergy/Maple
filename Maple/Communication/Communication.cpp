#include "Communication.h"

#include <random>

#include "VirtualizerSDK.h"
#include "xorstr.hpp"

#include "../Utilities/Security/Security.h"
#include "../Logging/Logger.h"
#include "Crypto/CryptoProvider.h"


#include "Packets/Requests/RequestPackets.h"
#include "Packets/Responses/ResponsePackets.h"


#include "../Dependencies/Milk/MilkThread.h"
#include "../Utilities/Anticheat/AnticheatUtilities.h"
#include "Packets/Fnv1a.h"

static inline MilkThread* pingMilkThread;
static inline MilkThread* heartbeatMilkThread;
static inline MilkThread* checkerMilkThread;

#define SEND(x) \
	if (auto serialized = m_Serializer.Serialize(x); serialized.has_value()) \
		m_TcpClient.Send(*serialized); \
	else \
		Security::CorruptMemory(); \

void Communication::sendHandshake()
{
	VIRTUALIZER_SHARK_BLACK_START

	std::mt19937 random(std::random_device{}());
	std::uniform_int_distribution<> byteDistribution(0x03, 0xFF);
	std::uniform_int_distribution<> intDistribution(0x00, 0xFFFFFFFF);

	// Random amount of bytes, but at least 0x03
	auto length = byteDistribution(random);
	auto data = std::vector<uint32_t>(length);

	for (size_t i = 0; i < length; i++)
	{
		data[i] = intDistribution(random);
	}

	data[1] = data[0] ^ data[2] ^ 0xDEADBEEF;

	HandshakeRequest handshakeRequest = HandshakeRequest(data);
	SEND(handshakeRequest);

	VIRTUALIZER_SHARK_BLACK_END
}

[[clang::optnone]] void Communication::PingThread()
{
	//pingMilkThread->CleanCodeCave();
	//delete pingMilkThread;

	while (true)
	{
		while (!m_HeartbeatThreadLaunched)
			Sleep(100);

		VIRTUALIZER_LION_WHITE_START
		if (!m_PingThreadLaunched)
			m_PingThreadLaunched = true;

		if (!m_Connected || !m_HandshakeSucceeded || !m_HeartbeatThreadLaunched || !Security::CheckIfThreadIsAlive(ThreadCheckerHandle))
		{
			IntegritySignature1 -= 0x1;
			IntegritySignature2 -= 0x1;
			IntegritySignature3 -= 0x1;
		}

		PingRequest pingRequest = PingRequest();
		SEND(pingRequest);

		Sleep(45000); //45 seconds
		
		VIRTUALIZER_LION_WHITE_END
	}
}

[[clang::optnone]] void Communication::CheckerThread()
{
	//checkerMilkThread->CleanCodeCave();
	//delete checkerMilkThread;

	while (true)
	{
		VIRTUALIZER_FISH_RED_START

		if (m_HeartbeatThreadLaunched && m_PingThreadLaunched)
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

void Communication::HeartbeatThread()
{
	//heartbeatMilkThread->CleanCodeCave();
	//delete heartbeatMilkThread;

	while (true)
	{
		if (!m_HeartbeatThreadLaunched)
			m_HeartbeatThreadLaunched = true;

		HeartbeatRequest heartbeatRequest = HeartbeatRequest(m_User->GetSessionToken());
		SEND(heartbeatRequest);

		Sleep(600000); // 10 minutes
	}
}

void Communication::OnReceive(const std::vector<unsigned char>& data)
{
	VIRTUALIZER_SHARK_BLACK_START

	auto deserialized = m_Serializer.Deserialize(data);
	if (!deserialized.has_value())
		Security::CorruptMemory();

	if (!m_PacketHandlers.contains(deserialized->second))
	{
		IntegritySignature1 -= 0x1;
		IntegritySignature2 -= 0x1;
		IntegritySignature3 -= 0x1;
	}

	auto& packetHandler = m_PacketHandlers[deserialized->second];
	packetHandler(deserialized->first);

	VIRTUALIZER_SHARK_BLACK_END
}

void Communication::OnDisconnect()
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

Communication::Communication(singletonLock) : m_Serializer(PacketSerializer::Get())
{
	m_Mha256 = Mha256();

	m_PacketHandlers =
	{
		{
			Hash32Fnv1aConst("HandshakeResponse"), [this](entt::meta_any packet)
			{
				VIRTUALIZER_SHARK_BLACK_START

				auto handshakeResponse = packet.cast<HandshakeResponse>();

				auto decryptionArray = m_Mha256.ComputeHash(handshakeResponse.GetKey());
				auto decryptionKey = std::vector<uint8_t>(decryptionArray.begin(), decryptionArray.end());

				auto decryptedKey = CryptoProvider::Get().ApplyRollingXor(handshakeResponse.GetEncryptedKey(), decryptionKey);
				auto decryptedIv = CryptoProvider::Get().ApplyRollingXor(handshakeResponse.GetEncryptedIv(), decryptionKey);

				CryptoProvider::Get().InitializeAES(decryptedKey, decryptedIv);
				
				m_HandshakeSucceeded = true;

				auto* heartbeatLambda = static_cast<void(*)()>([]() { Communication::Get().HeartbeatThread(); });
				heartbeatMilkThread = new MilkThread(reinterpret_cast<uintptr_t>(heartbeatLambda), true);
				heartbeatThreadHandle = heartbeatMilkThread->Start();

				auto* pingLambda = static_cast<void(*)()>([]() { Communication::Get().PingThread(); });
				pingMilkThread = new MilkThread(reinterpret_cast<uintptr_t>(pingLambda), true);
				pingThreadHandle = pingMilkThread->Start();

				VIRTUALIZER_SHARK_BLACK_END
			}
		},
		{
			Hash32Fnv1aConst("HeartbeatResponse"), [](entt::meta_any packet)
			{
				VIRTUALIZER_SHARK_BLACK_START

				auto heartbeatResponse = packet.cast<HeartbeatResponse>();

				if (heartbeatResponse.GetResult() != HeartbeatResult::Success)
				{
					int randomAddress = rand() % (UINT_MAX - 1048576 + 1) + 1048576;
					int errorCode = 4098;
					Logger::Log(LogSeverity::Error, xorstr_("Unhandled exception at 0x%X (0x%X). Please report this."), randomAddress, errorCode);

					Security::CorruptMemory();
				}

				VIRTUALIZER_SHARK_BLACK_END
			}
		},
		{ Hash32Fnv1aConst("PingResponse"), [](entt::meta_any packet) { } }
	};

	m_Connected = false;
	m_HandshakeSucceeded = false;
	m_HeartbeatThreadLaunched = false;
	m_PingThreadLaunched = false;

	//m_Serializer = PacketSerializer::Get();
}

bool Communication::Connect()
{
	VIRTUALIZER_TIGER_WHITE_START

	if (m_Connected)
	{
		IntegritySignature1 -= 0x1;
		IntegritySignature2 -= 0x1;
		IntegritySignature3 -= 0x1;

		return true;
	}

	auto receiveBinding = [this]<typename T>(T && data) { OnReceive(std::forward<T>(data)); };
	auto disconnectBinding = [this] { OnDisconnect(); };

	m_TcpClient = TCPClient(receiveBinding, disconnectBinding);
	if (!m_TcpClient.Connect(xorstr_("198.251.89.179"), xorstr_("9999")))
		return false;

	m_Connected = true;

	auto* checkerLambda = static_cast<void(*)()>([]() { Communication::Get().CheckerThread(); });
	checkerMilkThread = new MilkThread(reinterpret_cast<uintptr_t>(checkerLambda), true);
	ThreadCheckerHandle = checkerMilkThread->Start();

	sendHandshake();

	VIRTUALIZER_TIGER_WHITE_END

	return true;
}

void Communication::Disconnect()
{
	VIRTUALIZER_SHARK_BLACK_START

	m_Connected = false;
	m_TcpClient.Disconnect();

	VIRTUALIZER_SHARK_BLACK_END
}

bool Communication::GetIsConnected()
{
	return m_Connected;
}

bool Communication::GetIsHandshakeSucceeded()
{
	return m_HandshakeSucceeded;
}

bool Communication::GetIsHeartbeatThreadLaunched()
{
	return m_HeartbeatThreadLaunched;
}

User* Communication::GetUser()
{
	VIRTUALIZER_SHARK_BLACK_START

	User* ret = m_User;

	VIRTUALIZER_SHARK_BLACK_END

	return ret;
}

[[clang::optnone]] void Communication::SetUser(User* user)
{
	VIRTUALIZER_FISH_WHITE_START
	
	delete Communication::m_User;
	Communication::m_User = user;

	VIRTUALIZER_FISH_WHITE_END
}
