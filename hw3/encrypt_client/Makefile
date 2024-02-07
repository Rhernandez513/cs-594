CC = gcc
CFLAGS = -Wall -Werror

all: encrypt_client

encrypt_client: encrypt_client.o
    $(CC) $(CFLAGS) -o encrypt_client encrypt_client.o

encrypt_client.o: encrypt_client.c
    $(CC) $(CFLAGS) -c encrypt_client.c

clean:
    rm -f encrypt_client encrypt_client.o
