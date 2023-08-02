struct CRC
{
    uint32_t functionPointer;       // 0x00
    uint32_t functionPointerXORKey; // 0x04
    int functionSize;               // 0x08
    uint32_t functionSizeXORKey;    // 0x0C
    int checksum;                   // 0x10
    uint32_t checksumXORKey;        // 0x14
    char* functionName;             // 0x18
    uint32_t functionNameXORKey;    // 0x1C
    char* className;                // 0x20
    int unknown4;
    int unknown5;
    int unknown6;
    int unknown7;
};