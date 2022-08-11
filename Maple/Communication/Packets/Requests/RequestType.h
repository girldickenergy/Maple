#pragma once

enum class RequestType
{
	Handshake = 0xA0,
	Heartbeat = 0xC2,
	Ping = 0xC3
};