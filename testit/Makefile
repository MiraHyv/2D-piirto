cc = g++
ccflags = -std=c++17 -I../include -Iinclude

#includepath = ../include
sourcepath = src
objpath = obj
libpath = lib

googletestBuildPath = googletest-build
googletestPath = googletest
googletestLibs = $(wildcard $(googletestBuildPath)/lib/*.a)

sources = $(wildcard $(sourcepath)/*.cpp)
objects = $(sources:$(sourcepath)/%.cpp=$(objpath)/%.o)

all: buildGoogletest $(objects)

$(objects): $(objpath)/%.o : $(sourcepath)/%.cpp
	$(cc) -c $< -$(ccflags) -o $@

clean:
	rm -f $(objpath)/*
	rm -f $(libpath)/*
#	rm -rf $(googletestBuildPath)

remove: clean
	rm -rf $(googletestBuildPath)

buildGoogletest:
	mkdir -p $(googletestBuildPath)
	cmake -B$(googletestBuildPath) -H$(googletestPath)
	make -C $(googletestBuildPath)
	cp $(googletestBuildPath)/lib/*.a $(libpath)/
	cp -R googletest/googletest/include/* include/
	cp -R googletest/googlemock/include/* include/
