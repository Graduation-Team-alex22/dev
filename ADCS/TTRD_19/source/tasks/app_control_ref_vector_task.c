#include "../main/main.h"
#include "../adcs/adcs_controller/ref_vectors.h"
#include "app_Control_ref_vector_task.h"
#include "../main/project.h"

#ifdef DIAGNOSIS_OUTPUT
#include "stdio.h"
#include "../tasks/ttrd2-05a-t0401a-v001a_uart2_buff_o_task.h"
#endif

void App_Control_Ref_Vector_Init(void)
{
   #ifdef DIAGNOSIS_OUTPUT
      UART2_BUF_O_Write_String_To_Buffer("[DIAG - INIT] Ref Vector Init\n");
      UART2_BUF_O_Send_All_Data();
   #endif
}

uint32_t App_Control_Ref_Vector_Update(void)
{
   CTRL_Ref_Geomag_Update();
   CTRL_Ref_Sun_Update();

   #ifdef DIAGNOSIS_OUTPUT
      char buff[400];
      ref_vectors_t ref_vector = CTRL_Ref_Vectors_GetData();
      sprintf(buff, "SUN: %f %f %f\nMAG: %f %f %f\n",
                     ref_vector.sun_vec.sun_pos_eci.x,
                     ref_vector.sun_vec.sun_pos_eci.y,
                     ref_vector.sun_vec.sun_pos_eci.z,
                     ref_vector.geomag_vec.Xm,
                     ref_vector.geomag_vec.Ym,
                     ref_vector.geomag_vec.Zm);
      UART2_BUF_O_Write_String_To_Buffer("[DIAG - Update] Reference Vectors Update\n");
      UART2_BUF_O_Write_String_To_Buffer("[DIAG - Data]: \n");
      UART2_BUF_O_Write_String_To_Buffer(buff);
      UART2_BUF_O_Send_All_Data();
   #endif
   
   return RETURN_NORMAL_STATE;
}
