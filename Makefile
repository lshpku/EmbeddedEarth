INCLUDE := -I${GLFW_PATH} -I${ES_PATH}

LINK := -L${GLFW_LD_PATH} -lGLESv2 -lglfw3 -ldl -lpthread -lX11

OPTION :=

OBJECT := main.o obj.o texture.o shader.o control.o globe.o background.o panel.o

all: main

main: $(OBJECT)
	g++ -o main $(OBJECT) $(LINK)

%.o: %.cpp
	g++ -c -o $@ $< $(INCLUDE) $(OPTION)

clean:
	rm -f *.o main

run:
	./main

count:
	ls *.cpp shaders/*.txt | xargs wc -l
