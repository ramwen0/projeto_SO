# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS =

# Nome do executável
TARGET = so_simulator

# Diretórios
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

# Ficheiros fonte
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))
DEPS = $(INCDIR)/simulation.h $(INCDIR)/queue.h $(INCDIR)/inputs.h


# Regra principal
all: directories $(BINDIR)/$(TARGET)

# Cria os diretórios necessários
directories:
	mkdir -p $(OBJDIR) $(BINDIR)

# Regra de compilação
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

# Regra de linking
$(BINDIR)/$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

# Regra para limpar
clean:
	rm -rf $(OBJDIR) $(BINDIR) output*.out

# Regra para rodar
run: all
	./$(BINDIR)/$(TARGET)

# Regra para rodar com valgrind (detecção de memory leaks)
valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all ./$(BINDIR)/$(TARGET)

.PHONY: all clean run valgrind directories