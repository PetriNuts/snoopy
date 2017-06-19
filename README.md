Snoopy
===============================================================================



 Needed Software / Libraries
-------------------------------------------------------------------------------

 * [Visual Studio Express](http://visualstudio.com/products/visual-studio-express-vs) 
 * [XCode](http://developer.apple.com/tools/xcode/)
 * [CLion](https://www.jetbrains.com/clion/)
 * [Eclipse C/C++ Development Tooling - CDT](http://www.eclipse.org/cdt/)
 * [CMake](http://cmake.org)
 * [Snoopy](https://github.com/PetriNuts/snoopy)
 * [SPSA_SteeringAPI](https://github.com/PetriNuts/SPSA_SteeringAPI)
 * [Snoopy simulator](https://github.com/PetriNuts/SP_Simulator)
 * [dssd_util](https://github.com/PetriNuts/dssd_util)
 * [wxWidgets](http://www.wxwidgets.org/)
 * [ogl](https://github.com/PetriNuts/ogl)
 * [freechart](https://github.com/PetriNuts/freechart)
 * [OGDF](http://www.ogdf.net/)
 * [SBML](http://sbml.org/)
 * [IrrKlang](http://www.ambiera.com/irrklang/)
 * [SUNDIALS](https://computation.llnl.gov/projects/sundials/sundials-software)
 * [Gecode](http://www.gecode.org)
 * [Boost](http://www.boost.org/)
 * [Flex & Bison](http://sourceforge.net/projects/winflexbison/)
 * [GMP](http://www.gmplib.org)
 * [MPIR](http://www.mpir.org) drop-in replacement for gmp

Setting up the development environment
===============================================================================

*Expected duration: about one day*

Windows
-------------------------------------------------------------------------------

### Microsoft Visual Studio 2015 Installer Projects

[link](https://visualstudiogallery.msdn.microsoft.com/f1cc3f3e-c300-40a7-8797-c509fb8933b9)

### Compilation Guide

*This is work in Progress!!!*

1. needed environment variables  
	`$SPSA_STEERINGAPI = path of SPSA_SteeringAPI`  
	`$SP_SIMULATOR = path of SP_Simulator`  
	`$DSSD_UTIL = path of dssd_util library`
	`$WXWIN = path of wxWidgets`  
	`$OGL = path of ogl`  
	`$FREECHART = path of freechart`  
	`$BOOST_ROOT = path of boost`  
	`$OGDF = path of OGDF`  
	`$XERCESCROOT = path of xerces`  
	`$LIBSBML = path of SBML`  
	`$IRRKLANG = path of IrrKlang`  
	`$CVODE = path of sundials library`  
	`$GECODEDIR = path of gecode library`  
	`$GMPDIR = path of gmp/mpir library`

1. wxWidgets
	1. download wxWidgets 1.* from [wxWidgets Homepage](http://www.wxwidgets.org/) and unpack it into "~/workspace"
	1. modify: ($WXWIN)\include\wx\msw\setup.h  
	`#define wxUSE_STL 1`  
	`#define wxUSE_UNICODE 1`  
	`#define wxUSE_POSTSCRIPT 1`  
	`#define wxUSE_GRAPHICS_CONTEXT 1`
	1. for compiling wxWidgets as 64-bit check [link](http://wiki.wxwidgets.org/Supporting_x64_and_Win32_within_one_solution)
	1. open `($WXWIN)\build\msw\wx_vc14.sln`
	1. build solution for x64 as *Debug DLL* and *Release DLL*

1. OGL 
	1. get ogl  
	`git clone https://github.com/PetriNuts/ogl.git`
	1. open `($OGL)\build\ogl_vc14.sln` 
	1. build *ogl* for x64 as *Debug* and *Release*

1. freechart 
	1. get freechart  
	`git clone https://github.com/PetriNuts/freechart.git`
	1. open `($FREECHART)\build\wxfreechart_vc14.sln` 
	1. build *freechart* for x64 as *Debug* and *Release*

1. LibSBML
	1. download [Xerces-c](http://xml.apache.org/xerces-c/) and unpack it into "~/workspace"
	1. build *xerceslib* for x64  as *Release* und *Debug*
	1. download [LibSBML](http://sbml.org/Software/libSBML/Downloading_libSBML) and unpack it into "~/workspace"
	1. create a folder *build* 
	1. use cmake to create visual studio project files
	1. open `($LIBSBML)\build\libsbml.sln` 
	1. build *ALL_BUILD* for x64  as *Release* und *Debug*

1. OGDF
	1. download [OGDF](http://ogdf.net/doku.php/tech:download) and unpack it into "~/workspace"
	1. create a folder *build* 
	1. use cmake to create visual studio project files
	1. open `($OGDF)\build\ogdf.sln` 
	1. use cmake to create visual studio project files
	1. build *ALL_BUILD* for x64  as *Release* und *Debug*

1. Irrklang
    1. download [Irrklang](http://www.ambiera.com/irrklang/downloads.html) and unpack it into "~/workspace"

1. Boost
	1. download [boost](http://www.boost.org/users/download/) and unpack it into "~/workspace"
	1. open command line at $BOOST_ROOT
	1. call `bjam`
	1. call `b2 threading=multi architecture=x86 address-model=64 link=static`

1. MPIR
	1. download [MPIR](http://mpir.org/downloads.html) and unpack it into "~/workspace"
	1. open `($GMPDIR)\build.vc14\mpir.sln` 
	1. build *lib_mpir_core2* for x64 as *Debug* and *Release*

1. Sundials
	1. download [sundials](https://computation.llnl.gov/projects/sundials/sundials-software) and unpack it into "~/workspace"
	1. create a folder *build* 
	1. use cmake to create visual studio project files
	1. open `($CVODE)\build\sundials.sln` 
	1. build *ALL_BUILD* for x64  as *Release* und *Debug*

1. Gecode
	1. download [Gecode](http://www.gecode.org/download/gecode-4.4.0.tar.gz) and unpack it into "~/workspace"
	1. create a folder *build* 
	1. use cmake to create visual studio project files
	1. open `($GECODEDIR)\build\gecode.sln` 
	1. build *ALL_BUILD* for x64  as *Release* und *Debug*

1. dssd_util
	1. get dssd_util  
	`git clone https://github.com/PetriNuts/dssd_util.git`
	1. create a folder *build* 
	1. use cmake to create visual studio project files
	1. open `($DSSD_UTIL)\build\dssd_util.sln`
	1. build *ALL_BUILD* for x64  as *Release* und *Debug*

1. SPSA_SteeringAPI
	1. get SPSA_SteeringAPI  
	`git clone https://github.com/PetriNuts/SPSA_SteeringAPI.git`
	1. open `($SPSA_STEERINGAPI)\SPSA_STEERINGAPI.sln` 
	1. build *solution* for x64 as *Debug* and *Release*

1. SP_Simulator
	1. get SP_Simulator  
	`git clone https://github.com/PetriNuts/SP_Simulator.git`
	1. open `($SP_Simulator)\Snoopy Simulator.sln` 
	1. build *solution *for x64 as *Debug* and *Release*

1. Snoopy
	1. get Snoopy  
	`git clone https://github.com/PetriNuts/snoopy.git`
	1. open `($Snoopy)\SnoopyVC.sln` 
	1. build *solution* for x64 as *Debug* and *Release*
	1. To use dynamic link(DLL), you can either copy the DLLs to Snoopy folder or as a more general way add the path of the Dlls to your windows PATH in the environment variable e.g (PATH=D:\workspace\Steering\Snoopy Simulator\lib\lib_win\debug\DLL) so that you do not need to copy the DLLs each time they are changed. Do not forget to add both the debug and the release paths


Mac OSX
-------------------------------------------------------------------------------

### Compilation Guide

*This is work in Progress!!!*

Build-Guide for Snoopy to compile under Mac OS X

1.  download and install xcode, if you not already have

1.  download and install eclipse, and select as your workspace "~/workspace", where "~" means your home directory
	-  If incremental build is not working anymore, check out this [webpage](http://mcuoneclipse.com/2015/06/06/eclipse-project-refresh-policy-broken-incremental-build-with-external-make/).

1.  download and install clion, and select as your workspace "~/workspace", where "~" means your home directory

1. wxWidgets
	1. download wxWidgets 1.* from [wxWidgets Homepage](http://www.wxwidgets.org/) and unpack it into "~/workspace"
	1. create a folder *release*
	1. go into *release* and run  
	`../configure --disable-debug --enable-unicode --enable-shared --disable-static --enable-graphics_ctx --enable-std_iostreams --enable-std_string --enable-std_string_conv_in_wxstring --with-osx_cocoa --without-libjbig --without-liblzma --enable-macosx_arch=x86_64 --with-cxx=11 --disable-precomp-headers CC=cc CXX="c++ -std=c++11 -stdlib=libc++" LINK=cc LDFLAGS="-stdlib=libc++ -Wl,-install_name,@loader_path"`  
	`make && sudo make install`
	1. if you get an error during the build, check the following [site](http://goharsha.com/blog/compiling-wxwidgets-3-0-2-mac-os-x-yosemite/)

1. OGL 
	1. get ogl  
	`git clone https://github.com/PetriNuts/ogl.git`
	1. create a folder *release* 
	1. go into *release* and run  
	`../configure CC=cc CXX="c++ -std=c++11 -stdlib=libc++" LINK=cc LDFLAGS=-stdlib=libc++`  
	`make && sudo make install`

1. freechart 
	1. get freechart  
	`git clone https://github.com/PetriNuts/freechart.git`
	1. create a folder *release* 
	1. go into *release* and run  
	`../configure CC=cc CXX="c++ -std=c++11 -stdlib=libc++" LINK=cc LDFLAGS=-stdlib=libc++`  
	`make && sudo make install`

1. LibSBML
	1. download [LibSBML](http://sbml.org/Software/libSBML/Downloading_libSBML) and unpack it into "~/workspace"
	1. create a folder *build* 
	1. go into *build* and run  
	`cmake ..`  
	`make && sudo make install`

1. OGDF
	1. download [OGDF](http://ogdf.net/doku.php/tech:download) and unpack it into "~/workspace"
	1. create a folder *build* 
	1. go into *build* and run  
	`cmake ..`  
	`make && sudo make install`

1. Irrklang
	1. download [Irrklang](http://www.ambiera.com/irrklang/downloads.html) and unpack it into "~/workspace"
	1. copy the header files to `/usr/local/include/` and the lib to `/usr/local/lib/`

1. Boost
	1. download [boost](http://www.boost.org/users/download/) and unpack it into "~/workspace"
	1. open command line at $BOOST_ROOT and call  
	`./bootstrap.sh`  
	`./b2 variant=release threading=multi architecture=x86 address-model=64 link=static cxxflags="-std=c++11 -stdlib=libc++" linkflags="-stdlib=libc++" toolset=clang`  
	`sudo ./b2 variant=release threading=multi architecture=x86 address-model=64 link=static cxxflags="-std=c++11 -stdlib=libc++" linkflags="-stdlib=libc++" toolset=clang install`  

1. Sundials
	1. download [sundials](https://computation.llnl.gov/projects/sundials/sundials-software) and unpack it into "~/workspace"
	1. create a folder *build* 
	1. go into *build* and run  
	`cmake ..`  
	`make && sudo make install`

1. MPIR
	1. download [MPIR](http://mpir.org/downloads.html) and unpack it into "~/workspace"
	1. create a folder *release* 
	1. go into *release* and run  
	`../configure CC=cc CXX="c++ -std=c++11" LINK=cc`  
	`make && sudo make install`

1. Gecode
	1. download [Gecode](http://www.gecode.org/download/gecode-4.4.0.tar.gz) and unpack it into "~/workspace"
	1. create a folder *build* 
	1. go into *build* and run  
	`cmake ..`  
	`make && sudo make install`

1. dssd_util
	1. get dssd_util  
	`git clone https://github.com/PetriNuts/dssd_util.git`
	1. create a folder *build* 
	1. go into *build* and run  
	`cmake ..`  
	`make && sudo make install`

1. SPSA_SteeringAPI
	1. get SPSA_SteeringAPI  
	`git clone https://github.com/PetriNuts/SPSA_SteeringAPI.git`
	1. open the project in eclipse 
	1. build for *Debug* and *Release*

1. SP_Simulator
	1. get SP_Simulator  
	`git clone https://github.com/PetriNuts/SP_Simulator.git`
	1. open the project in eclipse 
	1. build for *Debug* and *Release*

1. Snoopy
	1. get Snoopy  
	`git clone https://github.com/PetriNuts/snoopy.git`
	1. open the project in eclipse or clion 
	1. build for *Debug* and *Release*


### Debug Guide
You have got several options:

1. Install GDB with MacPorts or Homebrew, as mentioned [here](http://stackoverflow.com/a/19536455) or [here](http://stackoverflow.com/a/23704065).
1. Use LLDB-MI from LLDB team; note that LLDB-MI does not come with the native LLDB on OS X, meaning you have to build LLDB your own with options to switch on the feature.
1. Use third-party [LLDB-MI2](https://github.com/freedib/lldbmi2) (see the page for reasons why he needs another Machine Interface)

All of these three options require a lot of configuration and are error-prone.

Linux
-------------------------------------------------------------------------------

### Compilation Guide

*This is work in Progress!!!*

1. check whether "/usr/local/lib" is searched by "ldconfig", if not run as root  
	`echo "/usr/local/lib" >> /etc/ld.so.conf.d/libc.conf`  
	`ldconfig`

1. download and install eclipse, and select as your workspace "~/workspace", where "~" means your home directory
	-  If incremental build is not working anymore, check out this [webpage](http://mcuoneclipse.com/2015/06/06/eclipse-project-refresh-policy-broken-incremental-build-with-external-make/).

1. wxWidgets
	1. download wxWidgets 1.* from [wxWidgets Homepage](http://www.wxwidgets.org/) and unpack it into "~/workspace"
	1. create a folder *release*
	1. go into *release* and run  
	`../configure --disable-debug --enable-unicode --enable-shared --disable-static --enable-graphics_ctx --enable-std_iostreams --enable-std_string --enable-std_string_conv_in_wxstring --without-libjbig --without-liblzma --with-cxx=11 --disable-precomp-headers CC=cc CXX=c++ LINK=cc`  
	`make && sudo make install`
	1. if you get an error during the build, check the following [site](http://goharsha.com/blog/compiling-wxwidgets-3-0-2-mac-os-x-yosemite/)

1. OGL 
	1. get ogl  
	`git clone https://github.com/PetriNuts/ogl.git`
	1. create a folder *release* 
	1. go into *release* and run  
	`../configure CC=cc CXX="c++ -std=c++11" LINK=cc`  
	`make && sudo make install`

1. freechart 
	1. get freechart  
	`git clone https://github.com/PetriNuts/freechart.git`
	1. create a folder *release* 
	1. go into *release* and run  
	`../configure CC=cc CXX="c++ -std=c++11" LINK=cc`  
	`make && sudo make install`

1. LibSBML
	1. download [LibSBML](http://sbml.org/Software/libSBML/Downloading_libSBML) and unpack it into "~/workspace"
	1. create a folder *build* 
	1. go into *build* and run  
	`cmake ..`  
	`make && sudo make install`

1. OGDF
	1. download [OGDF](http://ogdf.net/doku.php/tech:download) and unpack it into "~/workspace"
	1. create a folder *build* 
	1. go into *build* and run  
	`cmake ..`  
	`make && sudo make install`

1. Irrklang
	1. download [Irrklang](http://www.ambiera.com/irrklang/downloads.html) and unpack it into "~/workspace"
	1. copy the header files to `/usr/local/include/` and the lib to `/usr/local/lib/`

1. Boost
	1. download [boost](http://www.boost.org/users/download/) and unpack it into "~/workspace"
	1. open command line at $BOOST_ROOT and call  
	`./bootstrap.sh`  
	`./b2 variant=release threading=multi architecture=x86 address-model=64 link=static cxxflags="-std=c++11 -stdlib=libc++" linkflags="-stdlib=libc++" toolset=clang`  
	`sudo ./b2 variant=release threading=multi architecture=x86 address-model=64 link=static cxxflags="-std=c++11 -stdlib=libc++" linkflags="-stdlib=libc++" toolset=clang install`  

1. Sundials
	1. download [sundials](https://computation.llnl.gov/projects/sundials/sundials-software) and unpack it into "~/workspace"
	1. create a folder *build* 
	1. go into *build* and run  
	`cmake ..`  
	`make && sudo make install`

1. MPIR
	1. download [MPIR](http://mpir.org/downloads.html) and unpack it into "~/workspace"
	1. create a folder *release* 
	1. go into *release* and run  
	`../configure CC=cc CXX="c++ -std=c++11" LINK=cc`  
	`make && sudo make install`

1. Gecode
	1. download [Gecode](http://www.gecode.org/download/gecode-4.4.0.tar.gz) and unpack it into "~/workspace"
	1. create a folder *build* 
	1. go into *build* and run  
	`cmake ..`  
	`make && sudo make install`

1. dssd_util
	1. get dssd_util  
	`git clone https://github.com/PetriNuts/dssd_util.git`
	1. create a folder *build* 
	1. go into *build* and run  
	`cmake ..`  
	`make && sudo make install`

1. SPSA_SteeringAPI
	1. get SPSA_SteeringAPI  
	`git clone https://github.com/PetriNuts/SPSA_SteeringAPI.git`
	1. open the project in eclipse 
	1. build for *Debug* and *Release*

1. SP_Simulator
	1. get SP_Simulator  
	`git clone https://github.com/PetriNuts/SP_Simulator.git`
	1. open the project in eclipse 
	1. build for *Debug* and *Release*

1. Snoopy
	1. get Snoopy  
	`git clone https://github.com/PetriNuts/snoopy.git`
	1. open the project in eclipse or clion 
	1. build for *Debug* and *Release*

