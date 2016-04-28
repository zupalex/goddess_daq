#ifndef PNPAIR_H
#define PNPAIR_H

#include <vector>
#include "TObject.h"

///Template support Class to make the sorting easier when storing the data relative to the Silicon Detectors in the root tree
template <class pairType>
class PNPair
{
 private:

 public:
   ///Default constructor.
  PNPair();
  
  ///Default destructor.
  virtual ~PNPair(); 
  
  ///Clear the two vector members of the PNPair class
  void inline Clear();

  std::vector<pairType> inline near();
  std::vector<pairType> inline far();

  ///Vector containing the info concerning the front side. The energies and strip numbers are using this convention.
  std::vector<pairType> p;

  ///Vector containing the info concerning the back side. The energies and strip numbers are using this convention.
  std::vector<pairType> n;

  /// \cond This is just for ROOT and doesn't need to be documented
  ClassDef(PNPair,1)
  /// \endcond
};

template<class pairType> PNPair<pairType>::PNPair(){}

template<class pairType> PNPair<pairType>::~PNPair(){}

template<class pairType> inline void PNPair<pairType>::Clear()
{
  p.clear();
  n.clear();
}

template<class pairType> inline std::vector<pairType> PNPair<pairType>::near() { return p; }
template<class pairType> inline std::vector<pairType> PNPair<pairType>::far() { return n; }

templateClassImp(PNPair)

#endif
