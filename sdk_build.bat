REM This script must be run from the VS developers command prompt!

msbuild Hieroglyph3_Desktop_SDK.sln /t:Rebuild /p:Configuration=Debug /m:4 /p:Platform=Win32
msbuild Hieroglyph3_Desktop_SDK.sln /t:Rebuild /p:Configuration=Release /m:4 /p:Platform=Win32
msbuild Hieroglyph3_Desktop_SDK.sln /t:Rebuild /p:Configuration=Debug /m:4 /p:Platform=x64
msbuild Hieroglyph3_Desktop_SDK.sln /t:Rebuild /p:Configuration=Release /m:4 /p:Platform=x64

pause