struct CRC
{
	uint32_t functionPointer;       // 0x00
	uint32_t functionPointerXORKey; // 0x04
	uint32_t functionSize;          // 0x08
	uint32_t functionSizeXORKey;    // 0x0C
	uint32_t checksum;              // 0x10
	uint32_t checksumXORKey;        // 0x14
	uint32_t functionName;          // 0x18
	uint32_t functionNameXORKey;    // 0x1C
	uint32_t className;             // 0x20
	uint32_t classNameXORKey;       // 0x24
};