#ifndef SOLIDVECTOR_H
#define SOLIDVECTOR_H

#include "TVector3.h"

class SolidVector : public TVector3 {
	private:
		double rotZ;
		double rotPhi;
	public:
		SolidVector(double x=0, double y=0, double z=0, double rotZ = 0, double rotPhi = 0);

		///Returns the rotation angle around the z-axis.
		double RotZ() const {return rotZ;};
		///Returns the rotation angle around the phi-axis.
		double RotPhi() const {return rotPhi;};

		///Set the rotation angle around the z-axis.
		void SetRotationZ(double angle) {rotZ = angle;};
		///Set the rotation angle around the phi axis.
		void SetRotationPhi(double angle) {rotPhi = angle;};
	

};

#endif
