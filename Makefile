SRCDIR=src
INCDIR=include
OBJDIR=obj
CFLAGS=/W4 /EHsc /std:c++20 /I$(INCDIR) /utf-8 /nologo

FGLIBN=seqfile utility
DXLIBN=disk_explorer term_ui device entry entry_metadata utility page input_field editor navigator ghost_ship seqfile dialog popup

FGLIBS=$(patsubst %,obj\\%.obj,$(FGLIBN))
DXLIBS=$(patsubst %,obj\\%.obj,$(DXLIBN))

all: dirs diskexp

filegen: src\filegen.cpp $(FGLIBS)
	cl $(CFLAGS) /DFILEGEN /Fo:obj\ /Fe:filegen.exe $?

diskexp: src\diskexp.cpp $(DXLIBS)
	cl $(CFLAGS) /DDISKEXP /Fo:obj\ /Fe:diskexp.exe $?

update:
	scripts\update_symlinks.bat

$(OBJDIR)\seqfile.obj::            $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:$(OBJDIR)\ $(SRCDIR)\$(@B).cpp
$(OBJDIR)\utility.obj::            $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:$(OBJDIR)\ $(SRCDIR)\$(@B).cpp

$(OBJDIR)\entry.obj::              $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:$(OBJDIR)\ $(SRCDIR)\$(@B).cpp
$(OBJDIR)\entry_metadata.obj::     $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:$(OBJDIR)\ $(SRCDIR)\$(@B).cpp

$(OBJDIR)\disk_explorer.obj::      $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:$(OBJDIR)\ $(SRCDIR)\$(@B).cpp
$(OBJDIR)\term_ui.obj::            $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:$(OBJDIR)\ $(SRCDIR)\$(@B).cpp
$(OBJDIR)\device.obj::             $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:$(OBJDIR)\ $(SRCDIR)\$(@B).cpp
$(OBJDIR)\page.obj::               $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:$(OBJDIR)\ $(SRCDIR)\$(@B).cpp
$(OBJDIR)\input_field.obj::        $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:$(OBJDIR)\ $(SRCDIR)\$(@B).cpp
$(OBJDIR)\editor.obj::             $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:$(OBJDIR)\ $(SRCDIR)\$(@B).cpp
$(OBJDIR)\navigator.obj::          $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:$(OBJDIR)\ $(SRCDIR)\$(@B).cpp
$(OBJDIR)\ghost_ship.obj::         $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:$(OBJDIR)\ $(SRCDIR)\$(@B).cpp

$(OBJDIR)\dialog.obj::             $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:$(OBJDIR)\ $(SRCDIR)\$(@B).cpp
$(OBJDIR)\popup.obj::              $(SRCDIR)\$(@B).cpp $(INCDIR)\$(@B).h
	cl $(CFLAGS) /c /Fo:$(OBJDIR)\ $(SRCDIR)\$(@B).cpp


dirs:
	(IF NOT EXIST obj (MKDIR obj))

#(IF NOT EXIST bin (MKDIR bin)) & (IF NOT EXIST obj (MKDIR obj))

clean:
	FOR %I IN (obj\*) DO @((echo Removing file %I) & (del %I))
	IF EXIST obj (rmdir obj)
.PHONY: clean

#del "obj\*.obj" /q



