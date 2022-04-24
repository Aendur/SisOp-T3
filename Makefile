#CFLAGS=/Wall /W4 /EHsc /std:c++17 /Ih /utf-8 /wd5045 /nologo
CFLAGS=/W4 /EHsc /std:c++17 /Ih /utf-8 /nologo

FGLIBN=random_file utility
DXLIBN=disk_explorer term_ui device utility page fat32 entry
TMLIBN=term_ui

FGLIBS=$(patsubst %,obj\\%.obj,$(FGLIBN))
DXLIBS=$(patsubst %,obj\\%.obj,$(DXLIBN))
TMLIBS=$(patsubst %,obj\\%.obj,$(TMLIBN))

all: dirs diskexp

filegen: src\main.cpp $(FGLIBS)
	cl $(CFLAGS) /DFILEGEN /Fo:obj\ /Fe:bin\filegen.exe $?

diskexp: src\main.cpp $(DXLIBS)
	cl $(CFLAGS) /DDISKEXP /Fo:obj\ /Fe:bin\diskexp.exe $?

termui: src\main.cpp $(TMLIBS)
	cl $(CFLAGS) /DTERMUI_TEST /Fo:obj\ /Fe:bin\termui.exe $?

{src\}.cpp{obj\}.obj::
	cl $(CFLAGS) /c /Fo:obj\ $<

{src\clw\}.cpp{obj\}.obj::
	cl $(CFLAGS) /c /Fo:obj\ $<

.PHONY: clean

dirs:
	(IF NOT EXIST bin (MKDIR bin)) & (IF NOT EXIST obj (MKDIR obj))

clean:
	del "obj\*.obj" /q


