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
all:Client_OP Server_OP Hello_OP Uart_write_OP Uart_read_OP Temperature_i2c_OP Uart_raspi_OP Uart_readwrite_OP Temp_Test_OP Uart_Rasp_ARD_WR_OP All_Task_OP All_Data_Sync_OP

Temperature_i2c_OP:./Temp_i2c/Temperature_i2c
	$(CC) $(CFLAGS) -o Temperature_i2c ./Temp_i2c/Temperature_i2c.c

Uart_raspi_OP:./Uart_rpi/Uart_raspi
	$(CC) $(CFLAGS) -o Uart_raspi ./Uart_rpi/Uart_raspi.c

Client_OP:./Socket_Client/Client
	$(CC) $(CFLAGS) -o Client ./Socket_Client/Client.c $(LDFLAGS) $(INCLUDES)
	
Server_OP:./Socket_Server/Server
	$(CC) $(CFLAGS) -o Server ./Socket_Server/Server.c $(LDFLAGS) $(INCLUDES)

Hello_OP: Hello
	$(CC) $(CFLAGS) -o Hello Hello.c

Uart_write_OP:./UART_Communication/uart_write
	$(CC) $(CFLAGS) -o uart_write ./UART_Communication/uart_write.c

Uart_read_OP:./UART_Communication/uart_read
	$(CC) $(CFLAGS) -o uart_read ./UART_Communication/uart_read.c

Uart_readwrite_OP:./UART_Communication/uart_ReadWrite
	$(CC) $(CFLAGS) -o uart_ReadWrite ./UART_Communication/uart_ReadWrite.c

Temp_Test_OP:./I2C_TEMP_BB/Temp_Test
	$(CC) $(CFLAGS) -o Temp_Test ./I2C_TEMP_BB/Temp_Test.c

Uart_Rasp_ARD_WR_OP:./UART_Communication/uart_Rasp_ARD_WR
	$(CC) $(CFLAGS) -o uart_Rasp_ARD_WR ./UART_Communication/uart_Rasp_ARD_WR.c

All_Task_OP:UART2_Temp
	$(CC) $(CFLAGS) -o UART2_Temp UART2_Temp.c

All_Data_Sync_OP:All_Data_Sync
	$(CC) $(CFLAGS) -o All_Data_Sync All_Data_Sync.c

#cleaning all the executable files
clean:
	rm -f *.o Client Server Hello uart_write uart_read Temperature_i2c Temp_Test uart_Rasp_ARD_WR ./UART_Communication/uart_Rasp_ARD_WR ./UART_Communication/uart_ReadWrite ./UART_Communication/uart_write ./UART_Communication/uart_read ./Socket_Server/Server ./Socket_Client/Client UART2_Temp All_Data_Sync

