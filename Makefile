#Setting gcc as default compiler flag
ifeq ($(CC),)
	CC=$(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
	CFLAGS= -g -Wall -Werror
endif

#Recipe to compile
all: Server_OP Temperature_i2c_OP Uart_raspi_OP All_Task_OP All_Data_Sync_OP User_modes_OP bme280_OP HRT_System_OP

Temperature_i2c_OP:./Temp_i2c/Temperature_i2c
	$(CC) $(CFLAGS) -o Temperature_i2c ./Temp_i2c/Temperature_i2c.c

Uart_raspi_OP:./Uart_rpi/Uart_raspi
	$(CC) $(CFLAGS) -o Uart_raspi ./Uart_rpi/Uart_raspi.c

Server_OP:./Socket_Server/Server
	$(CC) $(CFLAGS) -o Server ./Socket_Server/Server.c $(INCLUDES)

Temp_Test_OP:./I2C_TEMP_BB/Temp_Test
	$(CC) $(CFLAGS) -o Temp_Test ./I2C_TEMP_BB/Temp_Test.c

All_Task_OP:UART2_Temp
	$(CC) $(CFLAGS) -o UART2_Temp UART2_Temp.c

All_Data_Sync_OP:All_Data_Sync
	$(CC) $(CFLAGS) -o All_Data_Sync All_Data_Sync.c

User_modes_OP:./User_modes/Comparison_mode
	$(CC) $(CFLAGS) -o Comparison_mode ./User_modes/Comparison_mode.c

bme280_OP:./I2C_Data_BB/bme280
	$(CC) $(CFLAGS) -o bme280 ./I2C_Data_BB/bme280.c

HRT_System_OP:./Sync_Integrate/main.c ./Sync_Integrate/hard_realtime.c
	$(CC) $(CFLAGS) -o main ./Sync_Integrate/main.c ./Sync_Integrate/hard_realtime.c $(INCLUDES) 

#cleaning all the executable files
clean:
	rm -f *.o Server  Temperature_i2c ./Socket_Server/Server ./Socket_Client/Client UART2_Temp All_Data_Sync Comparison_mode /User_modes/Comparison_mode bme280 ./I2C_Data_BB/bme280 main



