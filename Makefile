CPLEXFLAGS=-O3 -m64 -O -fPIC -fexceptions -DNDEBUG -DIL_STD -I/home/kallyous/Apps/CPlex/cplex/include -I/home/kallyous/Apps/CPlex/concert/include  -L/home/kallyous/Apps/CPlex/cplex/lib/x86-64_linux/static_pic -lilocplex -lcplex -L/home/kallyous/Apps/CPlex/concert/lib/x86-64_linux/static_pic -lconcert -lm -pthread -std=c++0x -ldl

CFLAGS=-std=c++11 -static-libstdc++ -static-libgcc -w

%.o: %.cpp %.hpp
	g++ -c $< -o $@ $(CFLAGS)

clean:
	rm -f *.o

test-v1:
	./solver-v1-original < "BHOSLIB-V/frb35/frb35-17-1"

test-current:
	./solver < "BHOSLIB-V/frb35/frb35-17-1"

run-v1:
	./solver-v1-original < "DIMACS-V/C/C1000-9"

generate-v1:
	echo "GENERATE ORIGINAL SOLVER STARTED"
	g++ -c -o solver-original.o solver-original.cpp $(CPLEXFLAGS) $(CFLAGS)
	g++  solver-original.o -o solver-v1-original $(CPLEXFLAGS) $(CFLAGS)
	echo "TEST BUILD"
	./solver-v1-original < "DIMACS-V/MANN/MANN-a9"

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

generate-callback-branch-v1:
	echo "GENERATE CALLBACK SOLVER STARTED"
	g++ -c -o solver-callback-branch-v1.o solver-callback-branch-v1.cpp $(CPLEXFLAGS) $(CFLAGS)
	g++  solver-callback-branch-v1.o -o solver $(CPLEXFLAGS) $(CFLAGS)
	echo "TEST BUILD"
	./solver < "DIMACS-V/MANN/MANN-a9"

generate-callback-branch-v2:
	echo "GENERATE CALLBACK SOLVER STARTED"
	g++ -c -o solver-callback-branch-v2.o solver-callback-branch-v2.cpp $(CPLEXFLAGS) $(CFLAGS)
	g++  solver-callback-branch-v2.o -o solver $(CPLEXFLAGS) $(CFLAGS)
	echo "TEST BUILD"
	./solver < "DIMACS-V/MANN/MANN-a9"

generate-callback-branch-v3:
	echo "GENERATE CALLBACK SOLVER STARTED"
	g++ -c -o solver-callback-branch-v3.o solver-callback-branch-v3.cpp $(CPLEXFLAGS) $(CFLAGS)
	g++  solver-callback-branch-v3.o -o solver $(CPLEXFLAGS) $(CFLAGS)
	echo "TEST BUILD"
	./solver < "DIMACS-V/MANN/MANN-a9"