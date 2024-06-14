
kamiSolver: UI.o solver.o
	g++ -O2 -Wall -Wextra -o kamiSolver UI.o solver.o


%.o: %.cpp
	g++ -O2 -c -Wall -Wextra -o $*.o $*.cpp

.PHONY: clean
clean:
	@(rm *.o 2> /dev/null && echo "rm *.o") || echo "rm *.o     (nothing deleted)"
	@(rm kamiSolver 2> /dev/null && echo "rm kamiSolver") || echo "rm kamiSolver  (nothing deleted)"