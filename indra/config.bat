@echo
set lib=%lib%
set include=%include%
python develop.py -G vc100 -tRelease configure -DLAA:BOOL=ON -DUSE_PRECOMPILED_HEADERS:BOOL=ON -DLL_TESTS=OFF -DPACKAGE:BOOL=OFF  2>&1 |c:\cygwin\bin\tee Config.log