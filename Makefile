CC := gcc
CFLAGS := -c

.PHONY : clean

all : example

i2c-lcd.o : i2c-lcd.c
	$(CC) $(CFLAGS) $?

example.o : example.c
	$(CC) $(CFLAGS) $?

example : i2c-lcd.o example.o
	$(CC) $? -o $@

clean :
	rm example *.o
