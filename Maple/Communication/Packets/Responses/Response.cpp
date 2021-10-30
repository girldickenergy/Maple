#include "Response.h"

#include "HandshakeResponse.h"
#include "HeartbeatResponse.h"

#include "../../../Utilities/Strings/StringUtilities.h"

Response::Response(const char* msg, size_t size)
{
	RawResponse = msg;
	ResponseSize = size;
	Type = static_cast<ResponseType>(RawResponse[0]);
	
	RawData = StringUtilities::Split(std::string(RawResponse, ResponseSize));
	RawData.erase(RawData.begin());
}

void* Response::ConstructResponse(const char* msg, size_t size, MatchedClient* matchedClient)
{
	VM_FISH_RED_START
	auto type = static_cast<ResponseType>(msg[0]);
	switch (type)
	{
		case ResponseType::Handshake:
			return new HandshakeResponse(msg, size);
		case ResponseType::Heartbeat:
			return new HeartbeatResponse(msg, size, matchedClient);
	}
	auto ret = new Response(msg, size);
	VM_FISH_RED_END
	return ret;
}
