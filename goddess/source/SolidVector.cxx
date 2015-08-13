#include "SolidVector.h"

SolidVector::SolidVector(double x, double y, double z, double rot_Z /*=0*/, double rot_Phi /*=0*/) :
	TVector3(x,y,z), rotZ(rot_Z), rotPhi(rot_Phi)
{

}

