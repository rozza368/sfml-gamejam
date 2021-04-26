GXX = g++
MODULES = -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system
EXE_NAME = game.out

$(EXE_NAME): main.o funcs.o gameClasses.o cutscene.o
	$(GXX) -o $(EXE_NAME) main.o funcs.o gameClasses.o cutscene.o $(MODULES)

main.o: main.cpp
	$(GXX) -c main.cpp

funcs.o: funcs.cpp funcs.hpp
	$(GXX) -c funcs.cpp

gameClasses.o: gameClasses.cpp gameClasses.hpp
	$(GXX) -c gameClasses.cpp

cutscene.o: cutscene.cpp cutscene.hpp
	$(GXX) -c cutscene.cpp

clean:
	rm *.o $(EXE_NAME)