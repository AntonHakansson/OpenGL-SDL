CC			= g++

TARGET		= Md3Loader

$(TARGET):	Main.o Init.o Md3.o
	$(CC) -o $(TARGET) Main.o Init.o Md3.o `sdl-config --libs` -lGL -lGLU -lm

Main.o:		Main.cpp main.h
	$(CC) -c Main.cpp `sdl-config --cflags`

Md3.o:	    Md3. p Md3.h
	$(CC) -c Md3.cpp `sdl-config --cflags`

Init.o:		Init.cpp main.h
	$(CC) -c Init.cpp `sdl-config --cflags`

clean:
	rm -f $(TARGET) Main.o Init.o Md3.o core
