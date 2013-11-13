all: link

# Target zum Kompilieren
compile:
	gcc -Wall -c -o sudokualg.o sudokualg.c

# Target zum Linken, bevor gelinkt wird musst compiliert werden
link: compile
	gcc sudoku.c -Wall sudokualg.o -o sudoku.sh

# Fuehrt das Programm aus
run: all
	./sudoku.sh
	
# Fuehrt das Programm mit fixen Werten aus
run-test: all
	./sudoku.sh normal puzzle1.csv out.csv
	./sudoku.sh x-sudoku xsudo1.csv xsudokuOut.csv

# Loescht alle Datein die vom Makefile erstellt wurden
clean:
	@rm sudoku.sh *.o
