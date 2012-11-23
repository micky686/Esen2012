#ifndef NODE_CONFIG_H
#define NODE_CONFIG_H
/* Copyright (c) 2004, 2005, 2007 Christian Trödhandl
   All rights reserved.
 
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
 
   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.
 
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */
 
/*
 * include/node_config.h
 *      General node configuration file: Includes a specific node configuration
 *      file depending on the set node HW type.
 *      Default is board Mega128 V2.2 
 *
 * $Id: node_config.h,v 1.3 2007-01-18 19:18:47 bernd Exp $
 *
 * Revision History:
 *   2004/03/07 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *   2004/05/04 (0.2): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - Added SLOTLENGTH_MAX, SLOTLENGTH_MIN, and BIT_TOLERANCE
 *   2005/07/29 (0.4): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - Added NODECONF_MEGA128_V3_0
 *   2005/10/04 (0.5): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - Added NODECONF_MEGA128_PCMCIA
 *     - Added NODECONF_MEGA168
 *     - Added NODECONF_SER_V0_1
 *   2007/01/09 (0.6): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - Added NODECONF_MEGA128_V4_0
 *
 */
#include <avr/io.h>

#if defined (NODECONF_MEGA128_PCMCIA)
#include "node_config_mega128-pcmcia.h"
#elif defined (NODECONF_MEGA128_V5_0)
#include "node_config_mega128-v5.0.h"
#elif defined (NODECONF_MEGA128_V4_0)
#include "node_config_mega128-v4.0.h"
#elif defined (NODECONF_MEGA128_V3_0)
#include "node_config_mega128-v3.0.h"
#elif defined (NODECONF_MEGA128_SER_V0_1)
#include "node_config_mega128-ser_v0.1.h"
#elif defined (NODECONF_MEGA128_V2_2)
#include "node_config_mega128-v2.2.h"
#elif defined (NODECONF_MEGA103)
#include "node_config_mega103.h"
#elif defined (NODECONF_MEGA168)
#include "node_config_mega168.h"
#elif defined (NODECONF_MEGA8)
#include "node_config_mega8.h"
#elif defined (NODECONF_MEGA8515)
#include "node_config_mega8515.h"
#else
#error No node type specified. Node type must be set with "-Dnode_type", where node_type is NODECONF_MEGA128_V2_2, NODECONF_MEGA103, NODECONF_MEGA8 or NODECONF_MEGA8515
/*
#include "node_config_mega128-v2.2.h"
*/
#endif


#if !defined (BAUDRATE)
#define BAUDRATE 19200
#endif /* BAUDRATE */

#if !defined (BITS_PER_SLOT)
#define BITS_PER_SLOT 13
#endif /* BITS_PER_SLOT */

#define BITLENGTH_NOMINAL CLOCKRATE/BAUDRATE
#define SLOTLENGTH_NOMINAL (BITLENGTH_NOMINAL*BITS_PER_SLOT)

#if !defined (BITLENGTH_MAX)
#define BITLENGTH_MAX (uint16_t) (BITLENGTH_NOMINAL*1.5)
#endif /* BITLENGTH_MAX */
#if !defined (BITLENGTH_MIN)
#define BITLENGTH_MIN (uint16_t) (BITLENGTH_NOMINAL*0.75)
#endif /* BITLENGTH_MIN */

#if !defined (SLOTLENGTH_MAX)
#define SLOTLENGTH_MAX (BITLENGTH_MAX*BITS_PER_SLOT)
#endif /* SLOTLENGTH_MAX */
#if !defined (SLOTLENGTH_MIN)
#define SLOTLENGTH_MIN (BITLENGTH_MIN*BITS_PER_SLOT)
#endif /* SLOTLENGTH_MIN */

#if !defined (BIT_TOLERANCE)
#define BIT_TOLERANCE ((BITLENGTH_MIN)>>2)
#endif /* BIT_TOLERANCE */

#endif /* NODE_CONFIG_H */
