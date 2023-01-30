includepath = include

target = ohjelma
testTarget = ajaTestit

demoObj = $(wildcard demo/obj/*.o)
testitObj = $(wildcard testit/obj/*.o)
piirtoObj = $(wildcard 2D-piirto/obj/*.o)

lflags_testit = -Ltestit/lib -lgmock -lgtest -pthread
lflags_piirto = `pkg-config glfw3 glew --libs`
lflags_demo =

#tehdään aina clean ensin koska muuten .h:t ei välttämättä ole ajan tasalla

all: clean $(target)

test: clean $(testTarget)
	./$(testTarget)

$(target): buildPiirto buildDemo
	g++ $(demoObj) $(piirtoObj) $(lflags_demo) $(lflags_piirto) -o $@

$(testTarget) : buildPiirto buildTestit
	g++ $(testitObj) $(piirtoObj) $(lflags_testit) $(lflags_piirto) -o $@

buildDemo:
	make -C demo

buildTestit:
	make -C testit

buildPiirto:
	make -C 2D-piirto

#clean: poista tehdyt binäärit ja kirjastot sekä objektit
clean:
	make clean -C demo
	make clean -C testit
	make clean -C 2D-piirto
	rm -f $(includepath)/*
	rm -f $(target) $(testTarget)

#remove: sama kuin clean, mutta poista vielä googletestin kirjastot
remove: clean
	make remove -C testit
