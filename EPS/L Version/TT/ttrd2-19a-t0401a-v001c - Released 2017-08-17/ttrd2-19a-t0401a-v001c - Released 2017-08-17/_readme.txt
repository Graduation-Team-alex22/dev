/*----------------------------------------------------------------------------*-

   Time-Triggered Reference Design [ttrd2-19a-t0401a-v001c]

   ---------------------------------------------------------------------------
   
   TTRD2-19a targets an STM32F401RE MCU. 
   It is designed to run on a Nucleo-F401RE board. 
   An external 16 MHz xtal is assumed to be present.

   TTRD2-19a implements a complete ‘CorrelaTTor-A’ platform. This includes 
   a TTC scheduler plus iWDT, MoniTTor and PredicTTor mechanisms.

   TTRD2-19a uses task timing data that were created using TTRD2-a07a.
   TTRD2-19a also uses task-sequence data that were created using TTRD2-a08a.

   Optionally, TTRD2-19a performs system resets every few seconds. The reset 
   process involves repeating all of the core ‘Power On Self Tests (POSTs). 
   Before performing the reset, part of the Processor state is stored: the 
   state is recovered after the tests and the system continues. This type of 
   periodic reset is sometimes employed in designs that must operate in 
   very ‘hostile’ environments (e.g. with high levels of EMI or radiation).

   Please see ‘ERES2’ Chapter 19 and Appendix 3 for further information 
   about this TTRD.

   ---------------------------------------------------------------------------

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
   described in ERES2 requires purchase of a ReliabiliTTy Technology 
   Licence: http://www.safetty.net/technology/reliabilitty-technology-licences

   Please contact SafeTTy Systems Ltd if you require clarification of these 
   licensing arrangements: http://www.safetty.net/contact

   CorrelaTTor, DuplicaTTor, MoniTTor, PredicTTor, ReliabiliTTy, SafeTTy, 
   SafeTTy Systems and WarranTTor are registered trademarks or trademarks 
   of SafeTTy Systems Ltd in the UK and other countries.

-*----------------------------------------------------------------------------*/
