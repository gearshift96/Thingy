CC=g++
CFLAGS=-Wall -Wextra -Werror -ansi -pedantic -std=c++11
CFLAGS_EXTRA=
MSFLAGS=/W4 /EHsc /Zi /Za
NAME=FOO
TYPE=cpp

gnu:
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $(NAME).$(TYPE) -o $(NAME) && ./$(NAME)

build:
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $(NAME).$(TYPE) -o $(NAME)

ms:
	cl $(MSFLAGS) $(NAME).$(TYPE) && ./$(NAME)
	drmemory -batch -brief -- $(NAME) 1>/dev/null