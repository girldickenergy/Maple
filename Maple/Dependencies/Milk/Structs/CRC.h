
struct CRC
{
    int unknown1;

    void* functionPointer;

    int unknown2;

    int functionSize;

    int unknown3;

    int checksum;

    int something1;
    int something2;
    int something3;

    char className[256];

    int unknown4;

    char functionName[256];
    CRC* nextEntry;
};