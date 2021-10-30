#pragma once

enum class ResponseType : unsigned char
{
	FatalError = 0xE0,
	Handshake = 0xA0,
	Heartbeat = 0xC2,
	Ping = 0xC3
};
