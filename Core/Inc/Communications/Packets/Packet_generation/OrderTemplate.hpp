#include "ST-LIB.hpp"

//Order packets for %board%
class OrderPacket{
    
%enums%

    private:
        constexpr static size_t size =%size%;
        uint32_t id{0};
    public:
        std::array<StackPacket*,size> packets; 
%packetnames%
        
    OrderPacket(%data%)
{

%packets%

}
};