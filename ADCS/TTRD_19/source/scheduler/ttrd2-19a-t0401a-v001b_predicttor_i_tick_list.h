/*----------------------------------------------------------------------------*-
  
   ttrd2-19a-t0401a-v001a_predicttor_i_tick_list.h (Release 2017-03-06a)

  ----------------------------------------------------------------------------

   See main PredicTTor module for details.
   
   The 'Tick List' in this file was created using ttrd22-a08a.

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

#ifndef _TICK_LIST_H
#define _TICK_LIST_H 1

// The tick lists for each mode
// (No tick list for Fail Safe mode)
#define LENGTH_OF_TICK_LIST_TASKS_NORMAL 265

// Length of "Task Sequence Initialisation Period"
// See 'ERES2' (Chapter 19) for details.
#define LENGTH_OF_TSIP_TASKS_NORMAL 41

// Tick list applies to all "Normal" modes
uint8_t TICK_LIST_NORMAL[LENGTH_OF_TICK_LIST_TASKS_NORMAL] =
{
0,4,17,                                 // Tick : 0000000013
0,4,                                    // Tick : 0000000014
0,4,                                    // Tick : 0000000015
0,4,                                    // Tick : 0000000016
0,4,                                    // Tick : 0000000017
0,4,                                    // Tick : 0000000018
0,4,                                    // Tick : 0000000019
0,4,                                    // Tick : 0000000020
0,4,5,                                  // Tick : 0000000021
0,4,6,                                  // Tick : 0000000022
0,4,7,                                  // Tick : 0000000023
0,4,8,                                  // Tick : 0000000024
0,4,9,                                  // Tick : 0000000025
0,4,10,                                 // Tick : 0000000026
0,4,11,                                 // Tick : 0000000027
0,4,12,                                 // Tick : 0000000028
0,4,13,                                 // Tick : 0000000029
0,4,14,                                 // Tick : 0000000030
0,4,15,                                 // Tick : 0000000031
0,4,16,                                 // Tick : 0000000032
0,4,                                    // Tick : 0000000033
0,4,                                    // Tick : 0000000034
0,4,                                    // Tick : 0000000035
0,4,                                    // Tick : 0000000036
0,4,                                    // Tick : 0000000037
0,4,                                    // Tick : 0000000038
0,4,                                    // Tick : 0000000039
0,4,                                    // Tick : 0000000040
0,4,5,                                  // Tick : 0000000041
0,4,6,                                  // Tick : 0000000042
0,4,7,                                  // Tick : 0000000043
0,4,8,                                  // Tick : 0000000044
0,4,9,                                  // Tick : 0000000045
0,4,10,                                 // Tick : 0000000046
0,4,11,                                 // Tick : 0000000047
0,4,12,                                 // Tick : 0000000048
0,4,13,                                 // Tick : 0000000049
0,2,4,14,                               // Tick : 0000000050
0,4,15,                                 // Tick : 0000000051
0,4,16,                                 // Tick : 0000000052
0,4,                                    // Tick : 0000000053
0,4,                                    // Tick : 0000000054
0,4,                                    // Tick : 0000000055
0,4,                                    // Tick : 0000000056
0,4,                                    // Tick : 0000000057
0,4,                                    // Tick : 0000000058
0,4,                                    // Tick : 0000000059
0,4,                                    // Tick : 0000000060
0,4,5,                                  // Tick : 0000000061
0,4,6,                                  // Tick : 0000000062
0,4,7,                                  // Tick : 0000000063
0,4,8,                                  // Tick : 0000000064
0,4,9,                                  // Tick : 0000000065
0,4,10,                                 // Tick : 0000000066
0,4,11,                                 // Tick : 0000000067
0,4,12,                                 // Tick : 0000000068
0,4,13,                                 // Tick : 0000000069
0,4,14,                                 // Tick : 0000000070
0,4,15,                                 // Tick : 0000000071
0,4,16,                                 // Tick : 0000000072
0,4,                                    // Tick : 0000000073
0,4,                                    // Tick : 0000000074
0,4,                                    // Tick : 0000000075
0,4,                                    // Tick : 0000000076
0,4,                                    // Tick : 0000000077
0,4,                                    // Tick : 0000000078
0,4,                                    // Tick : 0000000079
0,4,                                    // Tick : 0000000080
0,4,5,                                  // Tick : 0000000081
0,4,6,                                  // Tick : 0000000082
0,4,7,                                  // Tick : 0000000083
0,4,8,                                  // Tick : 0000000084
0,4,9,                                  // Tick : 0000000085
0,4,10,                                 // Tick : 0000000086
0,4,11,                                 // Tick : 0000000087
0,4,12,                                 // Tick : 0000000088
0,4,13,                                 // Tick : 0000000089
0,4,14,                                 // Tick : 0000000090
0,4,15,                                 // Tick : 0000000091
0,4,16,                                 // Tick : 0000000092
0,4,                                    // Tick : 0000000093
0,4,                                    // Tick : 0000000094
0,4,                                    // Tick : 0000000095
0,4,                                    // Tick : 0000000096
0,4,                                    // Tick : 0000000097
0,4,                                    // Tick : 0000000098
0,4,                                    // Tick : 0000000099
0,1,2,3,4,                              // Tick : 0000000100
0,4,5,                                  // Tick : 0000000101
0,4,6,                                  // Tick : 0000000102
0,4,7,                                  // Tick : 0000000103
0,4,8,                                  // Tick : 0000000104
0,4,9,                                  // Tick : 0000000105
0,4,10,                                 // Tick : 0000000106
0,4,11,                                 // Tick : 0000000107
0,4,12,                                 // Tick : 0000000108
0,4,13,                                 // Tick : 0000000109
0,4,14,                                 // Tick : 0000000110
0,4,15,                                 // Tick : 0000000111
0,4,16,                                 // Tick : 0000000112
};

#endif
/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
