#include "SolidVector.h"

SolidVector::SolidVector(double x_, double y_, double z_, double rot_Z /*=0*/, double rot_Phi /*=0*/) :
	TVector3(x_,y_,z_), rotZ(rot_Z), rotPhi(rot_Phi)
{

}

