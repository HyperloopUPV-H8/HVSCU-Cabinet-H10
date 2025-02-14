#include "HALALMock/Services/Communication/SPI/SPI.hpp"
#include "HALALMock/Services/Communication/FDCAN/FDCAN.hpp"
#include "HALALMock/Services/Communication/UART/UART.hpp"
#include "HALALMock/Services/Encoder/Encoder.hpp"
#include "HALALMock/Services/InputCapture/InputCapture.hpp"
#include "HALALMock/Services/ADC/ADC.hpp"
#include "HALALMock/Services/EXTI/EXTI.hpp"
#include "HALALMock/Services/SharedMemory/SharedMemory.hpp"
#include "HALALMock/Models/PinModel/Pin.hpp"
#include <netinet/in.h>
#include "HALALMock/Models/Packets/Packet.hpp"
#include "HALALMock/Models/Packets/Order.hpp"

LogConf Log::config =
    LogConf::Error | LogConf::Fatal | LogConf::Console | LogConf::File;
std::string Log::filename = "STLIB.log";

map<uint16_t,Order*> Order::orders = {};
map<uint16_t,Packet*> Packet::packets = {};

/************************************************
 *                 SharedMemory
 ***********************************************/
const char *SHM::gpio_memory_name = "gpio__blinking_led";
const char *SHM::state_machine_memory_name = "state_machine__blinking_led";

unordered_map<Pin, size_t> SHM::pin_offsets{
	{PA0, 0}, {PA1, 1}, {PA2, 2}, {PA3, 3}, {PA4, 4},
	{PA5, 5}, {PA6, 6}, {PA7, 7}, {PA8, 8}, {PA9, 9},
	{PA10, 10}, {PA11, 11}, {PA12, 12}, {PA13, 13}, 
	{PA14, 14}, {PA15, 15}, {PB0, 16}, {PB1, 17},
	{PB2, 18}, {PB3, 19}, {PB4, 20}, {PB5, 21},
	{PB6, 22}, {PB7, 23}, {PB8, 24}, {PB9, 25},
	{PB10, 26}, {PB11, 27}, {PB12, 28}, {PB13, 29},
	{PB14, 30}, {PB15, 31}, {PC0, 32}, {PC1, 33},
	{PC2, 34}, {PC3, 35}, {PC4, 36}, {PC5, 37},
	{PC6, 38}, {PC7, 39}, {PC8, 40}, {PC9, 41},
	{PC10, 42}, {PC11, 43}, {PC12, 44}, {PC13, 45},
	{PC14, 46}, {PC15, 47}, {PD0, 48}, {PD1, 49},
	{PD2, 50}, {PD3, 51}, {PD4, 52}, {PD5, 53},
	{PD6, 54}, {PD7, 55}, {PD8, 56}, {PD9, 57},
	{PD10, 58}, {PD11, 59}, {PD12, 60}, {PD13, 61},
	{PD14, 62}, {PD15, 63}, {PE0, 64}, {PE1, 65},
	{PE2, 66}, {PE3, 67}, {PE4, 68}, {PE5, 69},
	{PE6, 70}, {PE7, 71}, {PE8, 72}, {PE9, 73},
	{PE10, 74}, {PE11, 75}, {PE12, 76}, {PE13, 77},
	{PE14, 78}, {PE15, 79}, {PF0, 80}, {PF1, 81},
	{PF2, 82}, {PF3, 83}, {PF4, 84}, {PF5, 85},
	{PF6, 86}, {PF7, 87}, {PF8, 88}, {PF9, 89},
	{PF10, 90}, {PF11, 91}, {PF12, 92}, {PF13, 93},
	{PF14, 94}, {PF15, 95}, {PG0, 96}, {PG1, 97},
	{PG2, 98}, {PG3, 99}, {PG4, 100}, {PG5, 101},
	{PG6, 102}, {PG7, 103}, {PG8, 104}, {PG9, 105},
	{PG10, 106}, {PG11, 107}, {PG12, 108}, {PG13, 109},
	{PG14, 110}, {PG15, 111}, {PH0, 112}, {PH1, 113}
};


/************************************************
 *              Communication-FDCAN
 ***********************************************/

std::string FDCAN::ip = "";

FDCAN::Instance FDCAN::instance1 = {
    .TX = PD1,
    .RX = PD0,
    .dlc = DLC::BYTES_64,
    .rx_location = 0,
    .fdcan_number = 1,
};

FDCAN::Peripheral FDCAN::fdcan1 = FDCAN::Peripheral::peripheral1;

unordered_map<FDCAN::Peripheral, FDCAN::Instance*> FDCAN::available_fdcans = {
    {FDCAN::fdcan1, &FDCAN::instance1}
};

/************************************************
 *              Communication-SPI
 ***********************************************/

std::string SPI::ip = "192.168.1.10";

SPI::Instance SPI::instance3 = {
    .SCK = &PC10,
    .MOSI = &PC12,
    .MISO = &PC11,
    .SS = &PD3,
    .mode = SPIMode::MASTER,
    .use_DMA = false,
    .port = 2000
};

SPI::Peripheral SPI::spi3 = SPI::Peripheral::peripheral3;

unordered_map<SPI::Peripheral, SPI::Instance*> SPI::available_spi = {
    {SPI::spi3, &SPI::instance3}};

/************************************************
 *                 	  Encoder
 ***********************************************/

map<pair<Pin, Pin>, void*> Encoder::pin_timer_map = {
    {{PC6, PC7}, nullptr}};

/************************************************
 *                 Input Capture
 ***********************************************/

map<Pin, InputCapture::Instance> InputCapture::available_instances = {
    {PF0, InputCapture::Instance{PF0, nullptr, 0, 0}}
};

/************************************************
 *					   ADC
 ***********************************************/

map<Pin, ADC::Instance> ADC::available_instances = {
    {PF11, Instance(ADCResolution::ADC_RES_16BITS)},
    {PF12, Instance(ADCResolution::ADC_RES_16BITS)},
    {PF13, Instance(ADCResolution::ADC_RES_16BITS)},
    {PF14, Instance(ADCResolution::ADC_RES_16BITS)},
    {PF5, Instance(ADCResolution::ADC_RES_12BITS)},
    {PF6, Instance(ADCResolution::ADC_RES_12BITS)},
    {PF7, Instance(ADCResolution::ADC_RES_12BITS)},
    {PF8, Instance(ADCResolution::ADC_RES_12BITS)},
    {PF9, Instance(ADCResolution::ADC_RES_12BITS)},
    {PF10, Instance(ADCResolution::ADC_RES_12BITS)},
    {PC2, Instance(ADCResolution::ADC_RES_16BITS)},
    {PC3, Instance(ADCResolution::ADC_RES_16BITS)},
    {PF10, Instance(ADCResolution::ADC_RES_16BITS)},
    {PC0, Instance(ADCResolution::ADC_RES_16BITS)},
    {PA0, Instance(ADCResolution::ADC_RES_16BITS)},
    {PA3, Instance(ADCResolution::ADC_RES_16BITS)},
    {PA4, Instance(ADCResolution::ADC_RES_16BITS)},
    {PA5, Instance(ADCResolution::ADC_RES_16BITS)},
    {PA6, Instance(ADCResolution::ADC_RES_16BITS)},
    {PB0, Instance(ADCResolution::ADC_RES_16BITS)},
    {PB1, Instance(ADCResolution::ADC_RES_16BITS)}
};

/************************************************
 *					   EXTI
 ***********************************************/

map<uint16_t, ExternalInterrupt::Instance> ExternalInterrupt::instances = {
    {PE0.gpio_pin, Instance()},
    {PE1.gpio_pin, Instance()}
};

/************************************************
 *					   UART
 ***********************************************/

UART::Peripheral UART::uart2 = UART::Peripheral::peripheral2;
