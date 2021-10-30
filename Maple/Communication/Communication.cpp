#include "Communication.h"

#include <ThemidaSDK.h>

#include "../Utilities/Security/Security.h"

#include "Packets/Requests/HandshakeRequest.h"
#include "Packets/Requests/HeartbeatRequest.h"

#include "Packets/Responses/HandshakeResponse.h"
#include "Packets/Responses/HeartbeatResponse.h"
#include "Packets/Responses/Response.h"
#include "Packets/Responses/ResponseType.h"

#include "../Utilities/Security/xorstr.hpp"

void Communication::pingThread()
{
	while (true)
	{
		VM_SHARK_BLACK_START
		Request pingPacket = Request(RequestType::Ping);
		pipe_ret_t sendRet = TCPClient.sendBytes(pingPacket.Data);
		if (!sendRet.success) {
			Security::CorruptMemory();
			break;
		}

		Sleep(45000); //45 seconds
		VM_SHARK_BLACK_END
	}
}

void Communication::heartbeatThread()
{
	while (true)
	{
		VM_SHARK_BLACK_START
		if (!HeartbeatThreadLaunched)
			HeartbeatThreadLaunched = true;
		HeartbeatRequest heartbeatPacket = HeartbeatRequest(CurrentUser->SessionID, MatchedClient);
		pipe_ret_t sendRet = TCPClient.sendBytes(heartbeatPacket.Data);
		if (!sendRet.success) {
			Security::CorruptMemory();
			break;
		}

		DWORD check = 0x7B4527CA;
		CHECK_PROTECTION(check, 0x6DDE48A3);
		if(check == 0x7B4527CA) {
			Security::CorruptMemory();
			break;
		}

		check = 0xF30BB4D0;
		CHECK_CODE_INTEGRITY(check, 0xD4D4DCCF);
		if (check == 0xF30BB4D0) {
			Security::CorruptMemory();
			break;
		}

		check = 0xFE248DCC;
		CHECK_DEBUGGER(check, 0x5EE32188);
		if (check == 0xFE248DCC) {
			Security::CorruptMemory();
			break;
		}

		Sleep(600000); //10 minutes
		VM_SHARK_BLACK_END
	}
}

void Communication::checkerThread()
{
	while (true)
	{
		VM_SHARK_BLACK_START
		if (HeartbeatThreadLaunched)
		{ 
			Security::CheckIfThreadIsAlive(heartbeatThreadHandle, true); // Megumi Team cracked Maple by Terminating this Thread with ExitCode -> 1337
			Security::CheckIfThreadIsAlive(pingThreadHandle, true);
		}

		Sleep(15000); //15 seconds
		VM_SHARK_BLACK_END
	}
}

void Communication::onIncomingMessage(const char* msg, size_t size)
{
	auto* const response = static_cast<Response*>(Response::ConstructResponse(msg, size, MatchedClient));
	switch (response->Type)
	{
		case ResponseType::FatalError:
		{
			VM_SHARK_BLACK_START
			Security::CorruptMemory();
			VM_SHARK_BLACK_END
			break;
		}
		case ResponseType::Handshake:
		{
			auto* const handshakeResponse = static_cast<HandshakeResponse*>(response);
			switch (handshakeResponse->Result)
			{
				case HandshakeResult::Success:
				{
					VM_SHARK_BLACK_START
					MatchedClient = new ::MatchedClient(TCPClient);
					MatchedClient->aes->SetIV(handshakeResponse->IV);
					MatchedClient->aes->SetKey(handshakeResponse->Key);

					EstablishedConnection = true;
					HandshakeSucceeded = true;
					heartbeatThreadHandle = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(heartbeatThread), nullptr, 0, nullptr);
					pingThreadHandle = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(pingThread), nullptr, 0, nullptr);
					VM_SHARK_BLACK_END
					break;
				}
				case HandshakeResult::EpochTimedOut:
				case HandshakeResult::InternalError:
				{
					VM_SHARK_BLACK_START
					// Have both in a switch case, let's not tell anybody trying to crack that the epoch is wrong.
					Security::CorruptMemory();
					VM_SHARK_BLACK_END
					break;
				}
			}

			break;
		}
		case ResponseType::Heartbeat:
		{
			VM_SHARK_BLACK_START

			auto* const heartbeatResponse = static_cast<HeartbeatResponse*>(response);
			if (heartbeatResponse->Result != HeartbeatResult::Success)
				Security::CorruptMemory();
			VM_SHARK_BLACK_END
			break;
		}
		case ResponseType::Ping:
		{
			break;
		}
	}
}

void Communication::onDisconnection(const pipe_ret_t& ret)
{
	VM_SHARK_BLACK_START
	Security::CorruptMemory();
	VM_SHARK_BLACK_END
}

void Communication::ConnectToServer()
{
	VM_FISH_RED_START
	STR_ENCRYPT_START
	client_observer_t observer;
	observer.wantedIp = "198.251.89.179";
	observer.incoming_packet_func = onIncomingMessage;
	observer.disconnected_func = onDisconnection;
	TCPClient.subscribe(observer);

	pipe_ret_t connectRet = TCPClient.connectTo("198.251.89.179", 9999);
	if (connectRet.success)
	{
		// Send initial Handshake, to get RSA Encrypted Client Key and IV
		HandshakeRequest handshakePacket = HandshakeRequest();

		if (const pipe_ret_t sendRet = TCPClient.sendBytes(handshakePacket.Data); !sendRet.success)
		{
			Security::CorruptMemory();
		}
		
		ThreadCheckerHandle = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(checkerThread), nullptr, 0, nullptr);
	}
	else Security::CorruptMemory();
	STR_ENCRYPT_END
	VM_FISH_RED_END
}
