INCLUDE := -I/usr/local/include -I/Users/garthlei/Downloads/glfw-3.3.2/include

LINK := -L/usr/local/Cellar/glfw/3.3.2/lib -lglfw -framework OpenGL

OPTION := -std=c++17 -Wno-deprecated-declarations "-Wno-\#warnings"

OBJECT := main.o graphics.o drawable.o globe.o background.o

CXX := clang++

all: main

main: $(OBJECT)
	$(CXX) -o main $(OBJECT) $(LINK)

%.o: %.cc
	$(CXX) -c -o $@ $< $(INCLUDE) $(OPTION)

clean:
	rm -f *.o main

run:
	./main

count:
	ls *.cpp shaders/*.txt | xargs wc -l
