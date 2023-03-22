#pragma once

struct CRC
{
    void* functionPointer;
    int functionSize;
    int checksum;
    int something1;
    int something2;
    char className[256];
    char functionName[256];
    CRC* nextEntry;
};