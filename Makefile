CPLEXFLAGS=-O3 -m64 -O -fPIC -fexceptions -DNDEBUG -DIL_STD -I/home/kallyous/Apps/CPlex/cplex/include -I/home/kallyous/Apps/CPlex/concert/include  -L/home/kallyous/Apps/CPlex/cplex/lib/x86-64_linux/static_pic -lilocplex -lcplex -L/home/kallyous/Apps/CPlex/concert/lib/x86-64_linux/static_pic -lconcert -lm -pthread -std=c++0x -ldl

CFLAGS=-std=c++11 -static-libstdc++ -static-libgcc -w

%.o: %.cpp %.hpp
	g++ -c $< -o $@ $(CFLAGS)

clean:
	rm -f *.o

test:
	./solver < "DIMACS-V/C/C250-9"

run-v1:
	./solver-v1-original < "DIMACS-V/C/C250-9"

generate-v1:
	echo "GENERATE ORIGINAL SOLVER STARTED"
	g++ -c -o solver-original.o solver-original.cpp $(CPLEXFLAGS) $(CFLAGS)
	g++  solver-original.o -o solver $(CPLEXFLAGS) $(CFLAGS)
	echo "TEST BUILD"
	./solver < "DIMACS-V/MANN/MANN-a9"

generate-lazy-restrict:
	echo "GENERATE LAZY SOLVER STARTED"
	g++ -c -o solver-lazy-restrict.o solver-lazy-restrict.cpp $(CPLEXFLAGS) $(CFLAGS)
	g++  solver-lazy-restrict.o -o solver $(CPLEXFLAGS) $(CFLAGS)
	echo "TEST BUILD"
	./solver < "DIMACS-V/MANN/MANN-a9"

generate-callback-restrict:
	echo "GENERATE CALLBACK SOLVER STARTED"
	g++ -c -o solver-callback-lazy-restrict.o solver-callback-lazy-restrict.cpp $(CPLEXFLAGS) $(CFLAGS)
	g++  solver-callback-lazy-restrict.o -o solver $(CPLEXFLAGS) $(CFLAGS)
	echo "TEST BUILD"
	./solver < "DIMACS-V/MANN/MANN-a9"

generate-callback-branch:
	echo "GENERATE CALLBACK SOLVER STARTED"
	g++ -c -o solver-callback-branch.o solver-callback-branch.cpp $(CPLEXFLAGS) $(CFLAGS)
	g++  solver-callback-branch.o -o solver $(CPLEXFLAGS) $(CFLAGS)
	echo "TEST BUILD"
	./solver < "DIMACS-V/MANN/MANN-a9"