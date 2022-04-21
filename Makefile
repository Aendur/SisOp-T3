CFLAGS=/Wall /W4 /EHsc /std:c++17 /Ih /utf-8 /wd5045 /nologo
LIBN=random_file utility
LIBS=$(patsubst %,obj\\%.obj,$(LIBN))

all: filegen

filegen: src\main.cpp $(LIBS)
	cl $(CFLAGS) /DFILEGEN /Fo:obj\ /Fe:filegen.exe $?

diskexp: src\main.cpp $(LIBS)
	cl $(CFLAGS) /DDISKEXP /Fo:obj\ /Fe:diskexp.exe $?

{src}.cpp{obj}.obj::
	cl $(CFLAGS) /c /Fo:obj\ $<

.PHONY: clean
clean:
	del "obj\*.obj" /q


