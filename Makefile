CPLEXFLAGS=-O3 -m64 -O -fPIC -fexceptions -DNDEBUG -DIL_STD -I/home/kallyous/Apps/CPlex/cplex/include -I/home/kallyous/Apps/CPlex/concert/include  -L/home/kallyous/Apps/CPlex/cplex/lib/x86-64_linux/static_pic -lilocplex -lcplex -L/home/kallyous/Apps/CPlex/concert/lib/x86-64_linux/static_pic -lconcert -lm -pthread -std=c++0x -ldl

CFLAGS=-std=c++11 -static-libstdc++ -static-libgcc -Wall

%.o: %.cpp %.hpp
	g++ -c $< -o $@ $(CFLAGS)

clean:
	rm -f *.o

test:
	./solver < "DIMACS-V/C/C250-9"

run-v1:
	./solver-v1-orig < "DIMACS-V/C/C250-9"

generate-v1:
	echo "GENERATE ORIGINAL SOLVER STARTED"
	g++ -c -o solver-orig.o solver-orig.cpp $(CPLEXFLAGS) $(CFLAGS)
	g++  solver-orig.o -o solver $(CPLEXFLAGS) $(CFLAGS)
	echo "TEST BUILD"
	./solver < "DIMACS-V/MANN/MANN-a9"

generate-lazy:
	echo "GENERATE LAZY SOLVER STARTED"
	g++ -c -o solver-lazy.o solver-lazy.cpp $(CPLEXFLAGS) $(CFLAGS)
	g++  solver-lazy.o -o solver $(CPLEXFLAGS) $(CFLAGS)
	echo "TEST BUILD"
	./solver < "DIMACS-V/MANN/MANN-a9"

generate-callback:
	echo "GENERATE CALLBACK SOLVER STARTED"
	g++ -c -o solver-callback.o solver-callback.cpp $(CPLEXFLAGS) $(CFLAGS)
	g++  solver-callback.o -o solver $(CPLEXFLAGS) $(CFLAGS)
	echo "TEST BUILD"
	./solver < "DIMACS-V/MANN/MANN-a9"
