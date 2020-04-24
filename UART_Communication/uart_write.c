

#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>   // using the termios.h library

int main(){
	int file, count;

	if ((file = open("/dev/ttyO4", O_RDWR | O_NOCTTY | O_NDELAY))<0)
	{
	     perror("UART: Failed to open the file.\n");
	     return -1;
	}
   	struct termios options;               //The termios structure is vital
   	tcgetattr(file, &options);            //Sets the parameters associated with file

   	// Set up the communications options:
   	//   115200 baud, 8-bit, enable receiver, no modem control lines
   	options.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
  	options.c_iflag = IGNPAR | ICRNL;    //ignore partity errors, CR -> newline
   	tcflush(file, TCIFLUSH);             //discard file information not transmitted
   	tcsetattr(file, TCSANOW, &options);  //changes occur immmediately
	
	unsigned char transmit[] = " Hello Arduino this is Beaglebone";  //the string to send

   	if ((count = write(file, &transmit,sizeof(transmit)))<0)
	{
	        //send the string
      		perror("Failed to write to the output\n");
      		return -1;
   	}
	printf("Write successful\n");
   	
	usleep(1000000);    
	close(file);
	return 0;            
}
