/*----------------------------------------------------------------------------*-

   ttrd2-19a-t0401a-v001a_monittor_i.h (Release 2017-03-06a)

  ----------------------------------------------------------------------------
   
   - See ttrd2-19a-t0401a-v001a_monittor_i.c for details.

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

   CorrelaTTor, DuplicaTTor, MoniTTor, PredicTTor, ReliabiliTTy, SafeTTy, 
   SafeTTy Systems and WarranTTor are registered trademarks or trademarks 
   of SafeTTy Systems Ltd in the UK and other countries.

-*----------------------------------------------------------------------------*/

#ifndef _MONITTOR_I_H
#define _MONITTOR_I_H
/*
-------------------------------------------------------------------------------------------------
                                          HEADER FILES
-------------------------------------------------------------------------------------------------
*/
#include "../main/main.h"
/*
-------------------------------------------------------------------------------------------------
                                    PRIVATE FUNCTION PROTOTYPES
-------------------------------------------------------------------------------------------------
*/

// Params should have names!!!

void MONITTORi_Init(void);
void MONITTORi_Start(const uint32_t, const uint32_t, const uint32_t, const uint32_t);
void MONITTORi_Stop(void);
void MONITTORi_Disable(void);
 
#endif
/*
=================================================================================================
                                          END OF FILE
=================================================================================================
*/
