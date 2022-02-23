/*----------------------------------------------------------------------------*-
  
   ttrd2-19a-t0401a-v001b_processor.h (Release 2017-03-06a)

  ----------------------------------------------------------------------------

   See ttrd2-19a-t0401a-v001b_processor.c for details.

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

#ifndef _PROCESSOR_H
#define _PROCESSOR_H 1

// ------ Public data types (user defined) -----------------------------------

// Supported Processor Modes / States
typedef enum {
   FAIL_SAFE_S,
   STARTUP_01_PROCESSOR_MEMORY_CHECKS_S,
   STARTUP_02_SOFTWARE_CONFIGURATION_CHECKS_S,
   STARTUP_03_ENVIRONMENT_CHECKS_S,
   STARTUP_04_WDT_CHECK_S,
   STARTUP_05a_SCHEDULER_OSC_CHECK_M,
   STARTUP_05b_SCHEDULER_OSC_CHECK_M,
   STARTUP_06a_MONITTOR_ORUN_CHECK_M,
   STARTUP_06b_MONITTOR_URUN_CHECK_M,
   STARTUP_07_PREDICTTOR_CHECK_M,
   NORMAL_M
} eProc_MoSt;


// ------ Public function prototypes (declarations) --------------------------

void       PROCESSOR_Init(void);

void       PROCESSOR_Perform_Safe_Shutdown(const uint32_t PFC);

void       PROCESSOR_Change_MoSt(void);

eProc_MoSt PROCESSOR_Get_MoSt(void);

void       PROCESSOR_Store_Reqd_MoSt(const eProc_MoSt MOST);

void       PROCESSOR_Store_PFC(const uint32_t PFC);

#endif

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
