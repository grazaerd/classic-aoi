#include "socket.hpp"

#include <cstdint>
#include <memory>
#include <winsock2.h>
#include <mstcpip.h>
#include <Ws2tcpip.h>

#include "offset.hpp"
#include "mem.hpp"

namespace Socket {

    int hksetsockopt(SOCKET s, int level, int optname, const char* optval, int optlen) {
        const auto val = setsockopt(s, SOL_SOCKET, 4098, optval, 4);
        BOOL nodelay = TRUE;

        DWORD ackFrequency = 1;
        DWORD bytes = 0;

        setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*)&nodelay, sizeof(nodelay));
        WSAIoctl(s, SIO_TCP_SET_ACK_FREQUENCY, &ackFrequency, sizeof(ackFrequency),
                 NULL, 0, &bytes, NULL, NULL);

        return val;
    }
    
    int hkconnect(SOCKET s, const sockaddr *name, int namelen) {
        BOOL nodelay = TRUE;
        DWORD ackFrequency = 1;
        DWORD bytes = 0;

        setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*)&nodelay, sizeof(nodelay));
        WSAIoctl(s, SIO_TCP_SET_ACK_FREQUENCY, &ackFrequency, sizeof(ackFrequency),
                 NULL, 0, &bytes, NULL, NULL);


        return connect(s,name, namelen);
    }
    
    void Initialize() {

        if (ADRS::SetSocket != nullptr) {
            uint64_t pSocket = 0ull;
		    pSocket = Memory::moduleBase + Memory::FindRVA(Memory::moduleBase, std::bit_cast<void*>(ADRS::SetSocket), 2, 6);
			Memory::IatMod(std::bit_cast<uint64_t*>(ADRS::SetSocket), hksetsockopt);
        }
        
        if (ADRS::Connect != nullptr) {
            uint64_t pConnect = 0ull;
		    pConnect = Memory::moduleBase + Memory::FindRVA(Memory::moduleBase, std::bit_cast<void*>(ADRS::Connect), 2, 6);
			Memory::IatMod(std::bit_cast<uint64_t*>(ADRS::Connect), hkconnect);
        }
        
    }
    
}
