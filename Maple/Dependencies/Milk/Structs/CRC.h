struct CRC
{
	uint32_t functionName;          // 0x00
	uint32_t functionNameXORKey;    // 0x04
	uint32_t className;             // 0x08
	uint32_t classNameXORKey;       // 0x0C
    uint32_t unk1;					// 0x10
    uint32_t unk1XORKey;            // 0x14
    uint32_t unk2;                  // 0x18
    uint32_t unk2XORKey;            // 0x1C
	uint32_t functionPointer;       // 0x20
	uint32_t functionPointerXORKey; // 0x24
	uint32_t functionSize;          // 0x28
	uint32_t functionSizeXORKey;    // 0x2C
    uint32_t checksum;          // 0x30
    uint32_t checksumXORKey;    // 0x34
    uint32_t unk3;          // 0x38
    uint32_t unk3XORKey;    // 0x3C
};