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
int fd;
float cTemp;
float fTemp;
int rcount;

char conv[20];

int send_temp;





int uart_read()
{

    struct termios serial;
   
    char buffer[10];


//    printf("Opening  /dev/ttyS0");

    fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);

    if (fd == -1) 
    {
	 printf("OPEN READ ERROR");

    }

    if (tcgetattr(fd, &serial) < 0) 
    {
        perror("Getting configuration");
        return -1;
    }
serial.c_cflag = B115200 | CS8 | CREAD;
    // Set up Serial Configuration
    serial.c_iflag = IGNPAR|ICRNL;
    serial.c_oflag = 0;
    serial.c_lflag = 0;
    serial.c_cflag = 0;

    

   tcsetattr(fd, TCSANOW, &serial); // Apply configuration

      fcntl(fd, F_SETFL, 0);

     rcount = read(fd, buffer, 10);

    if (rcount < 0)
   {
        printf("Read error");
        return 5;

   }
    else 
    {
   //     printf("Received %d characters\n", rcount);
//	    printf("Received: %s\n", buffer);
    buffer[rcount] = '\0';

  //  printf("Received: %s\n", buffer);

    return 10;
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


//			usleep(20000);
			// Send temperature measurement command, NO HOLD MASTER(0xF3)
			char config[1] = {0xF3};
			write(file, config, 1); 
		    sleep(1);

			// Read 2 bytes of temperature data
			// temp msb, temp lsb

    			printf("\nTemp Reading data \n");
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
			    	fTemp = cTemp * 1.8 + 32;

				// Output data to screen
				printf("TIME:%s  \n  Temperature in Celsius : %.2f C \n",time_buffer, cTemp);
				printf("\n  Temperature in Fahrenheit : %.2f F \n", fTemp);
			}
  			printf("\nstarting uart\n");
    


   			send_temp=fTemp*100;

   			sprintf(conv,"%d",send_temp);


                struct termios serial;
   
//    char buffer[10];


//    printf("Opening  /dev/ttyS0");

    fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);

    if (fd == -1) 
    {
	 printf("OPEN READ ERROR");

    }

    if (tcgetattr(fd, &serial) < 0) 
    {
        perror("Getting configuration");
        return -1;
    }
serial.c_cflag = B115200 | CS8 | CREAD|CLOCAL;
    // Set up Serial Configuration
    serial.c_iflag = IGNPAR|ICRNL;

/////////////////////////////////
 //   serial.c_oflag = 0;
 //   serial.c_lflag = 0;
 //   serial.c_cflag = 0;
 //   serial.c_cc[VMIN] = 0;
  //  serial.c_cc[VTIME] = 0;
/////////////////////////////////


     tcflush(fd, TCIFLUSH);             //discard file information not transmitted

   tcsetattr(fd, TCSANOW, &serial); // Apply configuration

  


    // Attempt to send and receive

    int wcount; 
    printf("Sending: %s\n", conv);
   
           wcount = write(fd, conv, 10);
    if (wcount < 0) 
    {
        printf("Write Error");
      
    }
    else 
    {
        printf("Sent %d characters\n", wcount);
    }
//	usleep(10000);

    close(fd);




 


}
return 0;


}





