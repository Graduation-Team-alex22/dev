/*----------------------------------------------------------------------------*-

  ttrd2-05a-t0401a-v001a_timeout_t3.h (Release 2017-03-06a)

  ----------------------------------------------------------------------------
   
   - See ttrd2-05a-t0401a-v001a_timeout_t3.c for details.

-*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*-

  This code is copyright (c) 2014-2017 SafeTTy Systems Ltd.

  This code forms part of a Time-Triggered Reference Design (TTRD) that is 
  documented in the following book: 

   Pont, M.J. (2016) 
   "The Engineering of Reliable Embedded Systems (Second Edition)", 
   Published by SafeTTy Systems Ltd. ISBN: 978-0-9930355-3-1.

  Both the TTRDs and the above book ("ERES2") describe patented 
  technology and are subject to copyright and other restrictions.

  This code may be used without charge: [i] by universities and colleges on 
  courses for which a degree up to and including 'MSc' level (or equivalent) 
  is awarded; [ii] for non-commercial projects carried out by individuals 
  and hobbyists.

  Any and all other use of this code and / or the patented technology 
  described in ERES2 requires purchase of a ReliabiliTTy Technology Licence:
  http://www.safetty.net/technology/reliabilitty-technology-licences

  Please contact SafeTTy Systems Ltd if you require clarification of these 
  licensing arrangements: http://www.safetty.net/contact

  CorrelaTTor, DecomposiTTor, DuplicaTTor, MoniTTor, PredicTTor, ReliabiliTTy,  
  SafeTTy, SafeTTy Systems, TriplicaTTor and WarranTTor are registered 
  trademarks or trademarks of SafeTTy Systems Ltd in the UK & other countries.

-*----------------------------------------------------------------------------*/

#include "../main/main.h"

#ifndef _TIMEOUT_T3_H
#define _TIMEOUT_T3_H 1

// ------ Public constants ---------------------------------------------------

// Used to indicate state of timer
#define COUNTING  (0xABCDABCDU)
#define TIMED_OUT (0x54325432U)

// ------ Public function prototypes (declarations) --------------------------

void     TIMEOUT_T3_USEC_Init(void);
void     TIMEOUT_T3_USEC_Start(void);
uint32_t TIMEOUT_T3_USEC_Get_Timer_State(const uint32_t COUNT);

#endif

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
