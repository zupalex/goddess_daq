#ifndef DETECTOR_H
#define DETECTOR_H

#include "TObject.h"

///Abstract class defining a detector.
class Detector : public TObject {
	private:
	public:
		///Default constructor.
		Detector();
		///Default destructor.
		virtual ~Detector();

		///Set the raw energy of the channel.
		virtual void SetRawValue(unsigned int detectorChannel, int rawValue) = 0;

	/// \cond This is just for ROOT and doesn't need to be documented
	ClassDef(Detector,1)
	/// \endcond
};

#endif
