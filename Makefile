
#Setting gcc as default compiler flag

ifeq ($(CC),)
	CC=$(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
	CFLAGS= -g -Wall -Werror
endif

ifeq ($(LDFLAGS),)
	LDFLAGS= -pthread -lrt
endif

#Recipe to compile writer.c
all:Client_OP Server_OP Hello_OP Uart_OP

Client_OP:./Socket_Client/Client
	$(CC) $(CFLAGS) -o Client ./Socket_Client/Client.c
	
Server_OP:./Socket_Server/Server
	$(CC) $(CFLAGS) -o Server ./Socket_Server/Server.c

Hello_OP: Hello
	$(CC) $(CFLAGS) -o Hello Hello.c

Uart_OP:./UART_Communication/uart
	$(CC) $(CFLAGS) -o uart ./UART_Communication/uart.c

#cleaning all the executable files
clean:
	rm -f *.o Client Server Hello uart
