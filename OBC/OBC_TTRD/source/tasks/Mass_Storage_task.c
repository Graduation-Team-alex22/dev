#include "Mass_Storage_task.h" 

// ------ Public variables ---------------------------------------------------
tc_tm_pkt *mass_storagepkt =0;
uint8_t mass_storeFLAG = 0;


extern SAT_returnState mass_storage_app(tc_tm_pkt *pkt);

void     Mass_Storage_Init()
{

}

uint32_t Mass_Storage_Update(tc_tm_pkt *mass_storagepkt)
{
	SAT_returnState res;
	if(mass_storeFLAG == 1)
	{
		res = mass_storage_app(mass_storagepkt);
	}
		
	return 0;
}