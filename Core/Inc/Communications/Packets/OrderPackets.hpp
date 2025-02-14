#include "ST-LIB.hpp"

//Order packets for OBCCU
class OrderPacket{
    


    private:
        constexpr static size_t size =2;
        uint32_t id{0};
    public:
        std::array<StackPacket*,size> packets; 
        StackPacket* Open_Contactors;
        StackPacket* Close_Contactors;

        
    OrderPacket(uint16_t &idpacket902,uint16_t &idpacket903)
{

   Open_Contactors = new StackPacket(idpacket902);
   packets[id] = Open_Contactors;
   id++;
   Close_Contactors = new StackPacket(idpacket903);
   packets[id] = Close_Contactors;
   id++;

}
};