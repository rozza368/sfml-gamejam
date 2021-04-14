GXX = g++
MODULES = -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system
EXE_NAME = game.out

$(EXE_NAME): main.o
	$(GXX) -o $(EXE_NAME) main.o $(MODULES)

main.o: main.cpp
	$(GXX) -c main.cpp

clean:
	rm *.o $(EXE_NAME)