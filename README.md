# CppTSL

C++ technical static library

## Compiling

1. Windows

	```
	<cmake-directory>\cmake ../CppTSL/CSL/CSL-r -G "Visual Studio 16" -A x64
	MSBuild ALL_BUILD.vcxproj /t:build /p:Configuration=Debug
	```

	```
	<cmake-directory>\cmake ../CppTSL/CSL/CSL-r -G "Visual Studio 16" -A x64
	MSBuild ALL_BUILD.vcxproj /t:build /p:Configuration=Release
	```

1. Linux

	```
	<cmake-directory>/cmake ../CppTSL/CSL/CSL-r -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles"
	make
	```

	```
	<cmake-directory>/cmake ../CppTSL/CSL/CSL-r -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles"
	make
	```
