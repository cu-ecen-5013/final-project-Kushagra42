//REference 


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <termios.h>   // using the termios.h library
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>

#include "hard_realtime.h"

#define BUFFER_SIZE 100
#define SLAVE_ADDR 0x76
//Raspi UART Function

#define START_CHAR	(char)'1'
#define ARD_UART_PATH	"/dev/ttyO4"
#define RASP_UART_PATH	"/dev/ttyO5"

#define S_TO_NS			(uint32_t)1000000000
#define MS_TO_NS		(uint32_t)1000000
#define MS_TO_US		(uint32_t)1000

#define SELECT_R_WAIT_MS	500//

#define SYNC_TIME_S		1//m*100
#define ADDITIONAL_MS	100

#define PORT 9000    /* the port client will be connecting to */

#define MAXDATASIZE 100 /* max number of bytes we can get at once */


/************************USER MODES********************************/
#define REFERENCE_MODE
//#define STATISTIC_MODE
//#define COMPARISON_MODE
/************************USER MODES********************************/


char *msg;

int User_Modes(int sensor1,int sensor2,int sensor3)
{

#ifdef COMAPRISON_MODE

	if ((sensor1 == sensor2 && sensor1 == sensor3)||(sensor1 == sensor2 && sensor2 == sensor3))
	{
		printf("All sensor data are same %d", sensor1);
		msg = "All sensor data are same";
		return sensor1;
	}
	else if (sensor1 == sensor2 && sensor1 != sensor3)
	{
		printf("sensor1 and sensor2 data are same %d", sensor1);
		msg = "sensor1 and sensor2 data are same";
		return sensor1;
	}
	else if (sensor1 != sensor2 && sensor2 == sensor3)
	{
		printf("sensor2 and sensor3 data are same %d", sensor2);
		msg = "sensor2 and sensor3 data are same";
		return sensor2;
	}
	else if (sensor1 != sensor2 && sensor1 == sensor3)
	{
		printf("sensor1 and sensor3 data are same %d", sensor3);
		msg = "sensor1 and sensor3 data are same";
		return sensor3;
	}
	else 
	{
		printf("All are Faulty Values");
		msg = "All are Faulty Values";
		return 0;
	}
#endif

#ifdef REFERENCE_MODE
	if((sensor1 == sensor3) && (sensor2==sensor3))
	{
		printf("All sensor data is correct\n");
		msg = "All sensor data is correct";
		return sensor3;
	}
	else if((sensor1 != sensor3) && (sensor2 == sensor3))
	{
		printf("Sensor 1 data is wrong\n");
		msg = "Sensor 1 data is wrong";
		return sensor3;
	}
	else if((sensor1 == sensor3) && (sensor2 != sensor3))
	{
		printf("Sensor 2 data is wrong\n");
		msg = "Sensor 2 data is wrong";
		return sensor3;
	}
  	else if((sensor1 == sensor2) && (sensor1 != sensor3))
	{
		printf("Sensor 3 data is wrong\n");
		msg = "Sensor 3 data is wrong";
		return sensor3;
	}
	else
	{
		printf("All 3 sensors have different values\n");
		msg = "All 3 sensors have different values, Considering reference sensor";
		return sensor3;
	}



#endif

#ifdef STATISTIC_MODE

	if(((sensor1 == sensor2) && (sensor1 == sensor3))||((sensor1 == sensor2) && (sensor2 == sensor3)))
	{
		printf("Final value will be average of all 3 sensors\n");
		msg = "Final value will be average of all 3 sensors";
		int avg_temp = (sensor1 + sensor2 + sensor3)/3;
		return avg_temp;
	}
	else if((sensor1 == sensor2+1 || sensor1 == sensor2-1) && (sensor1 == sensor3+2 || sensor1 == sensor3-2) && (sensor2 == sensor3+1 || sensor3 == sensor3-1))
	{
		printf("All sensor values are in range\n");
		msg = "All sensor values are in range";
		int avg_temp = (sensor1 + sensor2 + sensor3)/3;
		return avg_temp;
	}
	else if((sensor1 == sensor2+1 || sensor1 ==  sensor2-1) && (sensor1 != sensor3+2 || sensor1 != sensor3-2) && (sensor2 != sensor3+1 || sensor3 != sensor3-1))
	{
		printf("Sensor 3 is not in range\n");
		msg = "Sensor 3 is not in range";
		int avg_temp = (sensor1 + sensor2)/2;
		return avg_temp;
	}
	else if((sensor1 != sensor2+1 || sensor1 != sensor2-1) && (sensor1 == sensor3+1 || sensor1 == sensor3-1) && (sensor2 != sensor3+1 || sensor3 != sensor3-1))
	{
		printf("Sensor 2 is not in range\n");
		msg = "Sensor 2 is not in range";
		int avg_temp = (sensor1 + sensor3)/2;
		return avg_temp;
	}
	else if((sensor1 != sensor2+1 || sensor1 != sensor2-1) && (sensor1 != sensor3+1 || sensor1 != sensor3-1) && (sensor2 == sensor3+1 || sensor3 == sensor3-1))
	{
		printf("Sensor 1 is not in range\n");
		msg = "Sensor 1 is not in range";
		int avg_temp = (sensor3 + sensor2)/2;
		return avg_temp;
	}
	else
	{
		printf("All are Faulty Values");
		msg = "All are Faulty Values";
		return 0;
	}

#endif
}


int new_socket;

void Socket_Init()
{
	char target_ip[20] = "192.168.50.104";
	uint32_t Port_Num = 9000;
	struct sockaddr_in client;

	
	new_socket = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
	if(new_socket < 0)		printf("Socket Creation Failed\n");
	
	client.sin_family = AF_INET;
	if(inet_pton(AF_INET, target_ip, &client.sin_addr) <= 0)	printf("Error in IP Address\n");
	client.sin_port = htons(Port_Num);
	
	if(connect(new_socket, (struct sockaddr *)&client, sizeof(client)) == 0)	printf("Connect Succeeded\n");
	else	printf("Connect Failed\n");
	
	//send data

}
int Client_Data(char *str, uint32_t len)
{

   
    if (send(new_socket, str, len, 0) == -1)
     {
         perror("send");
         exit (1);
     }
     printf("After the send function \n");

     return 0;
   
}

// return false if error - true if pass
bool UART_send_cmd(int *file)//o
{
	char cmd = START_CHAR;//

	hr_monitor_fd(file, 0, HR_WATCH_FOR_WRITE_MASK);
	
	
	if(write(*file, &cmd, sizeof(cmd)) != sizeof(cmd))
	{
		perror("UART_send_cmd failed\n");
		return false;
	}
	
	return true;
}

bool UART_receive_temp(int *file, float *temp)
{	
	char receive[BUFFER_SIZE];      //declare a buffer for receiving data
	uint8_t	count;	
	int	temperature;

	hr_monitor_fd(file, SELECT_R_WAIT_MS, HR_WATCH_FOR_READ_MASK);
	
	count = read(*file, (void*)&receive[0], BUFFER_SIZE);	//receive the data
	
	if(count < 0)
	{   
		perror("UART ARD receive failed\n");
		*temp = 0;
		return false;
	}
	
	if(count == 0) printf("UART no data to read\n");
	else 
	{
		temperature = atoi(receive);
		printf("UART got integer temp: %d\n",temperature);
		*temp = ((float)temperature)/100.00;
	}
	
	return true;
}

bool UART_periph_init(int *file, char *path)
{
	/********************************************open file**********************************/
	*file = open(path, O_RDWR | O_NOCTTY | O_NDELAY);
	if(*file < 0)
	{
		perror("UART_periph_init failed\n");
		return false;
	}
	
	struct termios options;               //The termios structure is vital
        tcgetattr(*file, &options);            //Sets the parameters associated with file

	// Set up the communications options:
	//   9600 baud, 8-bit, enable receiver, no modem control lines
	options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
	options.c_iflag = IGNPAR | ICRNL; 
	options.c_oflag = 0;
	options.c_lflag = 0;
	
	//ignore partity errors, CR -> newline
	tcflush(*file, TCIFLUSH);             //discard file information not transmitted
	tcsetattr(*file, TCSANOW, &options);  //changes occur immmediately
	
	return true;
}

void UART_periph_close(int *file)
{
	close(*file);
}

float Get_Temperature()
{

  int fd;
  char *bus = "/dev/i2c-1"; 
  int addr = SLAVE_ADDR;    
  char buf[2] = {0};
  uint32_t temp;
  uint32_t MSB, LSB, XLSB;

  float f,c;

  if ((fd = open(bus, O_RDWR)) < 0) {
    
    perror("Failed to open the i2c bus");
    exit(1);
  }

 if (ioctl(fd, I2C_SLAVE, addr) < 0) {
   perror("Failed to acquire bus access and/or talk to slave.\n");
  
   exit(1);
 }
 
     if (read(fd,buf,2) != 2) {
      
       perror("Failed to read from the i2c bus.\n");

   } else {

       MSB = (uint32_t)buf[3] << 12;
       LSB = (uint32_t)buf[4] << 4;
       XLSB = (uint32_t)buf[5] >> 4;
       temp = MSB | LSB | XLSB;
       printf("raw i2c daat is :%u",temp);

       c = temp*0.0625;
       f = (1.8 * c) + 32;

       printf("Temp Fahrenheit: %f Celsius: %f\n", f, c);
     }
     
     close(fd);

return f;

}

int main(int argc, char *argv[])
{
	
	int ARD_file, RASP_file;// resp;
	uint8_t i;
	float LOCAL_temp, ARD_temp, RASP_temp;
	int I2C_Sensor, ARD_Sensor, Raspi_Sensor;
	struct timespec prev_t;
	//uint32_t sleep_time;
	char client_msg[200];
	
	
	
	if(UART_periph_init(&ARD_file, ARD_UART_PATH) == false)		printf("UART for ARD failed to Initialize... Path: %s\n", ARD_UART_PATH);
	
	if(UART_periph_init(&RASP_file, RASP_UART_PATH) == false)		printf("UART for RASP failed to Initialize... Path: %s\n", RASP_UART_PATH);
	
        Socket_Init();

	for(i = 0; i < 10; i ++)
	{
		
		// Add a fix offset to previous absoulte time
		prev_t.tv_sec = prev_t.tv_sec + SYNC_TIME_S;

		hr_dynamic_time_buffer_wait(SYNC_TIME_S*100);
		// Dynamic Time Buffer Start....
		
		
		if(UART_send_cmd(&ARD_file) == false)	printf("UART for ARD failed to Send... Path: %s\n", ARD_UART_PATH);
		if(UART_send_cmd(&RASP_file) == false)	printf("UART for RASP failed to Send... Path: %s\n", RASP_UART_PATH);
		
		LOCAL_temp = Get_Temperature();
		
		if(UART_receive_temp(&ARD_file, &ARD_temp) == false)	printf("UART for ARD failed to Read... Path: %s\n", ARD_UART_PATH);

		if(UART_receive_temp(&RASP_file, &RASP_temp) == false)	printf("UART for RASP failed to Read... Path: %s\n", RASP_UART_PATH);
		
		// *********** TEMPERATURE COMPARING ***********
		
		
		// *********** SOCKET SENDING HERE ***********
		printf("Ard Temp: %.2f\nRasp Temp: %.2f\nLocal Temp: %.2f\n",ARD_temp,RASP_temp,LOCAL_temp);	// CHANGE TO SYSLOG
		snprintf(&client_msg[0], 200, "Ard Temp: %.2f\nRasp Temp: %.2f\nLocal Temp: %.2f\n",ARD_temp, RASP_temp, LOCAL_temp);
		Client_Data(&client_msg[0], 200);
	
		I2C_Sensor = LOCAL_temp;
		ARD_Sensor = ARD_temp;
		Raspi_Sensor = RASP_temp;
		
		int Sensor_Selected_Value = User_Modes(I2C_Sensor,ARD_Sensor,Raspi_Sensor);
		
		//***********Sending Comparison Analysis data ove socket******
		printf("I2C_Sensor: %.2d\nARD_Sensor: %.2d\nRaspi_Sensor: %.2d\n",I2C_Sensor,ARD_Sensor,Raspi_Sensor);	// CHANGE TO SYSLOG
		snprintf(&client_msg[0], 200, "Sensor_Selected_Value: %.2d\n",Sensor_Selected_Value);
		Client_Data(&client_msg[0], 200);

		snprintf(&client_msg[0], 200, "Sensor Fault detection message: %.2s\n",msg);
		Client_Data(&client_msg[0], 200);
		// Dynamic Time Buffer End....
	}
	    	
	snprintf(&client_msg[0], 200, "EXIT");
	Client_Data(&client_msg[0], 200);

   	close(new_socket);

	UART_periph_close(&ARD_file);
	UART_periph_close(&RASP_file);
	
	printf("Exiting...");
	exit(0);
return 0;
}

