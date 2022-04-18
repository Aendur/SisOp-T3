#####
CFLAGS = -std=c++17 -m64 -Wall -Wextra -Wpedantic
LIBS = $(patsubst source/%.cpp,obj/%.o,$(filter-out source/main.cpp,$(wildcard source/*.cpp source/*/*.cpp)))
#LINKS = -lSDL2 -lpthread
LINKS = -lSDL2
DIRS=bin obj/settings obj/UI

all: server client

server: source/main.cpp dirs $(LIBS)
	g++ $(CFLAGS) -DSERVER -Iheader -obin/server $< $(LIBS) $(LINKS)

client: source/main.cpp dirs $(LIBS)
	g++ $(CFLAGS) -DCLIENT -Iheader -obin/client $<  $(LIBS) $(LINKS)

tests/%: cpp/tests/%.cpp $(LIBS)
	g++ $(CFLAGS) -DUNIT_TEST -Iheader -obin/test $<  $(LIBS)

obj/%.o: source/%.cpp header/%.h
	g++ $(CFLAGS) -Iheader -c -o$@ $<

.PHONY: ipcrm dirs clean cleanse server client

dirs:
	mkdir -p $(DIRS)

# CUIDADO AO UTILIZAR AS RECEITAS ABAIXO POIS ELAS PODEM REMOVER OU SUBSTITUIR ARQUIVOS IMPORTANTES
ipcrm:
	ipcrm -M 0x005da900 -S 0x005da900 -S 0x005da901 -S 0x005da902

clean:
	rm -vfr obj

cleanse: clean
	rm -vfr $(DIRS)

