/*----------------------------------------------------------------------------*-

  ttrd2-a07a-t0401a-v001a_measure_t4.h (Release 2017-03-06a)

  ----------------------------------------------------------------------------
   
  - See ttrd2-a07a-t0401a-v001a_measure_t4.c for details.

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

#ifndef _MEASURE_T4_H
#define _MEASURE_T4_H 1

// ------ Public function prototypes (declarations) --------------------------

void     MEASURE_T4_Init(void);

void     MEASURE_T4_Start_T4(void);
uint32_t MEASURE_T4_Read_T4(void);

uint32_t MEASURE_T4_Update_Task_Timing(uint32_t Index, 
                                       uint32_t Timer_value);

void     MEASURE_T4_Write_Data_To_UART_Buffer(void);

#endif

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
