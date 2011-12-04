Welcome to the Hieroglyph 3 Rendering Engine!

Introduction:
-------------

This library has been written by Jason Zink with the intention of promoting the use of Direct3D 11, and at the same time having a common framework with which to make demo programs and quickly prototype new algorithms.

The library itself is written in C++ and utilizes DirectX 11 and Lua.  To run the demo programs, just open the Hieroglyph3.sln file, build the solution, then run the desired demo.  There are a number of demo programs that are intended to be demonstrative of how to use the engine, as well as a growing number of more advanced demos.  

The current status of the engine is pre-alpha, but is becoming much more stable.   

If you have any comments, bugs to report, or suggestions, please feel free to contact me on the gamedev.net private message system (my ID is 'Jason Z'), post a comment in my gamedev.net journal, or start a discussion on the codeplex discussion tabs.  I hope you enjoy using the library as much as I have enjoyed writing it!

Acknowledgements:
-----------------

I have been a long-time fan of Dave Eberly, and his WildMagic engine.  Several of the math classes in Hieroglyph are based on his implementations, and his vast engine provides a wealth of knowledge.  If you haven't seen it yet, go to his website and see what you think: www.geometrictools.com

I also use Lua, which is a fanstastic scripting language for use with C/C++.  If you haven't used it before, take the time to look around here: www.lua.org.

Hieroglyph 3 is also starting to get contributions from some of its users.  I would like to list the names of the contributors here as a small token of appreciation for their help, and also to provide their contact information if they choose to list it (if you have contributed and would like to be added to the list, please let me know!):

Matt Pettineo
Jack Hoxley
Francois Piette, (francois.piette@overbyte.be, www.overbyte.be)


Building Instructions:
----------------------

Hieroglyph requires the following items to be available to the linker/compiler:

1. DirectX SDK - Hieroglyph is currently being developed and tested with the June 2010 SDK.
2. Lua - The Lua libraries must be linked in.  I have included a copy of the VS2008 project I use to build Lua in the distribution, along with the source code for Lua v5.1.4.  If you already build Lua on your own, you can change the to your own libraries.  You will need to modify the post build rules to copy the appropriate libs and headers to the SDK output folder if you do so.

Outside of these dependancies, you should also install the latest windows SDK for your target platform.