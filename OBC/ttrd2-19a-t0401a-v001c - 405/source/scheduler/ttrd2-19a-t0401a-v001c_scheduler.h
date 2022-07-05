/*----------------------------------------------------------------------------*-
  
   ttrd2-19a-t0401a-v001c_scheduler.h (Release 2017-08-17a)

  ----------------------------------------------------------------------------

   See ttrd2-19a-t0401a-v001c_scheduler.c for details.

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

#ifndef _SCHEDULER_H
#define _SCHEDULER_H 1

// Processor Header
#include "../main/main.h"

// ------ Public data type declarations ----------------------------

// User-defined type to store required data for each task
typedef struct 
   {
   // Pointer to the task 
   // (must be a 'uint32_t (void)' function)
   uint32_t (*pTask) (void);

   // Delay (ticks) until the task will (next) be run
   uint32_t Delay;       

   // Interval (ticks) between subsequent runs.
   uint32_t Period;

   // Worst-case execution time (microseconds)
   uint32_t WCET;

   // Best-case execution time (microseconds)
   uint32_t BCET;
   } sTask_t;

// ------ Public function prototypes (declarations) --------------------------

void SCH_Init_Microseconds(const uint32_t TICKus);    
void SCH_Start(void);
void SCH_Dispatch_Tasks(void);

void SCH_Add_Task(uint32_t (* pTask)(),
                  const uint32_t DELAY,   // Ticks
                  const uint32_t PERIOD,  // Ticks
                  const uint32_t WCET,    // Microseconds
                  const uint32_t BCET);   // Microseconds
                  
// ------ Public constants -------------------------------------------------------------
// The maximum number of tasks required at any one time
// during the execution of the program
//
// MUST BE CHECKED FOR EACH PROJECT (*not* dynamic)
#define SCH_MAX_TASKS (15)

// Usually set to 1, unless 'Long Tasks' are employed
#define SCH_TICK_COUNT_LIMIT (1)

// Default value for pTask (no task at this location)
#define SCH_NULL_PTR ((uint32_t (*) (void)) 0)
 
#endif

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
