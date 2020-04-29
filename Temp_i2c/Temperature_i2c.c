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

#define BUFFER_SIZE 100 

#include <string.h>
#include <unistd.h>

#include <termios.h>

#define Si7021_address 0x40
int fd;
float cTemp;
float fTemp;
int rcount;

char conv[20];

int send_temp;

int file;

 char receive[BUFFER_SIZE];      //declare a buffer for receiving 

int main()
{
	time_t time_current;
	struct tm *local = NULL;
	char *time_buffer = NULL;
	// Create I2C bus
	int file_i2c;
	char *bus = "/dev/i2c-1";
	if((file_i2c = open(bus, O_RDWR)) < 0)
	{
		printf("Failed to open the bus. \n");
		exit(1);
	}
	// Get I2C device, SI7020_A20 I2C address is 0x40(64)
	ioctl(file_i2c, I2C_SLAVE, Si7021_address);

	while(1)
	{

/********************************************open file**********************************/
   if ((file = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY))<0)
   {
      printf("UART: Failed to open the file.\n");
      return -1;
   }
   struct termios options;               //The termios structure is vital
   tcgetattr(file, &options);            //Sets the parameters associated with file

   // Set up the communications options:
   //   115200 baud, 8-bit, enable receiver, no modem control lines
   options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
   options.c_iflag = IGNPAR | ICRNL; 
   options.c_oflag = 0;
   options.c_lflag = 0;
 //tcflush(file, TCIFLUSH);  
             //discard file information not transmitted
   tcsetattr(file, TCSANOW, &options);  //changes occur immmediately

    char config[1] = {0xF3};
	write(file, config, 1); 
    char data[2] = {0};


/*****DATA Reading operation*****************************************************/
    fcntl(file, F_SETFL, 0);
    int count;
    char receive[BUFFER_SIZE];      //declare a buffer for receiving data
   if ((count = read(file, (void*)receive, 2))<0){   //receive the data
      printf("Failed to read from the input\n");
     return -1;
     }
   if (count==0) 
        printf("There was no data available to read!\n");
   else {
    receive[count]='\0';
    printf("The following sensor value was read in [%d]: %s\n",count,receive);
   }
    close(file);

	if(strchr(receive,'1'))
	{
		usleep(200000);

		// Send temperature measurement command, NO HOLD MASTER(0xF3)
		// Read 2 bytes of temperature data
		// temp msb, temp lsb

		printf("\nTemp Reading data \n");

		if(read(file_i2c, data, 2) != 2)
		{
			printf("TIME   Error : Input/Output error \n");
		}
		else
		{
		// Convert the data
			cTemp = (((data[0] * 256 + data[1]) * 175.72) / 65536.0) - 46.85;
			fTemp = cTemp * 1.8 + 32;

		// Output data to screen
			printf("TIME:%s  \n  Temperature in Celsius : %.2f C \n",time_buffer, cTemp);
			printf("\n  Temperature in Fahrenheit : %.2f F \n", fTemp);
		}
		printf("\nstarting uart\n");
    
		send_temp=fTemp*100;

		sprintf(conv,"%d",send_temp);

/********************************************open file**********************************/
		if ((fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY))<0)  {
			printf("UART: Failed to open the file.\n");
			return -1;
		}
		struct termios options;               //The termios structure is vital
		tcgetattr(fd, &options);            //Sets the parameters associated with file

		// Set up the communications options:
		//   9600 baud, 8-bit, enable receiver, no modem control lines
		options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
		options.c_iflag = IGNPAR | ICRNL; 

		tcflush(fd, TCIFLUSH);  
        //discard file information not transmitted
		tcsetattr(fd, TCSANOW, &options);  //changes occur immmediately

		int wcount; 
		printf("Sending: %s\n", conv);
   
		wcount = write(fd, conv,4);
		if (wcount < 0) 
		{
			printf("Write Error");      
		}
		else 
		{
			printf("Sent %d characters\n", wcount);
		}
		usleep(10000); 

		close(fd);
	}
	else
	{
		printf("waiting for read");
	}

  }
return 0;


}





