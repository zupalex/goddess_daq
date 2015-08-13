#include "SolidVector.h"

SolidVector::SolidVector(double x, double y, double z, double rotZ /*=0*/, double rot_Phi /*=0*/) :
	TVector3(x,y,z), rotZ(rotZ), rotPhi(rot_Phi)
{

}

