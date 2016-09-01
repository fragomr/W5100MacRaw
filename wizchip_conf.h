//*****************************************************************************
//
//! \file wizchip_conf.h
//! \brief WIZCHIP Config Header File.
//! \version 1.0.0
//! \date 2013/10/21
//! \par  Revision history
//!       <2015/02/05> Notice
//!        The version history is not updated after this point.
//!        Download the latest version directly from GitHub. Please visit the our GitHub repository for ioLibrary.
//!        >> https://github.com/Wiznet/ioLibrary_Driver
//!       <2013/10/21> 1st Release
//! \author MidnightCow
//! \copyright
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

/**
 * @defgroup extra_functions 2. WIZnet Extra Functions
 *
 * @brief These functions is optional function. It could be replaced at WIZCHIP I/O function because they were made by WIZCHIP I/O functions.  
 * @details There are functions of configuring WIZCHIP, network, interrupt, phy, network information and timer. \n
 * 
 */

#ifndef  _WIZCHIP_CONF_H_
#define  _WIZCHIP_CONF_H_

#include <stdint.h>







/* 
 * The following functions are implemented for internal use. 
 * but You can call these functions for code size reduction instead of ctlwizchip() and ctlnetwork().
 */
 
/**
 * @ingroup extra_functions
 * @brief Reset WIZCHIP by softly.
 */ 
void   wizchip_sw_reset(void);

/**
 * @ingroup extra_functions
 * @brief Initializes WIZCHIP with socket buffer size
 * @param txsize Socket tx buffer sizes. If null, initialized the default size 2KB.
 * @param rxsize Socket rx buffer sizes. If null, initialized the default size 2KB.
 * @return 0 : succcess \n
 *        -1 : fail. Invalid buffer size
 */
int8_t wizchip_init(uint8_t* txsize, uint8_t* rxsize);


#endif   // _WIZCHIP_CONF_H_
