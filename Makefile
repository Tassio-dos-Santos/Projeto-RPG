# Compilador e flags
CC = gcc
CFLAGS = -Iinclude -pthread
LDFLAGS = -pthread

# Diretórios do projeto
SRC_DIR = src
INC_DIR = include
OBJ_DIR = build
BIN_DIR = bin
LOG_DIR = log

# Nome do executável final
TARGET = $(BIN_DIR)/jogo_RPG.exe

# Arquivos fonte e objetos
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/renderer.c $(SRC_DIR)/logger.c $(SRC_DIR)/data_structures.c
OBJS = $(OBJ_DIR)/main.o $(OBJ_DIR)/renderer.o $(OBJ_DIR)/logger.o $(OBJ_DIR)/data_structures.o

# Arquivos de cabeçalho
DEPS = $(INC_DIR)/renderer.h $(INC_DIR)/logger.h $(INC_DIR)/utils/data_structures.h $(INC_DIR)/utils/status.h

.PHONY: all clean directories

# Regra padrão (executada com só: make)
all: directories $(TARGET)

# Cria diretórios de build se necessário
directories:
# mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Linka os .o e gera o executável
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compila cada .c em .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

# Remove arquivos gerados
clean:
	powershell -Command "Remove-Item -Force $(OBJ_DIR)/*.o, $(BIN_DIR)/*.exe, $(LOG_DIR)/*.log"
