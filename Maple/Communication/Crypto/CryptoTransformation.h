#pragma once
#include <cstdint>

enum CryptoTransformation : uint8_t
{
	Xor,
	Shl,
	Shr,
	Shlr,
	Rol,
	Ror,
	Rolr,
	Add,
	Sub,
	Cancer
};