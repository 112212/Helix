#inc := -Iinclude -Isrc/Externallib
link64bit := -Llib/linux/64bit -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_gfx -lSDL2_ttf \
				-Wl,-rpath="../lib/linux/64bit/",-rpath="../lib/linux" -Llib/linux -lGL -lGLEW -lassimp
link32bit := -Llib/linux/32bit -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_gfx -lSDL2_ttf \
				-Wl,-rpath="../lib/linux/32bit/",-rpath="../lib/linux" -Llib/linux -lGL -lGLEW -lassimp
link := $(link64bit)

arch := 

hpp :=	\
#		src/Engine/Engine.hpp							\
		
cpp := 	\
		src/Main.cpp									\
		src/App.cpp										\
		src/Engine/Engine.cpp							\
		src/Engine/Gui.cpp								\
		src/Engine/Shader.cpp							\
		src/Engine/Model.cpp							\
		src/Engine/Camera.cpp							\

exe := release/Helix

build := build
flags := -O2

obj := $(addprefix $(build)/, $(patsubst %.cpp,%.o,$(cpp)))

.phony: make_dirs

all: make_dirs $(exe)

clean:
	find $(build) -type f -name *.o -exec rm {} \;

make_dirs:
	@mkdir -p $(build)
	@mkdir -p $(build)/src/
	@mkdir -p $(build)/src/Engine/
	@mkdir -p $(build)/src/Engine/Shaders/

$(exe): $(obj)
	g++ $^ -o $(exe) $(link) $(arch) -pthread 
#	g++ $^ -Wall -g -o $(exe) $(link) $(arch) -pthread 

$(build)/%.o: %.cpp
	g++ -c $< -o $@ -std=c++14 $(inc) $(arch) -pthread $(flags)
#	g++ -c $< -Wall -g -o $@ -std=c++14 $(inc) $(arch) -pthread $(flags)

#i686-w64-mingw32-g++
#x86_64-w64-mingw32-g++
