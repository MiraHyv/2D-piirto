cc = g++
ccflags = -std=c++17

includepath = ../include
sourcepath = src
objpath = obj

sources = $(wildcard $(sourcepath)/*.cpp)
objects = $(sources:$(sourcepath)/%.cpp=$(objpath)/%.o)

all: $(objects)

$(objects): $(objpath)/%.o : $(sourcepath)/%.cpp
	$(cc) -c $< $(cflags) -I$(includepath) -o $@

clean:
	rm -f $(objpath)/*
