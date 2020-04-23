//https://github.com/ControlEverythingCommunity/SI7020-A20/blob/master/C/SI7020_A20.c

// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// SI7020_A20
// This code is designed to work with the SI7020_A20_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Humidity?sku=SI7006-A20_I2CS#tabs-0-product_tabset-2

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define Si7021_address 0x40
int beaglebone=1;
int main()
{
	time_t time_current;
	struct tm *local = NULL;
	char *time_buffer = NULL;
	// Create I2C bus
	int file;
	char *bus = "/dev/i2c-1";
	if((file = open(bus, O_RDWR)) < 0)
	{
		printf("Failed to open the bus. \n");
		exit(1);
	}
	// Get I2C device, SI7020_A20 I2C address is 0x40(64)
	ioctl(file, I2C_SLAVE, Si7021_address);

	// Send humidity measurement command, NO HOLD MASTER(0xF5)
	char config[1] = {0xF5};
	write(file, config, 1);
	sleep(1);

	// Read 2 bytes of humidity data
	// humidity msb, humidity lsb
	
	while(1)
	{
		if(beaglebone==1)
		{
		char data[2] = {0};
		if(read(file, data, 2) != 2)
		{
			printf("Error : Input/Output error \n");
		}
		else
		{
			// Convert the data
			float humidity = (((data[0] * 256 + data[1]) * 125.0) / 65536.0) - 6;

			// Output data to screen
			printf("Relative Humidity : %.2f RH \n", humidity);
		}

		// Send temperature measurement command, NO HOLD MASTER(0xF3)
		config[0] = 0xF3;
		write(file, config, 1); 
		sleep(1);

		// Read 2 bytes of temperature data
		// temp msb, temp lsb
		if(read(file, data, 2) != 2)
		{
			printf("TIME:%s    Error : Input/Output error \n",time_buffer);
		}
		else
		{
			time( &time_current );
	
			local = localtime( &time_current );
	
			time_buffer = asctime(local);
			// Convert the data
			float cTemp = (((data[0] * 256 + data[1]) * 175.72) / 65536.0) - 46.85;
			float fTemp = cTemp * 1.8 + 32;

			// Output data to screen
			printf("TIME:%s    Temperature in Celsius : %.2f C \n",time_buffer, cTemp);
			printf("TIME:%s    Temperature in Fahrenheit : %.2f F \n",time_buffer, fTemp);
		}
		}
	}
return 0;

}


