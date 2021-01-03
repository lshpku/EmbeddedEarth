INCLUDE := -I./include -I${GLFW_PATH}

LINK := -L${GLFW_LD_PATH} -lglfw3 -ldl -lpthread -lX11

OPTION := -std=c++17 -Wall

OBJECT := main.o graphics.o drawable.o globe.o background.o control_bar.o digit_panel.o glad.o

CXX := g++

all: main

main: $(OBJECT)
	$(CXX) -o main $(OBJECT) $(LINK)

glad.o: glad.c
	$(CXX) -c -o $@ $< $(INCLUDE) $(OPTION)

%.o: %.cpp
	$(CXX) -c -o $@ $< $(INCLUDE) $(OPTION)

clean:
	rm -f *.o main

run:
	./main

count:
	ls *.cpp shaders/*.txt | xargs wc -l
