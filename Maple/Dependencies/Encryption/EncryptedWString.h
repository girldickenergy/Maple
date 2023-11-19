#pragma once

#include "BasicEncryptedString.h"

class EncryptedWString : public BasicEncryptedString<wchar_t>
{
public:
    using BasicEncryptedString<wchar_t>::BasicEncryptedString;
	friend EncryptedWString operator+(EncryptedWString lhs, const EncryptedWString& rhs)
	{
	    lhs += rhs;

	    return lhs;
	}
};
