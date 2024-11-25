#pragma once
#include <cstdint>

enum CryptoTransformation : uint8_t
{
	Xor,
	Rol,
	Ror,
	Rolr,
	Add,
	Sub,
	Cancer
};