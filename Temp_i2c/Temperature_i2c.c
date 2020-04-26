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
//#include "Uart_raspi.h"
#define BUFFER_SIZE 100 

#include <string.h>
#include <unistd.h>

#include <termios.h>
//#include "Uart_raspi.h"


#define Si7021_address 0x40
int beaglebone=1;
float cTemp;

char conv[20]= {0};

int send_temp;


void uart_write( char tx_buffer)
{
  int file, count;

	if ((file = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY))<0)
	{
	     printf("UART: Failed to open the file.\n");
	     
	}
   	struct termios options;               //The termios structure is vital
   	tcgetattr(file, &options);            //Sets the parameters associated with file

   	// Set up the communications options:
   	//   115200 baud, 8-bit, enable receiver, no modem control lines
   	options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
  	options.c_iflag = IGNPAR | ICRNL;    //ignore partity errors, CR -> newline
   	tcflush(file, TCIFLUSH);             //discard file information not transmitted
   	tcsetattr(file, TCSANOW, &options);  //changes occur immmediately

   	if ((count = write(file, &tx_buffer,6)<0))
	  {
	        //send the string
      		printf("Failed to write to the output\n");
      		
   	}
	printf("Write successful\n");
;    
	close(file);

}
int uart_read()
{
  int file,count;
/********************************************open file**********************************/
   if ((file = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY))<0)
   {
      perror("UART: Failed to open the file.\n");
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
   //ignore partity errors, CR -> newline
   tcflush(file, TCIFLUSH);             //discard file information not transmitted
   tcsetattr(file, TCSANOW, &options);  //changes occur immmediately

   fcntl(file, F_SETFL, 0);
   unsigned char receive[BUFFER_SIZE];      //declare a buffer for receiving data
   if ((count = read(file, (void*)receive, 100))<0){   //receive the datam
      perror("Failed to read from the input\n");
      return -1;
   }
   if (count==0)
   { 
        printf("There was no data available to read!\n");
        return 0;
   } 
   else 
   {
     
      printf("The following sensor value was read in [%d]: %s\n",count,receive);
      close(file);
      return 1;
   }
 
  
}

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

	while(1)
	{

		int uart_return=uart_read();

		if (uart_return==1)

		{
			usleep(20000);
			// Send temperature measurement command, NO HOLD MASTER(0xF3)
			char config[1] = {0xF3};
			write(file, config, 1); 
		

			// Read 2 bytes of temperature data
			// temp msb, temp lsb

    			printf("REading data \n");
			char data[2] = {0};

			if(read(file, data, 2) != 2)
			{
     		 		time( &time_current );
	
				local = localtime( &time_current );
	
				time_buffer = asctime(local);
				printf("TIME:%s    Error : Input/Output error \n",time_buffer);
			}
			else
			{
				time( &time_current );
	
				local = localtime( &time_current );
	
				time_buffer = asctime(local);
				// Convert the data
				cTemp = (((data[0] * 256 + data[1]) * 175.72) / 65536.0) - 46.85;
				float fTemp = cTemp * 1.8 + 32;

				// Output data to screen
				printf("TIME:%s    Temperature in Celsius : %.2f C \n",time_buffer, cTemp);
				printf("TIME:%s    Temperature in Fahrenheit : %.2f F \n",time_buffer, fTemp);
			}
  			printf("\nstarting uart\n");
    


    			send_temp=cTemp*100;

    			sprintf(conv,"%d",send_temp);
        		for(int i = 0; i<3; i++)
        		{
				uart_write(conv[i]); 
       	 		}
	
		}
		else
		{
			printf("Waiting for beaglebone to respond \n");
		}

	}
return 0;

}


