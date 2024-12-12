#include "MatchPacketMaker.h"

MatchPacketMaker::MatchPacketMaker()
{
    ConnectionInfo = INIT;
    packetHeader.headerType = MATCH;
}

int MatchPacketMaker::Serialzed(char* buffer, int size)
{
    char* data = new char[size + sizeof(PacketHeader)];
    int Length = 0;
    Length = PackingHeader(data);

    memcpy(data + Length, &ConnectionInfo, sizeof(ConnectionInfo));
    Length += sizeof(ConnectionInfo);

    memcpy(buffer, data, Length);

    return Length;
}

void MatchPacketMaker::Deserialzed(char* buffer)
{
    int Length = 0;
    Length += UnpackingHeader(buffer);

    memcpy(&ConnectionInfo, buffer + Length, sizeof(ConnectionState));
    Length += sizeof(ConnectionState);

    return;
}
