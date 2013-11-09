all: link

# Target zum Compilieren
compile:
	gcc -Wall -c -o sudokualg.o sudokualg.c

# Target zum Linken, bevor gelinkt wird musst compiliert werden
link: compile
	gcc sudoku.c -Wall sudokualg.o -o sudoku.sh

# Fuehrt das Programm aus
run: all
	./sudoku.sh

# Loescht alle Datein die vom Makrfile erstellt wurden
clean:
	@rm sudoku.sh *.o
