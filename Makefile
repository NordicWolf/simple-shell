CC=gcc
CFLAGS=-c -g -Wall
LFLAGS=-lrt
PARTS=shell.o main.o

# Directorios
OBJ=obj
SRC=src
INC=include

all: build

build:  $(PARTS)
	@echo "Enlazando"
	@$(CC) $(OBJ)/*.o -o bin/shell $(LFLAGS)
	@echo -e "\nListo. El programa se encuentra en el directorio bin"

%.o: $(SRC)/%.c
	@echo "Compilando $(<)"
	@if [ ! -d "$(OBJ)" ]; then mkdir $(OBJ); fi
	@$(CC) $(CFLAGS) $< -o $(OBJ)/$@ -I $(INC)

clean:
	@rm -vf bin/* obj/*.o
