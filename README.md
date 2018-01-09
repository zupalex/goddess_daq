This software suite requires cmake (version 2.8 minimum) and the ROOT libraries (https://root.cern.ch/)

The dodumentation of the source code can [be found here](https://codedocs.xyz/zupalex/goddess_daq/index.html) (generated using [Doxygen](http://www.stack.nl/~dimitri/doxygen/) and [CodeDocs,xyz](https://codedocs.xyz/))

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
Simply replace <path/to/goddess_daq> with the actual path to the cloned repository.  
**Replace only the part enclosed in <> (including the < and > sign) while leaving the rest as is.  
Escaping the quotes properly in the second line is mandatory.**
