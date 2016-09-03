//*****************************************************************************
//! \file w5100.c
//! \brief W5100 HAL Interface.
//! \version 1.0.0
//! \date 2013/10/21
//! \par  Revision history
//!       <2013/10/21> 1st Release
//! \author MidnightCow
//!
//! Copyright (c)  2013, WIZnet Co., LTD.
//! All rights reserved.
//!
//! Redistribution and use in source and binary forms, with or without
//! modification, are permitted provided that the following conditions
//! are met:
//!
//!     * Redistributions of source code must retain the above copyright
//! notice, this list of conditions and the following disclaimer.
//!     * Redistributions in binary form must reproduce the above copyright
//! notice, this list of conditions and the following disclaimer in the
//! documentation and/or other materials provided with the distribution.
//!     * Neither the name of the <ORGANIZATION> nor the names of its
//! contributors may be used to endorse or promote products derived
//! from this software without specific prior written permission.
//!
//! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//! AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//! IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//! ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
//! LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//! CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//! SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//! INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//! CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//! ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
//! THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

#include "w5100.h"

#include <SPI.h>



void Wiznet5100::wizchip_sw_reset()
{
    uint8_t mac[6];
    getSHAR(mac);

    setMR(MR_RST);
    getMR(); // for delay

    setSHAR(mac);
}


void Wiznet5100::wizchip_write(uint16_t AddrSel, uint8_t wb )
{
    wizchip_cs_select();
    SPI.transfer(0xF0);
    SPI.transfer((AddrSel & 0xFF00) >>  8);
    SPI.transfer((AddrSel & 0x00FF) >>  0);
    SPI.transfer(wb);    // Data write (write 1byte data)
    wizchip_cs_deselect();
}

void Wiznet5100::wizchip_write_word(uint16_t AddrSel, uint16_t word)
{
    wizchip_write(AddrSel,   (uint8_t)(word>>8));
    wizchip_write(AddrSel+1, (uint8_t) word);
}

uint8_t Wiznet5100::wizchip_read(uint16_t AddrSel)
{
    uint8_t ret;

    wizchip_cs_select();
    SPI.transfer(0x0F);
    SPI.transfer((AddrSel & 0xFF00) >>  8);
    SPI.transfer((AddrSel & 0x00FF) >>  0);
    ret = SPI.transfer(0);
    wizchip_cs_deselect();

    return ret;
}

uint16_t Wiznet5100::wizchip_read_word(uint16_t AddrSel)
{
    return ((uint16_t)wizchip_read(AddrSel) << 8) + wizchip_read(AddrSel + 1);
}


void Wiznet5100::wizchip_write_buf(uint16_t AddrSel, const uint8_t* pBuf, uint16_t len)
{
    for(uint16_t i = 0; i < len; i++)
    {
        wizchip_write(AddrSel + i, pBuf[i]);
    }
}


void Wiznet5100::wizchip_read_buf(uint16_t AddrSel, uint8_t* pBuf, uint16_t len)
{
    for(uint16_t i = 0; i < len; i++)
    {
        pBuf[i] = wizchip_read(AddrSel + i);
    }
}

void Wiznet5100::setSn_CR(uint8_t cr) {
    // Write the command to the Command Register
    wizchip_write(Sn_CR, cr);
    
    // Now wait for the command to complete
    while( wizchip_read(Sn_CR) );
}

uint16_t Wiznet5100::getSn_TX_FSR()
{
    uint16_t val=0,val1=0;
    do
    {
        val1 = wizchip_read(Sn_TX_FSR);
        val1 = (val1 << 8) + wizchip_read(Sn_TX_FSR + 1);
        if (val1 != 0)
        {
            val = wizchip_read(Sn_TX_FSR);
            val = (val << 8) + wizchip_read(Sn_TX_FSR + 1);
        }
    } while (val != val1);
    return val;
}


uint16_t Wiznet5100::getSn_RX_RSR()
{
    uint16_t val=0,val1=0;
    do
    {
        val1 = wizchip_read(Sn_RX_RSR);
        val1 = (val1 << 8) + wizchip_read(Sn_RX_RSR + 1);
        if (val1 != 0)
        {
            val = wizchip_read(Sn_RX_RSR);
            val = (val << 8) + wizchip_read(Sn_RX_RSR + 1);
        }
    } while (val != val1);
    return val;
}

void Wiznet5100::wizchip_send_data(const uint8_t *wizdata, uint16_t len)
{
    uint16_t ptr;
    uint16_t size;
    uint16_t dst_mask;
    uint16_t dst_ptr;

    ptr = getSn_TX_WR();

    dst_mask = ptr & getSn_TxMASK();
    dst_ptr = TxBufferAddress + dst_mask;

    if (dst_mask + len > getSn_TxMAX())
    {
        size = getSn_TxMAX() - dst_mask;
        wizchip_write_buf(dst_ptr, wizdata, size);
        wizdata += size;
        size = len - size;
        dst_ptr = TxBufferAddress;
        wizchip_write_buf(dst_ptr, wizdata, size);
    }
    else
    {
        wizchip_write_buf(dst_ptr, wizdata, len);
    }

    ptr += len;

    setSn_TX_WR(ptr);
}

void Wiznet5100::wizchip_recv_data(uint8_t *wizdata, uint16_t len)
{
    uint16_t ptr;
    uint16_t size;
    uint16_t src_mask;
    uint16_t src_ptr;

    ptr = getSn_RX_RD();

    src_mask = ptr & getSn_RxMASK();
    src_ptr = RxBufferAddress + src_mask;


    if( (src_mask + len) > getSn_RxMAX() )
    {
        size = getSn_RxMAX() - src_mask;
        wizchip_read_buf(src_ptr, wizdata, size);
        wizdata += size;
        size = len - size;
        src_ptr = RxBufferAddress;
        wizchip_read_buf(src_ptr, wizdata, size);
    }
    else
    {
        wizchip_read_buf(src_ptr, wizdata, len);
    }

    ptr += len;

    setSn_RX_RD(ptr);
}


Wiznet5100::Wiznet5100(int8_t cs)
{
    _cs = cs;
}

boolean Wiznet5100::begin(const uint8_t *mac_address)
{
    pinMode(_cs, OUTPUT);
    wizchip_cs_deselect();

    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV4); // 4 MHz?
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);

    wizchip_sw_reset();

    setSHAR(mac_address);

    setSn_MR(Sn_MR_MACRAW);
    setSn_CR(Sn_CR_OPEN);

    if (getSn_SR() != SOCK_MACRAW) {
        Serial.println("Failed to put socket 0 into MACRaw mode");
        return false;
    }

    // Success
    return true;
}

void Wiznet5100::end()
{
    setSn_CR(Sn_CR_CLOSE);

    // clear all interrupt of the socket
    setSn_IR(0xFF);

    // Wait for socket to change to closed
    while(getSn_SR() != SOCK_CLOSED);
}

uint16_t Wiznet5100::readFrame(uint8_t *buffer, uint16_t bufsize)
{
    uint16_t len = getSn_RX_RSR();
    if ( len > 0 )
    {
        uint8_t head[2];
        uint16_t data_len=0;

        wizchip_recv_data(head, 2);
        setSn_CR(Sn_CR_RECV);

        data_len = head[0];
        data_len = (data_len<<8) + head[1];
        data_len -= 2;

        if(data_len > bufsize)
        {
            Serial.println("Packet is bigger than buffer");
            return 0;
        }

        wizchip_recv_data(buffer, data_len );
        setSn_CR(Sn_CR_RECV);

        return data_len;
    }

    return 0;
}

uint16_t Wiznet5100::sendFrame(const uint8_t *buf, uint16_t len)
{
    uint16_t freesize = 0;

    // check size not to exceed MAX size.
    freesize = getSn_TxMAX();
    if (len > freesize) len = freesize;

    // Wait for space in the transmit buffer
    while(1)
    {
        freesize = getSn_TX_FSR();
        if(getSn_SR() == SOCK_CLOSED) {
            Serial.println("Socket closed");
            return -1;
        }
        if(len <= freesize) break;
    };

    wizchip_send_data(buf, len);
    setSn_CR(Sn_CR_SEND);

    while(1)
    {
        uint8_t tmp = getSn_IR();
        if(tmp & Sn_IR_SENDOK)
        {
            setSn_IR(Sn_IR_SENDOK);
            Serial.println("Sn_IR_SENDOK");
            break;
        }
        else if(tmp & Sn_IR_TIMEOUT)
        {
            setSn_IR(Sn_IR_TIMEOUT);
            Serial.println("Timeout");
            return -1;
        }
    }

    return len;
}
