
/*-----------------*/
/* user event code */
/* (may be empty.) */
/*-----------------*/

//Assign raw energies to correct detectors
//***The following is handled by the detector classes***
//Iterate contact multiplicity (number of contacts per detector)
//Iterate strips hit per quadrant multiplicity (number of strips per quadrant)
//Iterate quadrant hit multiplicity (number of quadrant firing)
//Push back on quadrant hit container, maybe not a vector?
//Compute strip position on superX3s




//If quadrant hit multiplicity > 1 continue
//else 
//	compute event coordinates
//	compute event energies: dE, E1, E2

orruba->SetEvent("", 0, 0, 0, TVector3(0,0,0));

data.Fill();
