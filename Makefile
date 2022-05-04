#CFLAGS=/Wall /W4 /EHsc /std:c++17 /Iheader /utf-8 /wd5045 /nologo
#CFLAGS=/W4 /EHsc /std:c++17 /Iheader /utf-8 /nologo
SRCDIR=src
INCDIR=src
CFLAGS=/W4 /EHsc /std:c++20 /I$(INCDIR) /utf-8 /nologo

FGLIBN=seqfile utility
DXLIBN=disk_explorer term_ui device utility page input_field editor navigator dialog popup

FGLIBS=$(patsubst %,obj\\%.obj,$(FGLIBN))
DXLIBS=$(patsubst %,obj\\%.obj,$(DXLIBN))

all: dirs diskexp

filegen: src\filegen.cpp $(FGLIBS)
	cl $(CFLAGS) /DFILEGEN /Fo:obj\ /Fe:filegen.exe $?

diskexp: src\diskexp.cpp $(DXLIBS)
	cl $(CFLAGS) /DDISKEXP /Fo:obj\ /Fe:diskexp.exe $?


obj\seqfile.obj::            $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:obj\ $(SRCDIR)\$(@B).cpp
obj\utility.obj::            $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:obj\ $(SRCDIR)\$(@B).cpp

obj\disk_explorer.obj::      $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:obj\ $(SRCDIR)\$(@B).cpp
obj\term_ui.obj::            $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:obj\ $(SRCDIR)\$(@B).cpp
obj\device.obj::             $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:obj\ $(SRCDIR)\$(@B).cpp
obj\page.obj::               $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:obj\ $(SRCDIR)\$(@B).cpp
obj\input_field.obj::        $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:obj\ $(SRCDIR)\$(@B).cpp
obj\editor.obj::             $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:obj\ $(SRCDIR)\$(@B).cpp
obj\navigator.obj::          $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:obj\ $(SRCDIR)\$(@B).cpp
obj\dialog.obj::             $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:obj\ $(SRCDIR)\$(@B).cpp
obj\popup.obj::              $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:obj\ $(SRCDIR)\$(@B).cpp


dirs:
	(IF NOT EXIST obj (MKDIR obj))

#(IF NOT EXIST bin (MKDIR bin)) & (IF NOT EXIST obj (MKDIR obj))

clean:
	FOR %I IN (obj\*) DO @((echo Removing file %I) & (del %I))
	IF EXIST obj (rmdir obj)
.PHONY: clean

#del "obj\*.obj" /q



