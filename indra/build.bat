@echo
msbuild build-vc100\Secondlife.sln /t:build /p:Configuration=Release /p:Platform=win32  2>&1 |c:\cygwin\bin\tee Build.log
::msbuild build-vc100\newview\package.vcxproj /t:build /p:Configuration=RelWithDebInfo /p:Platform=win32  /p:"VCBuildAdditionalOptions= /useenv" /p:"VCBuildAdditionalOptions= /incremental" 2>&1 |c:\cygwin\bin\tee Buildpackage.log