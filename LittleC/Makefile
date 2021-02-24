# Makefile criado por Gustavo Bacagine <gustavo.bacagine@protonmail.com>
#
# Data de criação: 07/02/2020
# Data da última modificação: 07/02/2020 
#
# Aqui deve vir uma breve esplicação sobre a aplicação
#
# Git Hub: https://github.com/Bacagine/C_Completo_e_Total

TARGET = littlec
OBJS = $(wildcard *.o)
CC = gcc
#FLAGS = -ansi

#all:
#	$(CC) -c $(TARGET).c
#	$(CC) -o $(TARGET) $(OBJS)
	
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)
lclib.o:
	$(CC) -c lclib.c
parser.o:
	$(CC) -c parser.c
	
install:
	cp littlec /usr/bin
uninstall:
	rm littlec /usr/bin/littlec
clean:
	rm -rf $(OBJS)
mrproper: clean
	rm -rf $(TARGET)
