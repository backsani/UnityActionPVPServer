#include "MatchPacketMaker.h"

MatchPacketMaker::MatchPacketMaker()
{
}

int MatchPacketMaker::Serialzed(char* buffer, int size)
{
    char* data = new char[size + sizeof(PacketHeader)];
    int Length = 0;
    Length = PackingHeader(data);

    return 0;
}

void MatchPacketMaker::Deserialzed(char* buffer)
{
    int Length = 0;
    Length += UnpackingHeader(buffer);
}
