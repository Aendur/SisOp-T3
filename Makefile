#CFLAGS=/Wall /W4 /EHsc /std:c++17 /Ih /utf-8 /wd5045 /nologo
CFLAGS=/W4 /EHsc /std:c++17 /Ih /utf-8 /nologo

FGLIBN=random_file utility
DXLIBN=disk_explorer fat32 entry
TMLIBN=term_ui

FGLIBS=$(patsubst %,obj\\%.obj,$(FGLIBN))
DXLIBS=$(patsubst %,obj\\%.obj,$(DXLIBN))
TMLIBS=$(patsubst %,obj\\%.obj,$(TMLIBN))

all: filegen

filegen: src\main.cpp $(FGLIBS)
	cl $(CFLAGS) /DFILEGEN /Fo:obj\ /Fe:filegen.exe $?

diskexp: src\main.cpp $(DXLIBS)
	cl $(CFLAGS) /DDISKEXP /Fo:obj\ /Fe:diskexp.exe $?

termui: src\main.cpp $(TMLIBS)
	cl $(CFLAGS) /DTERMUI_TEST /Fo:obj\ /Fe:termui.exe $?

{src}.cpp{obj}.obj::
	cl $(CFLAGS) /c /Fo:obj\ $<

.PHONY: clean
clean:
	del "obj\*.obj" /q


