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


#include <string.h>
#include <unistd.h>

#include <termios.h>
//#include "Uart_raspi.h"


#define Si7021_address 0x40
int beaglebone=1;
float cTemp;


int uart(char tx_buffer[20])
{
  int fd;
  // Open the Port. We want read/write, no "controlling tty" status, and open it no matter what state DCD is in
  fd = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1)
  {
    perror("open_port: Unable to open /dev/ttyAMA0 - ");
    return(-1);
  }

  // Turn off blocking for reads, use (fd, F_SETFL, FNDELAY) if you want that
  fcntl(fd, F_SETFL, 0);


  struct termios options;
  tcgetattr(fd, &options);

    int baud_in=cfsetispeed(&options, B9600);
  if (baud_in == -1)
  {
    perror("Error in setting input required baud rate\n ");
    return(-1);
  }
  int baud_out=cfsetospeed(&options, B9600);
  if (baud_out==-1)
  {
    perror("Error in setting output required baud rate\n ");
    return(-1);
  }
      /* input flags */
    options.c_iflag &= ~ IGNBRK;                       /* enable ignoring break */
    options.c_iflag &= ~(IGNPAR | PARMRK);             /* disable parity checks */
    options.c_iflag &= ~ INPCK;                        /* disable parity checking */
    options.c_iflag &= ~ ISTRIP;                       /* disable stripping 8th bit */
    options.c_iflag &= ~(INLCR | ICRNL);               /* disable translating NL <-> CR */
    options.c_iflag &= ~ IGNCR;                        /* disable ignoring CR */
    options.c_iflag &= ~(IXON | IXOFF);                /* disable XON/XOFF flow control */
    /* output flags */
    options.c_oflag  &= ~ OPOST;                        /* disable output processing */
    options.c_oflag  &= ~(ONLCR | OCRNL);               /* disable translating NL <-> CR */
    /* not for FreeBSD */
    options.c_oflag &= ~ OFILL;                        /* disable fill characters */
    /* control flags */
    options.c_cflag  |=   CLOCAL;                       /* prevent changing ownership */
    options.c_cflag  |=   CREAD;                        /* enable reciever */
    options.c_cflag  &= ~ PARENB;                       /* disable parity */
    options.c_cflag  &= ~ CSTOPB;                       /* disable 2 stop bits */
    options.c_cflag  &= ~ CSIZE;                        /* remove size flag... */
    options.c_cflag  |=   CS8;                          /* ...enable 8 bit characters */
    options.c_cflag  |=   HUPCL;                        /* enable lower control lines on close - hang up */
    options.c_cflag &= ~CRTSCTS;	

        /* local flags */
    options.c_lflag &= ~ ISIG;                         /* disable generating signals */
    options.c_lflag &= ~ ICANON;                       /* disable canonical mode - line by line */
    options.c_lflag &= ~ ECHO;                         /* disable echoing characters */
    options.c_lflag &= ~ ECHONL;                       /* ??? */
    options.c_lflag &= ~ NOFLSH;                       /* disable flushing on SIGINT */
    options.c_lflag &= ~ IEXTEN;                       /* disable input processing */

        /* control characters */
    memset(options.c_cc,0,sizeof(options.c_cc));
/*
  int baud_in=cfsetispeed(&options, B115200);
  if (baud_in == -1)
  {
    perror("Error in setting input required baud rate\n ");
    return(-1);
  }
  int baud_out=cfsetospeed(&options, B115200);
  if (baud_out==-1)
  {
    perror("Error in setting output required baud rate\n ");
    return(-1);
  }
*/  

//  tcsetattr(fd, TCSANOW, &options);
  tcsetattr(fd, TCSAFLUSH, &options);
      /* enable input & output transmission */
 //   tcflow(fd, TCOON | TCION);
  // Write to the port
//  unsigned char tx_buffer[20]= "Hello Aesd";
//  unsigned char *p_tx_buffer;
  printf("Testing uart ");	
//  p_tx_buffer = &tx_buffer[0];
//  *p_tx_buffer++ = 'H';
//  *p_tx_buffer++ = 'e';
//  *p_tx_buffer++ = 'l';
//  *p_tx_buffer++ = 'l';
//  *p_tx_buffer++ = 'o';
//size_t count = strlen(tx_buffer);
//  int n = write("Hi testing uart functionality for raspi\n");
  int n = write(fd, &tx_buffer, 11);		//Filestream, bytes to write, number of bytes to write
//  int n = write(fd,&tx_buffer[0],(p_tx_buffer - &tx_buffer[0]));
  if (n < 0) 
  {
    perror("Write failure ");
    return -1;
  }
  
  usleep(10000);
  // Read up to 255 characters from the port if they are there
  char buf[256];
  int r= read(fd, (void*)buf, n);
  if (r < 0) 
  {
    perror("Read failure ");
    return -1;
  }
  else if (r == 0) 
	printf("No data on port\n");
  else 
  {
    buf[r] = '\0';
    printf("%i bytes read : %s\n", r, buf);
  }

  // Don't forget to clean up
  close(fd);
  printf("End of uart test");
  return 0;
//  return 0;
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
			cTemp = (((data[0] * 256 + data[1]) * 175.72) / 65536.0) - 46.85;
			float fTemp = cTemp * 1.8 + 32;

			// Output data to screen
			printf("TIME:%s    Temperature in Celsius : %.2f C \n",time_buffer, cTemp);
			printf("TIME:%s    Temperature in Fahrenheit : %.2f F \n",time_buffer, fTemp);
		}
  
    char tx_buffer1[20];

 
    sprintf(tx_buffer1, "%f",cTemp);
    int return_uart=uart(tx_buffer1);
    if(return_uart==0)
	{
		printf("Uart successful");
//		return 0;

	}
   else 
	{
		printf("Uart error");
		return -1;

	}
		}
	}
return 0;

}


