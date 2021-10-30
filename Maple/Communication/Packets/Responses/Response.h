#pragma once

#include <string>
#include <vector>

#include <ThemidaSDK.h>

#include "ResponseType.h"
#include "../../MatchedClient.h"

class Response
{
public:
	const char* RawResponse;
	size_t ResponseSize;
	ResponseType Type;
	std::vector<std::string> RawData;

	Response(const char* msg, size_t size);

	static void* ConstructResponse(const char* msg, size_t size, MatchedClient* matchedClient);
};
