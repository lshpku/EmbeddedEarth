INCLUDE := -I/usr/local/include

LINK := -lglfw -framework OpenGL

OPTION := -Wno-deprecated-declarations "-Wno-\#warnings"

OBJECT := main.o obj.o texture.o shader.o control.o globe.o background.o panel.o digitpanel.o

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
