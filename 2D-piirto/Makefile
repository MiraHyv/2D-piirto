cc = g++
ccflags = `pkg-config glfw3 glew --cflags` -std=c++17

includepath = include
sourcepath = src
objpath = obj

sources = $(wildcard $(sourcepath)/*.cpp)
objects = $(sources:$(sourcepath)/%.cpp=$(objpath)/%.o)

all: $(objects)
	cp $(includepath)/*.h ../include/ #ei näin!
	cp $(includepath)/*.hpp ../include/

$(objects): $(objpath)/%.o : $(sourcepath)/%.cpp
	$(cc) -c $< $(ccflags) -I$(includepath) -o $@

clean:
	rm -f $(objpath)/*
