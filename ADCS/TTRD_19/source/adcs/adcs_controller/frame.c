#include "frame.h"
#include "math.h"

#include "../services_utilities/common.h"

xyz_t CTRL_LLA2ECEF(const llh_t *source)
{
   xyz_t ecef;
   
   // we approximate to a perfect sphere
   ecef.x = source->alt * cos(RAD(source->lat)) * cos(RAD(source->lon));
   ecef.y = source->alt * cos(RAD(source->lat)) * sin(RAD(source->lon));
   ecef.z = source->alt * sin(RAD(source->lat));
   
   return ecef;
}
