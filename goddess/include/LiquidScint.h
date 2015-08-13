#ifndef LIQUIDSCINT_H
#define LIQUIDSCINT_H

#include "Detector.h"

class LiquidScint : public Detector {
	private:
	public:
		LiquidScint();
		virtual ~LiquidScint();
		static const unsigned short numChannels = 3;

		///Return the number of channels of the type specified.
		int GetChannels(bool secondary=false) {return numChannels;};

		///Set the raw energy of the channel.
		virtual void SetRawValue(unsigned int detectorChannel, bool secondaryType, int rawValue);

		void Clear();


	/// \cond This is just for ROOT and doesn't need to be documented
	ClassDef(LiquidScint,1);
	/// \endcond
};

#endif
