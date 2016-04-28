REM This script must be run from the VS developers command prompt!

msbuild Hieroglyph3_Desktop.sln /t:Clean /p:Configuration=Debug /m:4 /p:Platform=Win32
msbuild Hieroglyph3_Desktop.sln /t:Clean /p:Configuration=Release /m:4 /p:Platform=Win32
msbuild Hieroglyph3_Desktop.sln /t:Clean /p:Configuration=Debug /m:4 /p:Platform=x64
msbuild Hieroglyph3_Desktop.sln /t:Clean /p:Configuration=Release /m:4 /p:Platform=x64

msbuild Hieroglyph3_Desktop_SDK.sln /t:Clean /p:Configuration=Debug /m:4 /p:Platform=Win32
msbuild Hieroglyph3_Desktop_SDK.sln /t:Clean /p:Configuration=Release /m:4 /p:Platform=Win32
msbuild Hieroglyph3_Desktop_SDK.sln /t:Clean /p:Configuration=Debug /m:4 /p:Platform=x64
msbuild Hieroglyph3_Desktop_SDK.sln /t:Clean /p:Configuration=Release /m:4 /p:Platform=x64

msbuild Hieroglyph3_Desktop_Kinect.sln /t:Clean /p:Configuration=Debug /m:4 /p:Platform=Win32
msbuild Hieroglyph3_Desktop_Kinect.sln /t:Clean /p:Configuration=Release /m:4 /p:Platform=Win32
msbuild Hieroglyph3_Desktop_Kinect.sln /t:Clean /p:Configuration=Debug /m:4 /p:Platform=x64
msbuild Hieroglyph3_Desktop_Kinect.sln /t:Clean /p:Configuration=Release /m:4 /p:Platform=x64

msbuild Hieroglyph3_Desktop_MFC.sln /t:Clean /p:Configuration=Debug /m:4 /p:Platform=Win32
msbuild Hieroglyph3_Desktop_MFC.sln /t:Clean /p:Configuration=Release /m:4 /p:Platform=Win32
msbuild Hieroglyph3_Desktop_MFC.sln /t:Clean /p:Configuration=Debug /m:4 /p:Platform=x64
msbuild Hieroglyph3_Desktop_MFC.sln /t:Clean /p:Configuration=Release /m:4 /p:Platform=x64

msbuild Hieroglyph3_Desktop_Rift.sln /t:Clean /p:Configuration=Debug /m:4 /p:Platform=Win32
msbuild Hieroglyph3_Desktop_Rift.sln /t:Clean /p:Configuration=Release /m:4 /p:Platform=Win32
msbuild Hieroglyph3_Desktop_Rift.sln /t:Clean /p:Configuration=Debug /m:4 /p:Platform=x64
msbuild Hieroglyph3_Desktop_Rift.sln /t:Clean /p:Configuration=Release /m:4 /p:Platform=x64

msbuild Hieroglyph3_Desktop_WPF.sln /t:Clean /p:Configuration=Debug /m:4 /p:Platform=Win32
msbuild Hieroglyph3_Desktop_WPF.sln /t:Clean /p:Configuration=Release /m:4 /p:Platform=Win32
msbuild Hieroglyph3_Desktop_WPF.sln /t:Clean /p:Configuration=Debug /m:4 /p:Platform=x64
msbuild Hieroglyph3_Desktop_WPF.sln /t:Clean /p:Configuration=Release /m:4 /p:Platform=x64

pause