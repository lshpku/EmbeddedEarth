INCLUDE := -I${GLFW_PATH} -I${ES_PATH}

LINK := -L${GLFW_LD_PATH} -lGLESv2 -lglfw3 -ldl -lpthread -lX11

OPTION := -std=c++17 -Wall -Werror

OBJECT := main.o graphics.o drawable.o globe.o background.o control_bar.o digit_panel.o

CXX := clang++

all: main

main: $(OBJECT)
	$(CXX) -o main $(OBJECT) $(LINK)

%.o: %.cpp
	$(CXX) -c -o $@ $< $(INCLUDE) $(OPTION)

clean:
	rm -f *.o main

run:
	./main

count:
	ls *.cpp shaders/*.txt | xargs wc -l
