# Hieroglyph 3 Rendering Engine

## Introduction

This library has been written by Jason Zink with the intention of promoting the use of Direct3D 11, and at the same time having a common framework with which to make demo programs and quickly prototype new algorithms.

The library itself is written in C++ and utilizes DirectX 11, Lua, and the DirectXTK library.  It has also added support for the Kinect sensor as input, and recently added support for the Oculus Rift (Kinect and Rift support are both optional).  The library has been upgraded to utilize Visual Studio 2012, as well as the Windows 8 SDK.  This configuration allows building for Desktop applications in Windows 7 and Windows 8.

To run the demo programs, just open the Hieroglyph3_Desktop.sln file, build the solution, then run the desired demo.  There are a number of demo programs that are intended to be demonstrative of how to use the engine, as well as a growing number of more advanced demos.  If you want to try out the Kinect based samples, open the Hieroglyph3_Desktop_Kinect.sln file.  This solution will contain all of the pre-requisites for the libraries and samples which use the Kinect.  For the Oculus Rift based samples, please open the Hieroglyph3_desktop_Rift.sln file.

## Building Instructions

Hieroglyph requires the use of VS2012 for building.  In addition, the following items need to be available to the linker/compiler:

1. Windows 8 SDK - Hieroglyph is currently being developed and tested with the Windows 8 SDK.  This is automatically installed with all versions of the VS2012, including the express edition.  Please install VS2012 - even if you need to keep the older compilers installed, the updated functionality is worth upgrading!
2. Lua - The Lua libraries must be linked in.  I have included a copy of the VS2012 project I use to build Lua in the distribution, along with the source code for Lua v5.1.4.  Please note the license for using Lua and act accordingly!  If you already build Lua on your own, you can change the to your own libraries.  You will need to modify the post build rules to copy the appropriate libs and headers to the SDK output folder if you do so.
3. DirectXTK - I have included a copy of the DirectXTK source code in the source distribution.  Please note the license for using this library and act accordingly!  Upgrading to new versions of this library should be very easy, and just requires copying the source code into the Dependencies\DirectXTK folder.
4. Kinect SDK - If you want to use the Kinect sensor as input, then you need to install the latest Kinect for Windows SDK.  Please follow the instructions on the Kinect SDK website, and everything else should be ready to go.  Please note that this requires Windows 7 or 8 to compile the Kinect samples (sorry Vista users)!
5. Oculus Rift SDK - If you want to build the samples for the Oculus Rift, you must download and install the OVR SDK from Oculus, and extract it to the Dependencies folder.  I have tested it with OculusSDK v0.4.2, and will update to the latest SDK versions as they come out.

With these dependencies taken care of, you should be able to easily build the projects and try out the sample programs.  In addition, both x86 and x64 build targets are now available.
If you encounter issues or problems, please post an issue in the Hieroglyph 3 codeplex site!
