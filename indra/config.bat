@echo
set lib=%lib%
set include=%include%
develop.py -G vc100 -tRelease configure -DUSE_PRECOMPILED_HEADERS:BOOL=ON -DLL_TESTS=OFF -DPACKAGE:BOOL=TRUE  2>&1 |c:\cygwin\bin\tee Config.log