/*----------------------------------------------------------------------------*-

  ttrd2-19a-t0401a-v001b_predicttor_i.c (Release 2017-03-06a)
  
  ----------------------------------------------------------------------------

  PredicTTor mechanism for STM32F401RE.

  This version assumes internal representations of task sequences
  (there may be one sequence for each Mode).  
  
  This version supports asynchronous task sets.

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

// Tick list data (TSR)
#include "ttrd2-19a-t0401a-v001b_predicttor_i_tick_list.h"

// ------ Private Duplicated Variables ('File Global') -----------------------

// Flag set when system starts or mode changes
static uint32_t New_MoSt_g = FALSE;
static uint32_t New_MoSt_ig = TRUE;   

/*----------------------------------------------------------------------------*-

  PREDICTTORi_Init()

  Set up PredicTTor mechanism.

  PARAMETERS:
     None.

  LONG-TERM DATA:
     New_MoSt_g, New_MoSt_ig;   
       
  MCU HARDWARE:
     None.

  PRE-CONDITION CHECKS:
     None.

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     None.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void PREDICTTORi_Init(void)
   {
   // Set flag to indicate Mode / State change
   New_MoSt_g = TRUE;
   New_MoSt_ig = FALSE;   
   }

/*----------------------------------------------------------------------------*-

  PREDICTTORi_Check_Task_Sequence()

  Performs the checking.

  PARAMETERS:
     TASK_ID - ID of task that is about to be released.

  LONG-TERM DATA:
     New_MoSt_g, New_MoSt_ig;   
       
  MCU HARDWARE:
      None.

  PRE-CONDITION CHECKS:
     Check Duplicated Variables.
     
     If any PreC check fails, we force a shutdown
     [other behaviour may be more appropriate in your system.]

  POST-CONDITION CHECKS:
     None.

  ERROR DETECTION / ERROR HANDLING:
     See PreCs.

  RETURN VALUE:
     None.

-*----------------------------------------------------------------------------*/
void PREDICTTORi_Check_Task_Sequence(const uint32_t TASK_ID)
   {
   // Current position in active tick list
   static uint32_t Tick_list_index_s = 0;
   static uint32_t Tick_list_index_is = ~0;

   // Pointer to the active tick list
   static uint8_t* Tick_list_ptr_s = 0;
   static uint32_t Tick_list_ptr_is = ~0;

   // Length of current tick list
   static uint32_t Length_of_tick_list_s = 0;
   static uint32_t Length_of_tick_list_is = ~0;

   // Length of init period (for async task sets)
   static uint32_t Length_TSIP_s = 0;
   static uint32_t Length_TSIP_is = ~0;

   uint32_t MoSt = PROCESSOR_Get_MoSt();

   // Check pre-conditions (START)

   // Failure of any PreC will force a shutdown in this design
   // Other behaviour may be more appropriate in your system

   // Check data integrity (file-global DV)
   if (New_MoSt_ig != ~New_MoSt_g)
      {
      PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
      }

   // Check data integrity (local DVs)
   if ((Tick_list_index_is != ~Tick_list_index_s) ||
       (Tick_list_ptr_is != ~((uint32_t) Tick_list_ptr_s)) ||
       (Length_of_tick_list_is != ~Length_of_tick_list_s) ||
       (Length_TSIP_s != ~Length_TSIP_is))
      {
      PROCESSOR_Perform_Safe_Shutdown(PFC_LONG_TERM_DATA_CORRUPTION);
      }

   // Check pre-conditions (END)

   // PreCs - incl check MoSt

   switch(MoSt)
      {
      default:
      case FAIL_SAFE_S:
      case STARTUP_01_PROCESSOR_MEMORY_CHECKS_S:
      case STARTUP_02_SOFTWARE_CONFIGURATION_CHECKS_S:
      case STARTUP_03_ENVIRONMENT_CHECKS_S:
      case STARTUP_04_WDT_CHECK_S:
      case STARTUP_05a_SCHEDULER_OSC_CHECK_M:
      case STARTUP_05b_SCHEDULER_OSC_CHECK_M:
      case STARTUP_06a_MONITTOR_ORUN_CHECK_M:
      case STARTUP_06b_MONITTOR_URUN_CHECK_M:
         {
         // Nothing to do - PredicTTor not used
         break;
         }

      case STARTUP_07_PREDICTTOR_CHECK_M:
      case NORMAL_M:
         {
         if (New_MoSt_g == TRUE)
            {
            // Select an appropriate Tick List
            if (MoSt == STARTUP_07_PREDICTTOR_CHECK_M)
               {
               // We use the 'Normal' Tick List to check the PredicTTor in this example
               // => adapt as required to match the needs of your application 
               Tick_list_ptr_s = &TICK_LIST_NORMAL[0];
               Tick_list_ptr_is = ~((uint32_t) Tick_list_ptr_s);

               Length_of_tick_list_s = LENGTH_OF_TICK_LIST_TASKS_NORMAL;
               Length_of_tick_list_is = ~Length_of_tick_list_s;

               Length_TSIP_s = LENGTH_OF_TSIP_TASKS_NORMAL;
               Length_TSIP_is = ~Length_TSIP_s;
               }
            else if (MoSt == NORMAL_M)
               {
               Tick_list_ptr_s = &TICK_LIST_NORMAL[0];
               Tick_list_ptr_is = ~((uint32_t) Tick_list_ptr_s);

               Length_of_tick_list_s = LENGTH_OF_TICK_LIST_TASKS_NORMAL;
               Length_of_tick_list_is = ~Length_of_tick_list_s;

               Length_TSIP_s = LENGTH_OF_TSIP_TASKS_NORMAL;
               Length_TSIP_is = ~Length_TSIP_s;
               }
            }

         // Are we in the TSIP?
         if ((New_MoSt_g == TRUE) && (Tick_list_index_s < Length_TSIP_s))
            {
            // Here we simply ignore data during the TSIP
            Tick_list_index_s++;
            }
         else
            {
            if (New_MoSt_g == TRUE)
               {
               // Completed TSIP (or this is a sync task set)
               New_MoSt_g = FALSE;
               New_MoSt_ig = TRUE;

               // Reset the index
               Tick_list_index_s = 0;
               }
            }

         // Have we completed the TSIP (if there is one)?
         if (New_MoSt_g == FALSE)
            {
            // Check the next task ID ...
            if (Tick_list_ptr_s[Tick_list_index_s] != TASK_ID)
               {
               // Task doesn't match the stored sequence ...

               // Check the current mode
               if (MoSt == STARTUP_07_PREDICTTOR_CHECK_M)
                  {
                  // This is expected behaviour in this mode (run-time test)
                  PROCESSOR_Store_Reqd_MoSt(NORMAL_M);  
                  PROCESSOR_Store_PFC(PFC_NO_FAULT);
                  PROCESSOR_Change_MoSt();
                  }
               else
                  {
                  // This is *not* expected behaviour
                  PROCESSOR_Perform_Safe_Shutdown(PFC_PREDICTTOR_TASK_SEQ_FAULT_TEST);
                  }
               }

            // Sequence is correct - update PredicTTor state
            if (++Tick_list_index_s == Length_of_tick_list_s)
               {
               Tick_list_index_s = 0;
               }
            }

         // Update inverted copy
         Tick_list_index_is = ~Tick_list_index_s;
         }
      }
   }

/*----------------------------------------------------------------------------*-
  ------------------------------ END OF FILE ---------------------------------
-*----------------------------------------------------------------------------*/
