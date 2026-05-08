# Compilador e flags
CC = gcc
CFLAGS = -pthread

# Nome do executável final
TARGET = jogo_RPG.exe

# Nome dos objetos gerados
OBJS = main.o renderer.o logger.o data_structures.o

# Regra padrão (executada com só: make)
all: $(TARGET)

# Linka os .o e gera o executável
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compila cada .c em .o
main.o: main.c renderer.h logger.h data_structures.h
	$(CC) $(CFLAGS) -c main.c

renderer.o: renderer.c renderer.h data_structures.h
	$(CC) $(CFLAGS) -c renderer.c

logger.o: logger.c logger.h data_structures.h
	$(CC) $(CFLAGS) -c logger.c

data_structures.o: data_structures.c data_structures.h
	$(CC) $(CFLAGS) -c data_structures.c

# Remove arquivos gerados
clean:
	powershell -Command "Remove-Item -Force *.o, $(TARGET)"