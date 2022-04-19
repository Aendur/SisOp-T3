#####
CFLAGS = -std=c++17 -m64 -Wall -Wextra -Wpedantic
LIBS = $(patsubst source/%.cpp,obj/%.o,$(filter-out source/main.cpp,$(wildcard source/*.cpp source/*/*.cpp)))

main: source/main.cpp dirs $(LIBS)
	g++ $(CFLAGS) -Iheader -oa.out $< $(LIBS) $(LINKS)

tests/%: cpp/tests/%.cpp $(LIBS)
	g++ $(CFLAGS) -DUNIT_TEST -Iheader -obin/test $<  $(LIBS)

obj/%.o: source/%.cpp header/%.h
	g++ $(CFLAGS) -Iheader -c -o$@ $<

dirs: bin obj

bin:
	mkdir -p bin

obj:
	mkdir -p obj

# CUIDADO AO UTILIZAR AS RECEITAS ABAIXO POIS ELAS PODEM REMOVER OU SUBSTITUIR ARQUIVOS IMPORTANTES
.PHONY: clean cleanse
clean:
	rm -vfr obj

cleanse: clean
	rm -vfr $(DIRS)

