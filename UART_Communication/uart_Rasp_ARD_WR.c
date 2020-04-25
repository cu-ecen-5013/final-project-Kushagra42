//REference 


#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>   // using the termios.h library

#define BUFFER_SIZE 100

//Raspi UART Function
 float ARD_uart()
{
   int file, count;
   int store_int;
   float ARD_Temp;
/********************************************open file**********************************/
   if ((file = open("/dev/ttyO4", O_RDWR | O_NOCTTY | O_NDELAY))<0){
      perror("UART: Failed to open the file.\n");
      return -1;
   }
   struct termios options;               //The termios structure is vital
   tcgetattr(file, &options);            //Sets the parameters associated with file

   // Set up the communications options:
   //   9600 baud, 8-bit, enable receiver, no modem control lines
   options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
   options.c_iflag = IGNPAR | ICRNL; 
   options.c_oflag = 0;
   options.c_lflag = 0;
   //ignore partity errors, CR -> newline
   tcflush(file, TCIFLUSH);             //discard file information not transmitted
   tcsetattr(file, TCSANOW, &options);  //changes occur immmediately

   unsigned char transmit[18] = "1";  //the string to send

/****************Sending command to arduino********************************************/
   if ((count = write(file, &transmit,18))<0){        //send the string
      perror("Failed to write to the output\n");
      return -1;
   }

/*****DATA Reading operation*****************************************************/
   fcntl(file, F_SETFL, 0);
   char receive[BUFFER_SIZE];      //declare a buffer for receiving data
   if ((count = read(file, (void*)receive, 100))<0){   //receive the data
      perror("Failed to read from the input\n");
      return -1;
   }
   if (count==0) printf("There was no data available to read!\n");
   else {
      printf("The following sensor value was read in [%d]: %s\n",count,receive);
   }
   	store_int = atoi(receive);
   	printf("converetd integer value is:%d\n",store_int);
	ARD_Temp = store_int/100.00;
	printf("Temperature is:%.2f\n",ARD_Temp);
   close(file);
   return ARD_Temp;
}



//ARDUINO UART Function
 int Rasp_uart()
{
   int file, count;
  int store_int;
   float Rasp_Temp;
/********************************************open file**********************************/
   if ((file = open("/dev/ttyO2", O_RDWR | O_NOCTTY | O_NDELAY))<0){
      perror("UART: Failed to open the file.\n");
      return -1;
   }
   struct termios options;               //The termios structure is vital
   tcgetattr(file, &options);            //Sets the parameters associated with file

   // Set up the communications options:
   //   9600 baud, 8-bit, enable receiver, no modem control lines
   options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
   options.c_iflag = IGNPAR | ICRNL; 
   options.c_oflag = 0;
   options.c_lflag = 0;
   //ignore partity errors, CR -> newline
   tcflush(file, TCIFLUSH);             //discard file information not transmitted
   tcsetattr(file, TCSANOW, &options);  //changes occur immmediately

   unsigned char transmit[18] = "1";  //the string to send

/****************Sending command to arduino********************************************/
   if ((count = write(file, &transmit,18))<0){        //send the string
      perror("Failed to write to the output\n");
      return -1;
   }

/*****DATA Reading operation*****************************************************/
   fcntl(file, F_SETFL, 0);
   char receive[BUFFER_SIZE];      //declare a buffer for receiving data
   if ((count = read(file, (void*)receive, 100))<0){   //receive the data
      perror("Failed to read from the input\n");
      return -1;
   }
   if (count==0) printf("There was no data available to read!\n");
   else {
      printf("The following sensor value was read in [%d]: %s\n",count,receive);
   }

   	store_int = atoi(receive);
   	printf("converetd integer value is:%d\n",store_int);
	Rasp_Temp = store_int/100.00;
	printf("Temperature is:%.2f\n",Rasp_Temp);
   close(file);
   return 0;
}

int main()
{
		 
	printf("Sending command 1 to BB and Arduino\n");
	float Temperature1=ARD_uart();
 	float Temperature2=Rasp_uart();
   	
	printf("Temperature value from Arduino is: %f\n", Temperature1); 
	printf("Temperature Value from Raspi is: %f\n", Temperature2);

}
