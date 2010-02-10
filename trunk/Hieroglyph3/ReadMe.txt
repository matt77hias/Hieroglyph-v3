Welcome to the Hieroglyph 3 Rendering Engine!

Introduction:

This library has been written by Jason Zink with the intention of promoting the use of Direct3D 11, and at the same time having a common framework with which to make demo programs and quickly prototype new algorithms.

The library itself is written in C++ and utilizes DirectX 11 and Lua.  To run the demo programs, just open the Hieroglyph3.sln file, build the solution, then run the desired demo.  Currently there is only a single simple demo program that doesn't render anything, but you have to start out somewhere.

The current status of the engine is pre-alpha.  I am in the process of building the rendering system around the D3D11 renderer.  On the rendering side, most API functionality is currently supported and can be used to manually perform rendering.  The entire object system has been copied into the project, but need significant work to get operational.  This will be the target of the next release - to have the object system ready for use in the demo programs.  In addition, the next release will have some support for my RenderView/Material system.

Also included in this initial release is a basic event system and a basic scripting system.  Further development will come for these systems as well as time becomes available.  

If you have any comments, bugs to report, or suggestions, please feel free to contact me on the gamedev.net private message system (my ID is 'Jason Z'), or post a comment in my gamedev.net journal.  I hope you enjoy using the library as much as I have enjoyed writing it!

Acknowledgements:

I have been a long-time fan of Dave Eberly, and his WildMagic engine.  Several of the math classes in Hieroglyph are based on his implementations, and his vast engine provides a wealth of knowledge.  If you haven't seen it yet, go to his website and see what you think: www.geometrictools.com

I also use Lua, which is a fanstastic scripting language for use with C/C++.  If you haven't used it before, take the time to look around here: www.lua.org
