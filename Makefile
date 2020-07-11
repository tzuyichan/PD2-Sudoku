final: generate.cpp transform.o solve.o sudoku.o
	clang++ -o generate -std=c++11 -stdlib=libc++ generate.cpp
	clang++ -o transform -std=c++11 -stdlib=libc++ transform.o sudoku.o
	clang++ -o solve -std=c++11 -stdlib=libc++ solve.o sudoku.o

transform.o: transform.cpp sudoku.h
	clang++ -c -std=c++11 -stdlib=libc++ transform.cpp

solve.o: solve.cpp sudoku.h
	clang++ -c -std=c++11 -stdlib=libc++ solve.cpp

sudoku.o: sudoku.cpp sudoku.h
	clang++ -c -std=c++11 -stdlib=libc++ sudoku.cpp

clean:
	rm generate transform solve *.o