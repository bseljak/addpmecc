CC=gcc
target=addpmecc


all:
	$(CC) src/addpmecc.c -o $(target)

clean:
	rm $(target)


