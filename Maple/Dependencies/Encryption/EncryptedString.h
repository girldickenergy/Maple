#pragma once

#include "BasicEncryptedString.h"

class EncryptedString : public BasicEncryptedString<char>
{
public:
    using BasicEncryptedString<char>::BasicEncryptedString;
	friend EncryptedString operator+(EncryptedString lhs, const EncryptedString& rhs)
	{
	    lhs += rhs;

	    return lhs;
	}
};
