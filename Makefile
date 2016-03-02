#inc := -Iinclude -Isrc/Externallib
link64bit := -Llib/linux/64bit -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_gfx \
				-Wl,-rpath="../lib/linux/64bit/",-rpath="../lib/linux" -Llib/linux -lGL -lGLEW
link32bit := -Llib/linux/32bit -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_gfx \
				-Wl,-rpath="../lib/linux/32bit/",-rpath="../lib/linux" -Llib/linux -lGL -lGLEW
link := $(link64bit)

arch := 

hpp :=	\
		src/Engine/Engine.hpp							\
		
cpp := 	\
		src/Main.cpp									\
		src/Engine/Fps.cpp								\
		src/Engine/Gui.cpp								\
		src/Engine/ShaderManager.cpp					\

exe := Release/Helix

build := build
flags := -O2

obj := $(addprefix $(build)/, $(patsubst %.cpp,%.o,$(cpp)))

.phony: make_dirs

all: make_dirs $(exe)

clean:
	find $(build) -type f -name *.o -exec rm {} \;

make_dirs:
	@mkdir -p $(build)
	@mkdir -p $(build)/src
	@mkdir -p $(build)/src/Engine/

$(exe): $(obj)
	g++ $^ -o $(exe) $(link) $(arch) -pthread 

$(build)/%.o: %.cpp
	g++ -c $< -o $@ -std=c++11 $(inc) $(arch) -pthread $(flags)
