#ifndef ORRUBA_H
#define ORRUBA_H

#include "TObject.h"

class ORRUBA : public TObject {
	private:
		///Energy loss in first layer, may be zero if only one layer.
		float dE;
		///Energy deposited in second layer.
		float E1;
		///Energy deposited in third layer, may be zero if only two layers.
		float E2;

		///The horizontal position from the target.
		float x;
		///The vertical position from the target.
		float y;
		///The height z from the target position along the beam axis.
		float z;

		///Radial dimension in cylindrical coordinates.
		float rho;
		///The azimuthal angle originating from the upward direction.
		float azi;

		///Radial direction in spherical coordinates.
		float r;
		///The polar angle starting from the beam direction.
		float pol;

		///A string specifying which quadrant the position occurred.
		std::string quadrant;
	public:
		ORRUBA();

	ClassDefNV(ORRUBA,1)
};

#endif
