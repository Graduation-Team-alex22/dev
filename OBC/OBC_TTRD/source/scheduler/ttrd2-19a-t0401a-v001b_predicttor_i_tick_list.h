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
#define LENGTH_OF_TICK_LIST_TASKS_NORMAL 508

// Length of "Task Sequence Initialisation Period"
// See 'ERES2' (Chapter 19) for details.
#define LENGTH_OF_TSIP_TASKS_NORMAL 23

// Tick list applies to all "Normal" modes
uint8_t TICK_LIST_NORMAL[LENGTH_OF_TICK_LIST_TASKS_NORMAL] =
{
0,2,5,6,                                  // Tick : 0000000010
0,5,                                    // Tick : 0000000011
0,2,5,                                  // Tick : 0000000012
0,5,                                    // Tick : 0000000013
0,2,5,                                  // Tick : 0000000014
0,5,                                    // Tick : 0000000015
0,2,5,                                  // Tick : 0000000016
0,5,                                    // Tick : 0000000017
0,2,5,                                  // Tick : 0000000018
0,5,                                    // Tick : 0000000019
0,2,5,                                  // Tick : 0000000020
0,5,                                    // Tick : 0000000021
0,2,5,                                  // Tick : 0000000022
0,5,                                    // Tick : 0000000023
0,2,5,                                  // Tick : 0000000024
0,5,                                    // Tick : 0000000025
0,2,5,                                  // Tick : 0000000026
0,5,                                    // Tick : 0000000027
0,2,5,                                  // Tick : 0000000028
0,5,                                    // Tick : 0000000029
0,2,5,                                  // Tick : 0000000030
0,5,                                    // Tick : 0000000031
0,2,5,                                  // Tick : 0000000032
0,5,                                    // Tick : 0000000033
0,2,5,                                  // Tick : 0000000034
0,5,                                    // Tick : 0000000035
0,2,5,                                  // Tick : 0000000036
0,5,                                    // Tick : 0000000037
0,2,5,                                  // Tick : 0000000038
0,5,                                    // Tick : 0000000039
0,2,5,                                  // Tick : 0000000040
0,5,                                    // Tick : 0000000041
0,2,5,                                  // Tick : 0000000042
0,5,                                    // Tick : 0000000043
0,2,5,                                  // Tick : 0000000044
0,5,                                    // Tick : 0000000045
0,2,5,                                  // Tick : 0000000046
0,5,                                    // Tick : 0000000047
0,2,5,                                  // Tick : 0000000048
0,5,                                    // Tick : 0000000049
0,2,5,                                  // Tick : 0000000050
0,5,                                    // Tick : 0000000051
0,2,5,                                  // Tick : 0000000052
0,5,                                    // Tick : 0000000053
0,2,5,                                  // Tick : 0000000054
0,5,                                    // Tick : 0000000055
0,2,5,                                  // Tick : 0000000056
0,5,                                    // Tick : 0000000057
0,2,5,                                  // Tick : 0000000058
0,5,                                    // Tick : 0000000059
0,2,5,6,                                  // Tick : 0000000060
0,5,                                    // Tick : 0000000061
0,2,5,                                  // Tick : 0000000062
0,5,                                    // Tick : 0000000063
0,2,5,                                  // Tick : 0000000064
0,5,                                    // Tick : 0000000065
0,2,5,                                  // Tick : 0000000066
0,5,                                    // Tick : 0000000067
0,2,5,                                  // Tick : 0000000068
0,5,                                    // Tick : 0000000069
0,2,5,                                  // Tick : 0000000070
0,5,                                    // Tick : 0000000071
0,2,5,                                  // Tick : 0000000072
0,5,                                    // Tick : 0000000073
0,2,5,                                  // Tick : 0000000074
0,5,                                    // Tick : 0000000075
0,2,5,                                  // Tick : 0000000076
0,5,                                    // Tick : 0000000077
0,2,5,                                  // Tick : 0000000078
0,5,                                    // Tick : 0000000079
0,2,5,                                  // Tick : 0000000080
0,5,                                    // Tick : 0000000081
0,2,5,                                  // Tick : 0000000082
0,5,                                    // Tick : 0000000083
0,2,5,                                  // Tick : 0000000084
0,5,                                    // Tick : 0000000085
0,2,5,                                  // Tick : 0000000086
0,5,                                    // Tick : 0000000087
0,2,5,                                  // Tick : 0000000088
0,5,                                    // Tick : 0000000089
0,2,5,                                  // Tick : 0000000090
0,5,                                    // Tick : 0000000091
0,2,5,                                  // Tick : 0000000092
0,5,                                    // Tick : 0000000093
0,2,5,                                  // Tick : 0000000094
0,5,                                    // Tick : 0000000095
0,2,5,                                  // Tick : 0000000096
0,5,                                    // Tick : 0000000097
0,2,5,                                  // Tick : 0000000098
0,5,                                    // Tick : 0000000099
0,2,3,5,                                // Tick : 0000000100
0,5,                                    // Tick : 0000000101
0,2,5,                                  // Tick : 0000000102
0,5,                                    // Tick : 0000000103
0,2,5,                                  // Tick : 0000000104
0,5,                                    // Tick : 0000000105
0,2,5,                                  // Tick : 0000000106
0,5,                                    // Tick : 0000000107
0,2,5,                                  // Tick : 0000000108
0,5,                                    // Tick : 0000000109
0,2,5,6,                                  // Tick : 0000000110
0,5,                                    // Tick : 0000000111
0,2,5,                                  // Tick : 0000000112
0,5,                                    // Tick : 0000000113
0,2,5,                                  // Tick : 0000000114
0,5,                                    // Tick : 0000000115
0,2,5,                                  // Tick : 0000000116
0,5,                                    // Tick : 0000000117
0,2,5,                                  // Tick : 0000000118
0,5,                                    // Tick : 0000000119
0,2,5,                                  // Tick : 0000000120
0,5,                                    // Tick : 0000000121
0,2,5,                                  // Tick : 0000000122
0,5,                                    // Tick : 0000000123
0,2,5,                                  // Tick : 0000000124
0,5,                                    // Tick : 0000000125
0,2,5,                                  // Tick : 0000000126
0,5,                                    // Tick : 0000000127
0,2,5,                                  // Tick : 0000000128
0,5,                                    // Tick : 0000000129
0,2,5,                                  // Tick : 0000000130
0,5,                                    // Tick : 0000000131
0,2,5,                                  // Tick : 0000000132
0,5,                                    // Tick : 0000000133
0,2,5,                                  // Tick : 0000000134
0,5,                                    // Tick : 0000000135
0,2,5,                                  // Tick : 0000000136
0,5,                                    // Tick : 0000000137
0,2,5,                                  // Tick : 0000000138
0,5,                                    // Tick : 0000000139
0,2,5,                                  // Tick : 0000000140
0,5,                                    // Tick : 0000000141
0,2,5,                                  // Tick : 0000000142
0,5,                                    // Tick : 0000000143
0,2,5,                                  // Tick : 0000000144
0,5,                                    // Tick : 0000000145
0,2,5,                                  // Tick : 0000000146
0,5,                                    // Tick : 0000000147
0,2,5,                                  // Tick : 0000000148
0,5,                                    // Tick : 0000000149
0,2,5,                                  // Tick : 0000000150
0,5,                                    // Tick : 0000000151
0,2,5,                                  // Tick : 0000000152
0,5,                                    // Tick : 0000000153
0,2,5,                                  // Tick : 0000000154
0,5,                                    // Tick : 0000000155
0,2,5,                                  // Tick : 0000000156
0,5,                                    // Tick : 0000000157
0,2,5,                                  // Tick : 0000000158
0,5,                                    // Tick : 0000000159
0,2,5,6,                                  // Tick : 0000000160
0,5,                                    // Tick : 0000000161
0,2,5,                                  // Tick : 0000000162
0,5,                                    // Tick : 0000000163
0,2,5,                                  // Tick : 0000000164
0,5,                                    // Tick : 0000000165
0,2,5,                                  // Tick : 0000000166
0,5,                                    // Tick : 0000000167
0,2,5,                                  // Tick : 0000000168
0,5,                                    // Tick : 0000000169
0,2,5,                                  // Tick : 0000000170
0,5,                                    // Tick : 0000000171
0,2,5,                                  // Tick : 0000000172
0,5,                                    // Tick : 0000000173
0,2,5,                                  // Tick : 0000000174
0,5,                                    // Tick : 0000000175
0,2,5,                                  // Tick : 0000000176
0,5,                                    // Tick : 0000000177
0,2,5,                                  // Tick : 0000000178
0,5,                                    // Tick : 0000000179
0,2,5,                                  // Tick : 0000000180
0,5,                                    // Tick : 0000000181
0,2,5,                                  // Tick : 0000000182
0,5,                                    // Tick : 0000000183
0,2,5,                                  // Tick : 0000000184
0,5,                                    // Tick : 0000000185
0,2,5,                                  // Tick : 0000000186
0,5,                                    // Tick : 0000000187
0,2,5,                                  // Tick : 0000000188
0,5,                                    // Tick : 0000000189
0,2,5,                                  // Tick : 0000000190
0,5,                                    // Tick : 0000000191
0,2,5,                                  // Tick : 0000000192
0,5,                                    // Tick : 0000000193
0,2,5,                                  // Tick : 0000000194
0,5,                                    // Tick : 0000000195
0,2,5,                                  // Tick : 0000000196
0,5,                                    // Tick : 0000000197
0,2,5,                                  // Tick : 0000000198
0,5,                                    // Tick : 0000000199
0,1,2,3,4,5,                           // Tick : 0000000200
0,5,                                    // Tick : 0000000201
0,2,5,                                  // Tick : 0000000202
0,5,                                    // Tick : 0000000203
0,2,5,                                  // Tick : 0000000204
0,5,                                    // Tick : 0000000205
0,2,5,                                  // Tick : 0000000206
0,5,                                    // Tick : 0000000207
0,2,5,                                  // Tick : 0000000208
0,5,                                    // Tick : 0000000209
};

#endif
/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
