#include "../main/main.h"
#include "../adcs/adcs_controller/ref_vectors.h"
#include "app_Control_ref_vector_task.h"

void App_Control_Ref_Vector_Init(void)
{

}

uint32_t App_Control_Ref_Vector_Update(void)
{
   CTRL_Ref_Geomag_Update();
   CTRL_Ref_Sun_Update();

   return RETURN_NORMAL_STATE;
}
