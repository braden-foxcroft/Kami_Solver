
main: UI.o solver.o
	g++ -Wall -Wextra -o main UI.o solver.o


%.o: %.cpp
	g++ -c -Wall -Wextra -o $*.o $*.cpp

.PHONY: clean
clean:
	@(rm *.o 2> /dev/null && echo "rm *.o") || echo "rm *.o     (nothing deleted)"
	@(rm main 2> /dev/null && echo "rm main") || echo "rm main  (nothing deleted)"