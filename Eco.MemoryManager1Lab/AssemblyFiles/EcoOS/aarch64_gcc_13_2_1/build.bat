call make clean -f Makefile TARGET=0 DEBUG=0
call make -f Makefile TARGET=0 DEBUG=0
call make clean -f Makefile TARGET=0 DEBUG=1
call make -f Makefile TARGET=0 DEBUG=1
call make clean -f Makefile TARGET=1 DEBUG=0
call make -f Makefile TARGET=1 DEBUG=0
call make clean -f Makefile TARGET=1 DEBUG=1
call make -f Makefile TARGET=1 DEBUG=1

rem xcopy /y ..\..\..\BuildFiles\EcoOS\arm64-v8a\StaticRelease\lib81589BFED0B84B1194524BEE623E1838.a %ECO_FRAMEWORK%\Eco.MemoryManager1Lab\BuildFiles\EcoOS\arm64-v8a\StaticRelease\
rem xcopy /y ..\..\..\BuildFiles\EcoOS\arm64-v8a\DynamicRelease\81589BFED0B84B1194524BEE623E1838.so %ECO_FRAMEWORK%\Eco.MemoryManager1Lab\BuildFiles\EcoOS\arm64-v8a\DynamicRelease\
