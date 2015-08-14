#ifndef DETECTOR_H
#define DETECTOR_H

#include "TObject.h"

///Abstract class defining a detector.
class Detector : public TObject {
	public:
		static const bool Primary = false;
		static const bool Secondary = true;

	private:
	public:
		///Default constructor.
		Detector();
		///Default destructor.
		virtual ~Detector();

		///Return the number of channels of the type specified.
		virtual int GetNumChannels(bool secondaryType) = 0;

		///Set the raw energy of the channel.
		virtual void SetRawValue(unsigned int detectorChannel, bool secondaryType, unsigned int rawValue) = 0;

		virtual void Clear() = 0;

	/// \cond This is just for ROOT and doesn't need to be documented
	ClassDef(Detector,1)
	/// \endcond
};

#endif
