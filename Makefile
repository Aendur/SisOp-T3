#CFLAGS=/Wall /W4 /EHsc /std:c++17 /Ih /utf-8 /wd5045 /nologo
CFLAGS=/W4 /EHsc /std:c++17 /Ih /utf-8 /nologo

FGLIBN=utility
DXLIBN=term_ui device utility page input_field fat32 entry

FGLIBS=$(patsubst %,obj\\%.obj,$(FGLIBN))
DXLIBS=$(patsubst %,obj\\%.obj,$(DXLIBN))

all: dirs filegen diskexp

filegen: src\random_file.cpp $(FGLIBS)
	cl $(CFLAGS) /DFILEGEN /Fo:obj\ /Fe:bin\filegen.exe $?

diskexp: src\disk_explorer.cpp $(DXLIBS)
	cl $(CFLAGS) /DDISKEXP /Fo:obj\ /Fe:bin\diskexp.exe $?

{src\}.cpp{obj\}.obj::
	cl $(CFLAGS) /c /Fo:obj\ $<

{src\clw\}.cpp{obj\}.obj::
	cl $(CFLAGS) /c /Fo:obj\ $<

.PHONY: clean

dirs:
	(IF NOT EXIST bin (MKDIR bin)) & (IF NOT EXIST obj (MKDIR obj))

clean:
	del "obj\*.obj" /q


