#ifndef IONCHAMBER_H
#define IONCHAMBER_H

#include "Detector.h"
#include <vector>

class IonChamber : public Detector
{
private:
    ///The number of anode channels ganged together to make the energy loss signal.
    const int numDE; //!
    ///The number of anode channels ganged together to make the residual energy signal.
    const int numEres; //!

    ///Array of energy in each anode in MeV.
    std::vector<float> anodeRaw;
    ///The energy (slow) signals from the PMT.
    std::vector<float> scintRawE;
    ///The time (fast) signals from the PMT.
    std::vector<float> scintRawT;

    ///Array of energy in each anode in MeV.
    std::vector<float> anodeCal;
    ///The energy (slow) signals from the PMT.
    std::vector<float> scintCalE;
    ///The time (fast) signals from the PMT.
    std::vector<float> scintCalT;

    ///Energy loss in the first anodes in MeV.
    float dE;
    ///Residual energy after the dE layer in MeV.
    float resE;
    ///The total energy dE + resE in MeV.
    float E;

    ///The polynomial calibration parameters for the anode.
    std::vector<std::vector<double>> parAnodeEnCal; //!
    ///The polynomial calibration parameters for the scintillator.
    std::vector<std::vector<double>> parScintEnCal; //!
    ///The polynomial calibration parameters for the scintillator time properties.
    std::vector<std::vector<double>> parScintTimeCal; //!

public:
    ///Default constructor.
    IonChamber(int numAnode, int numScint, int numDE, int numEres);
    ///Default destructor.
    virtual ~IonChamber();

    ///Clear the event.
    void Clear();

    void SetAnodeEnCalPars(int ch, std::vector<double> pars);
    void SetScintEnCalPars(int ch, std::vector<double> pars);
    void SetScintTimeCalPars(int ch, std::vector<double> pars);

    ///Set the raw energy of the contact and compute the calibrated value.
    virtual void SetRawValue(unsigned int channel, bool scintType, unsigned int rawValue, int ignThr);
    ///Set the timestamp for the channel.
    virtual void SetTimeStamp(unsigned int detectorChannel, bool secondaryType, unsigned long long timestamp);

    ///Return the number of channels of the type specified.
    int GetNumChannels(bool scintType);
    ///Return the energy loss total for the anode dE section.
    float GetAnodeDE() {
        return dE;
    };
    ///Return the residual energy loss total for the anode Eres section.
    float GetAnodeResE() {
        return resE;
    };
    ///Return the total energy loss.
    float GetAnodeE() {
        return E;
    };

    ///Check if the specified anode channel has been declared.
    bool ValidAnode(size_t ch);
    ///Check if the specified scintillator channel has been declared.
    bool ValidScint(size_t ch);

    /// \cond This is just for ROOT and doesn't need to be documented
    ClassDef(IonChamber, 1)
    /// \endcond
};

#endif
