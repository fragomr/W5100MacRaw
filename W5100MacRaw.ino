#include <SPI.h>
#include "w5100.h"

const int sockNum = 0;


void printPaddedHex(uint8_t byte)
{
    char str[2];
    str[0] = (byte >> 4) & 0x0f;
    str[1] = byte & 0x0f;

    for (int i=0; i<2; i++) {
        // base for converting single digit numbers to ASCII is 48
        // base for 10-16 to become lower-case characters a-f is 87
        if (str[i] > 9) str[i] += 39;
        str[i] += 48;
        Serial.print(str[i]);
    }
}

void printMACAddress(const uint8_t address[6])
{
    for (uint8_t i = 0; i < 6; ++i) {
        printPaddedHex(address[i]);
        if (i < 5)
            Serial.print(':');
    }
    Serial.println();
}

int read(uint8_t *buffer, uint16_t bufsize)
{
    uint16_t len = getSn_RX_RSR(sockNum);
    if ( len > 0 )
    {
        uint8_t head[2];
        uint16_t data_len=0;

        wiz_recv_data(sockNum, head, 2);
        setSn_CR(sockNum, Sn_CR_RECV);
        while(getSn_CR(sockNum));

        data_len = head[0];
        data_len = (data_len<<8) + head[1];
        data_len -= 2;

        if(data_len > bufsize)
        {
            Serial.println("Packet is bigger than buffer");
            return 0;
        }

        wiz_recv_data( sockNum, buffer, data_len );
        setSn_CR(sockNum, Sn_CR_RECV);
        while(getSn_CR(sockNum));

        return data_len;
    }

    return 0;
}


void setup() {
    // Setup serial port for debugging
    Serial.begin(38400);
    Serial.println("[W5100MacRaw]");

    byte mac_address[] = {
        0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
    };

    pinMode(SS, OUTPUT);
    digitalWrite(SS, HIGH);

    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV4); // 4 MHz?
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);

    // Initialise the basic info
    wizchip_init(NULL, NULL);

    setSHAR(mac_address);

    setSn_MR(sockNum, Sn_MR_MACRAW);
    setSn_CR(sockNum, Sn_CR_OPEN);

    /* wait to process the command... */
    while( getSn_CR(sockNum) ) ;

    uint8_t socketCommand = getSn_CR(sockNum);
    Serial.print("socketCommand=0x");
    Serial.println(socketCommand, HEX);

    uint8_t socketStatus = getSn_SR(sockNum);
    Serial.print("socketStatus=0x");
    Serial.println(socketStatus, HEX);

    uint8_t retryCount = getRCR();
    Serial.print("retryCount=");
    Serial.println(retryCount, DEC);
}


uint8_t buffer[800];

void loop() {

    uint16_t len = read(buffer, sizeof(buffer));
    if ( len > 0 ) {
        Serial.print("len=");
        Serial.println(len, DEC);

        Serial.print("Dest=");
        printMACAddress(&buffer[0]);
        Serial.print("Src=");
        printMACAddress(&buffer[6]);

        // 0x0800 = IPv4
        // 0x0806 = ARP
        // 0x86DD = IPv6
        Serial.print("Type=0x");
        printPaddedHex(buffer[12]);
        printPaddedHex(buffer[13]);
        Serial.println();

        Serial.println();
    }
}
