#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>   // using the termios.h library

int main(){
	printf("Entered program\n");
	int file, count;
	//Open ttyO1-UART 1 file for receiving data 
	if ((file = open("/dev/ttyO1", O_RDWR | O_NOCTTY | O_NDELAY))<0)
	{
	     perror("UART: Failed to open the file.\n");
	     return -1;
	}

   	struct termios options;               //The termios structure is vital
   	tcgetattr(file, &options);            //Sets the parameters associated with file
	printf("Initializing\n");
   	// Set up the communications options:
   	//   115200 baud, 8-bit, enable receiver, no modem control lines
   	options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;	//control options
  	options.c_iflag = IGNPAR | ICRNL;    //ignore partity errors, CR -> newline,input options
	options.c_oflag = 0;
	options.c_lflag = 0;
   	tcflush(file, TCIFLUSH);             //discard file information not transmitted	
   	tcsetattr(file, TCSANOW, &options);  //changes occur immmediately_TCSANOW
	
	//fcntl used to wait for read to occur
   	fcntl(file, F_SETFL, 0);
	
	printf("Waiting for read to happen!!!!\n");
	unsigned char receive[100];      //declare a buffer for receiving data
   	if ((count = read(file, (void*) receive,100)) < 0)
	{   
		//receive the data
	      	perror("Failed to read from the input\n");
	      	return -1;
	}

   	if (count==0) 
		printf("There was no data available to read!\n");
   	else 
	{
	      printf("The following was read in [%d]: %s\n",count,receive);
   	}
	
  	close(file);
   	return 0;
}
