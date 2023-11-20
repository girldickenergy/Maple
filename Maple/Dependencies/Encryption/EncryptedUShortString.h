#pragma once

#include "BasicEncryptedString.h"

class EncryptedUShortString : public BasicEncryptedString<unsigned short>
{
public:
    using BasicEncryptedString<unsigned short>::BasicEncryptedString;
	friend EncryptedUShortString operator+(EncryptedUShortString lhs, const EncryptedUShortString& rhs)
	{
	    lhs += rhs;

	    return lhs;
	}
};
