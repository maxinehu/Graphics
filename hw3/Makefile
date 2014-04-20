CC=g++
CXXFLAGS:=-g
LDFLAGS:=-g -lGL -lGLU -lglut -lGLEW

OBJECTS=hw2.o utils.o debuglib.o tga.o

all: hw2

hw2: $(OBJECTS)
	$(CC) $(OBJECTS) -o hw2 $(LDFLAGS)

