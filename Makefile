#CFLAGS=/Wall /W4 /EHsc /std:c++17 /Ih /utf-8 /wd5045 /nologo
CFLAGS=/W4 /EHsc /std:c++17 /Ih /utf-8 /nologo

FGLIBN=random_file utility
DXLIBN=disk_explorer term_ui device utility page input_field editor dialog

FGLIBS=$(patsubst %,obj\\%.obj,$(FGLIBN))
DXLIBS=$(patsubst %,obj\\%.obj,$(DXLIBN))

all: dirs diskexp

filegen: src\filegen.cpp $(FGLIBS)
	cl $(CFLAGS) /DFILEGEN /Fo:obj\ /Fe:bin\filegen.exe $?

diskexp: src\diskexp.cpp $(DXLIBS)
	cl $(CFLAGS) /DDISKEXP /Fo:obj\ /Fe:bin\diskexp.exe $?


#{src\}.cpp{obj\}.obj::
#	cl $(CFLAGS) /c /Fo:obj\ $<

obj\random_file.obj::        src\$(@B).cpp h\$(@B).h
	cl $(CFLAGS) /c /Fo:obj\ src\$(@B).cpp
obj\utility.obj::            src\$(@B).cpp h\$(@B).h
	cl $(CFLAGS) /c /Fo:obj\ src\$(@B).cpp
obj\disk_explorer.obj::      src\$(@B).cpp h\$(@B).h
	cl $(CFLAGS) /c /Fo:obj\ src\$(@B).cpp
obj\term_ui.obj::            src\$(@B).cpp h\$(@B).h
	cl $(CFLAGS) /c /Fo:obj\ src\$(@B).cpp
obj\device.obj::             src\$(@B).cpp h\$(@B).h
	cl $(CFLAGS) /c /Fo:obj\ src\$(@B).cpp
obj\utility.obj::            src\$(@B).cpp h\$(@B).h
	cl $(CFLAGS) /c /Fo:obj\ src\$(@B).cpp
obj\page.obj::               src\$(@B).cpp h\$(@B).h
	cl $(CFLAGS) /c /Fo:obj\ src\$(@B).cpp
obj\input_field.obj::        src\$(@B).cpp h\$(@B).h
	cl $(CFLAGS) /c /Fo:obj\ src\$(@B).cpp
obj\editor.obj::             src\$(@B).cpp h\$(@B).h
	cl $(CFLAGS) /c /Fo:obj\ src\$(@B).cpp
obj\dialog.obj::             src\$(@B).cpp h\$(@B).h
	cl $(CFLAGS) /c /Fo:obj\ src\$(@B).cpp


.PHONY: clean

dirs:
	(IF NOT EXIST bin (MKDIR bin)) & (IF NOT EXIST obj (MKDIR obj))

clean:
	del "obj\*.obj" /q


