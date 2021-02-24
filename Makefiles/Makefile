TARGET = "test"
OBJS = $(wildcard ./obj/*.o)
SRC = $(wildcard ./src/*.c)
LIBS = $(wildcard ./src/*.h)
CC = gcc

$(TARGET): $(OBJS)
	$(CC) -o .bin/$(TARGET) $(SRC)
test.o: $(LIBS) $(SRC)
	$(CC) -c test.c
test2.o: $(LIBS) $(SRC)
	$(CC) -c test2.c
test3.o: $(LIBS) $(SRC) 
	$(CC) -c test3.c
clean:
	rm -rf ./obj/*.o
