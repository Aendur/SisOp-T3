CFLAGS=/Wall /W4 /EHsc /std:c++17 /Ih /utf-8 /wd5045 /nologo

FGLIBN=random_file utility
DXLIBN=disk_explorer fat32 entry

FGLIBS=$(patsubst %,obj\\%.obj,$(FGLIBN))
DXLIBS=$(patsubst %,obj\\%.obj,$(DXLIBN))

all: filegen

filegen: src\main.cpp $(FGLIBS)
	cl $(CFLAGS) /DFILEGEN /Fo:obj\ /Fe:filegen.exe $?

diskexp: src\main.cpp $(DXLIBS)
	cl $(CFLAGS) /DDISKEXP /Fo:obj\ /Fe:diskexp.exe $?

{src}.cpp{obj}.obj::
	cl $(CFLAGS) /c /Fo:obj\ $<

.PHONY: clean
clean:
	del "obj\*.obj" /q


