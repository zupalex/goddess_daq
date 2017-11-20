# goddess_daq
GODDESS DAQ

This software suite requires cmake (version 2.8 minimum) and the ROOT libraries (https://root.cern.ch/)

After cloning the repository, navigate to the newly created folder and do the following:

```
mkdir build
cd build
cmake ..
make install
```

Once the compilation is done, you should add the following lines in your .rootlogon.C to automatically load the goddess_daq libraries when starting ROOT.

```
{
  gROOT->ProcessLine(".L <path/to/goddess_daq>/scripts/goddess_analysis_macro.cxx");  
  gROOT->ProcessLine("goddess_analysis_macro(\"<path/to/goddess_daq>\")");  
}
```

Simply replace <path/to/goddess_daq> to the actual path to the cloned repository.
