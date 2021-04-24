GXX = g++
MODULES = -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system
EXE_NAME = game.out

$(EXE_NAME): main.o funcs.o
	$(GXX) -o $(EXE_NAME) main.o funcs.o $(MODULES)

main.o: main.cpp
	$(GXX) -c main.cpp

funcs.o: funcs.cpp
	$(GXX) -c funcs.cpp

clean:
	rm *.o $(EXE_NAME)