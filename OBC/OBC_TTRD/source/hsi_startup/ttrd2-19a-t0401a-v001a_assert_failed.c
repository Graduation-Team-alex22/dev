/*----------------------------------------------------------------------------*-

   ttrd2-19a-t0401a-v001a_assert_failed.c (Release 2017-03-06a)

  ----------------------------------------------------------------------------

   Used to handle parameter-check failures in library code.
    
   See stm32f4xx_conf.h

   NOTE: Needs to be adapted to meet the needs of a given application!

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

// Processor Header
#include "../main/main.h"

// Support for UART / USB reporting on Nucleo board
#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"

/*----------------------------------------------------------------------------*/

void assert_failed(uint8_t* file, uint32_t line)
   {
   // We treat this as a Fatal Platform Failure
   // OTHER BEHAVIOUR MAY BE MORE APPROPRIATE IN YOUR APPLICATION
   // Where possible, you may wish to store the file and line data
   
   UART2_BUF_O_Write_String_To_Buffer("assert_failed\n");
   UART2_BUF_O_Write_String_To_Buffer((char*) file);
   UART2_BUF_O_Write_String_To_Buffer("  Line:  "); 
   UART2_BUF_O_Write_Number03_To_Buffer(line);
   UART2_BUF_O_Write_String_To_Buffer("\n"); 
   UART2_BUF_O_Send_All_Data();

   PROCESSOR_Perform_Safe_Shutdown(PFC_HSI_LIBRARY);     
   }

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
