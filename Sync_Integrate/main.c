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

#include <linux/i2c-dev.h>
#include "hard_realtime.h"

#define BUFFER_SIZE 100
#define SLAVE_ADDR 0x76
//Raspi UART Function

#define START_CHAR	(char)'1'
#define ARD_UART_PATH	"/dev/ttyO4"
#define RASP_UART_PATH	"/dev/ttyO5"

#define SELECT_R_WAIT_MS	300//

#define SYNC_TIME_S		1//m*100
#define ADDITIONAL_MS	100

#define PORT 9000    /* the port client will be connecting to */

#define MAXDATASIZE 100 /* max number of bytes we can get at once */
#define DELAY 1000000

/************************USER MODES********************************/
//#define REFERENCE_MODE
//#define STATISTIC_MODE
//#define COMPARISON_MODE
/************************USER MODES********************************/

//#define 	DELTA_MIN	0.5

/*

mode 1 - compare
mode 2 - ref with other 2 - comapre with fixed range acceptance
mode 3 - closest 2 


*/

//float val1, val2, val3;
//
//if((val1 >= (val2 + DELTA)) || (val1 <= (val2 - DELTA)))	

char msg[200];
static int file_i2c = 0;
// Sensor calibration data
static int calT1,calT2,calT3;


int bme280Init(int iChannel, int iAddr)
{
int i, rc;
unsigned char ucTemp[32];
unsigned char ucCal[36];
char filename[32];
 
	sprintf(filename, "/dev/i2c-%d", iChannel);
	if ((file_i2c = open(filename, O_RDWR)) < 0)
	{
		fprintf(stderr, "Failed to open the i2c bus\n");
		file_i2c = 0;
		return -1;
	}

	if (ioctl(file_i2c, I2C_SLAVE, iAddr) < 0)
	{
		fprintf(stderr, "Failed to acquire bus access or talk to slave\n");
		file_i2c = 0;
		return -1;
	}

	ucTemp[0] = 0xd0; // get ID
	rc = write(file_i2c, ucTemp, 1);
	i = read(file_i2c, ucTemp, 1);
	if (rc < 0 || i != 1 || ucTemp[0] != 0x60)
	{
		printf("Error, ID doesn't match 0x60; wrong device?\n");
		return -1;
	}
	// Read 24 bytes of calibration data
	ucTemp[0] = 0x88; // starting 4from register 0x88
	rc = write(file_i2c, ucTemp, 1);
	i = read(file_i2c, ucCal, 24);
	if (rc < 0 || i != 24)
	{
		printf("calibration data not read correctly\n");
		return -1;
	}
	ucTemp[0] = 0xa1; // get humidity calibration byte
	rc = write(file_i2c, ucTemp, 1);
	i = read(file_i2c, &ucCal[24], 1);
	ucTemp[0] = 0xe1; // get 7 more humidity calibration bytes
	rc = write(file_i2c, ucTemp, 1);
	i = read(file_i2c, &ucCal[25], 7);
	if (rc < 0 || i < 0) // something went wrong
	{}
	// Prepare temperature calibration data
	calT1 = ucCal[0] + (ucCal[1] << 8);
	calT2 = ucCal[2] + (ucCal[3] << 8);
	if (calT2 > 32767) calT2 -= 65536; // negative value
	calT3 = ucCal[4] + (ucCal[5] << 8);
	if (calT3 > 32767) calT3 -= 65536;

	ucTemp[0] = 0xf4; // control measurement register
	ucTemp[1] = 0x27; // normal mode, temp over sampling rate=1
	rc = write(file_i2c, ucTemp, 2);

	ucTemp[0] = 0xf5; // CONFIG
	ucTemp[1] = 0xa0; // set stand by time to 1 second
	rc = write(file_i2c, ucTemp, 2);
	if (rc < 0) {} // suppress warning

	return 0;

} 

float bme280ReadValues()
{
unsigned char ucTemp[16];
int i,rc;
int t; // raw sensor values
float temp;
int var1,var2,t_fine, temp_shift;



	ucTemp[0] = 0xf7; // start of data registers we want
	rc = write(file_i2c, ucTemp, 1); // write address of register to read
	i = read(file_i2c, ucTemp, 8);
	if (rc < 0 || i != 8)
	{
		return -1; // something went wrong
	}
	t = (ucTemp[3] << 12) + (ucTemp[4] << 4) + (ucTemp[5] >> 4);

	var1 = ((((t >> 3) - (calT1 <<1))) * (calT2)) >> 11;
	var2 = (((((t >> 4) - (calT1)) * ((t>>4) - (calT1))) >> 12) * (calT3)) >> 14;
	t_fine = var1 + var2;
	temp_shift = (t_fine * 5 + 128) >> 8;
	temp = temp_shift / 100;
	temp *= 1.8;
	temp += 32;
	
	return temp;
} 

int User_Modes(int sensor1,int sensor2,int sensor3,char *user_mode)
{

int argument = atoi(user_mode);
if(argument == 0)
{
	if ((sensor1 == sensor2 && sensor1 == sensor3)||(sensor1 == sensor2 && sensor2 == sensor3))
	{
		sprintf(msg, "All sensor data are same %d\n", sensor1);
		printf("%s", msg);
		return sensor1;
	}
	else if (sensor1 == sensor2 && sensor1 != sensor3)
	{
		sprintf(msg, "sensor1 and sensor2 data are same %d\n", sensor1);
		printf("%s", msg);
		return sensor1;
	}
	else if (sensor1 != sensor2 && sensor2 == sensor3)
	{
		sprintf(msg, "sensor2 and sensor3 data are same %d\n", sensor2);
		printf("%s", msg);
		return sensor2;
	}
	else if (sensor1 != sensor2 && sensor1 == sensor3)
	{
		sprintf(msg, "sensor1 and sensor3 data are same %d\n", sensor3);
		printf("%s", msg);
		return sensor3;
	}
	else 
	{
		sprintf(msg, "All are Faulty Values\n");
		printf("%s", msg);
		return 0;
	}
}


if(argument == 1)
{

	if((sensor1 == sensor3) && (sensor2==sensor3))
	{
		sprintf(msg, "All sensor data is correct\n");
		printf("%s", msg);
		return sensor3;
	}
	else if((sensor1 != sensor3) && (sensor2 == sensor3))
	{
		sprintf(msg, "Sensor 1 data is wrong\n");
		printf("%s", msg);
		return sensor3;
	}
	else if((sensor1 == sensor3) && (sensor2 != sensor3))
	{
		sprintf(msg, "Sensor 2 data is wrong\n");
		printf("%s", msg);
		return sensor3;
	}
  	else if((sensor1 == sensor2) && (sensor1 != sensor3))
	{
		sprintf(msg, "Sensor 3 data is wrong\n");
		printf("%s", msg);
		return sensor3;
	}
	else
	{
		sprintf(msg, "All 3 sensors have different values, Considering reference sensor\n");
		printf("%s", msg);
		return sensor3;
	}

}


if(argument == 2)
{

	if(((sensor1 == sensor2) && (sensor1 == sensor3))||((sensor1 == sensor2) && (sensor2 == sensor3)))
	{
		sprintf(msg, "Final value will be average of all 3 sensors\n");
		printf("%s", msg);
		int avg_temp = (sensor1 + sensor2 + sensor3)/3;
		return avg_temp;
	}
	else if((sensor1 == sensor2+1 || sensor1 == sensor2-1) && (sensor1 == sensor3+2 || sensor1 == sensor3-2) && (sensor2 == sensor3+1 || sensor3 == sensor3-1))
	{
		sprintf(msg, "All sensor values are in range\n");
		printf("%s", msg);
		int avg_temp = (sensor1 + sensor2 + sensor3)/3;
		return avg_temp;
	}
	else if((sensor1 == sensor2+1 || sensor1 ==  sensor2-1) && (sensor1 != sensor3+2 || sensor1 != sensor3-2) && (sensor2 != sensor3+1 || sensor3 != sensor3-1))
	{
		sprintf(msg, "Sensor 3 is not in range\n");
		printf("%s", msg);
		int avg_temp = (sensor1 + sensor2)/2;
		return avg_temp;
	}
	else if((sensor1 != sensor2+1 || sensor1 != sensor2-1) && (sensor1 == sensor3+1 || sensor1 == sensor3-1) && (sensor2 != sensor3+1 || sensor3 != sensor3-1))
	{
		sprintf(msg, "Sensor 2 is not in range\n");
		printf("%s", msg);
		int avg_temp = (sensor1 + sensor3)/2;
		return avg_temp;
	}
	else if((sensor1 != sensor2+1 || sensor1 != sensor2-1) && (sensor1 != sensor3+1 || sensor1 != sensor3-1) && (sensor2 == sensor3+1 || sensor3 == sensor3-1))
	{
		sprintf(msg, "Sensor 1 is not in range\n");
		printf("%s", msg);
		int avg_temp = (sensor3 + sensor2)/2;
		return avg_temp;
	}
	else
	{
		sprintf(msg, "All are Faulty Values\n");
		printf("%s", msg);
		return 0;
	}
}

return 0;
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
	
	//char buf[500];
	//memset(buf,0,500);
	//strncpy(buf, str, len);
	uint32_t cnt = 0;
	uint32_t resp = 0;

   do{
		resp = send(new_socket, str, len, 0);
		if(resp < 0)
		{
			perror("send\n");
			return -1;
		}
		cnt += resp;
   } while(cnt < len);
       

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

int main(int argc, char *argv[])
{
	char rasp_msg[20], ard_msg[20], local_msg[20];
	int ARD_file, RASP_file;
	uint8_t i;
	float LOCAL_temp=0, ARD_temp, RASP_temp=0;
	int I2C_Sensor, ARD_Sensor, Raspi_Sensor;
	int tmp;
	char client_msg[500];
	
	int j;

/*********************Checking if argument was passed**************************/

if(argc < 1)
{
	printf("PLease give the USER MODE 0:Comparison mode 1:Reference mode 2:Statistic mode\n");
}
else
{
/*********************BME280***********************/
	j = bme280Init(1, 0x76);
	if (j != 0)
	{
		return -1; // problem - quit
	}
	printf("BME280 device successfully opened.\n");
	usleep(1000000); // wait for data to settle for first read
	
/*********************BME280***********************/
	
	if(UART_periph_init(&ARD_file, ARD_UART_PATH) == false)		printf("UART for ARD failed to Initialize... Path: %s\n", ARD_UART_PATH);
	
	if(UART_periph_init(&RASP_file, RASP_UART_PATH) == false)	printf("UART for RASP failed to Initialize... Path: %s\n", RASP_UART_PATH);
			
	Socket_Init();

	for(i = 0; i < 5; i++)
	{
		
				
		hr_dynamic_time_buffer_wait(SYNC_TIME_S*1000);
		// Dynamic Time Buffer Start....
		
		if(UART_send_cmd(&ARD_file) == false)	printf("UART for ARD failed to Send... Path: %s\n", ARD_UART_PATH);
		if(UART_send_cmd(&RASP_file) == false)	printf("UART for RASP failed to Send... Path: %s\n", RASP_UART_PATH);
		LOCAL_temp = bme280ReadValues();
	        
			
		if(UART_receive_temp(&ARD_file, &ARD_temp) == false)	printf("UART for ARD failed to Read... Path: %s\n", ARD_UART_PATH);
		
		if(UART_receive_temp(&RASP_file, &RASP_temp) == false)	printf("UART for RASP failed to Read... Path: %s\n", RASP_UART_PATH);
		
		// *********** TEMPERATURE COMPARING ***********
		
		
		// *********** SOCKET SENDING HERE ***********
		printf("Ard Temp: %.2f\nRasp Temp: %.2f\nLocal Temp: %.2f\n",ARD_temp,RASP_temp,LOCAL_temp);	// CHANGE TO SYSLOG
	
		I2C_Sensor = LOCAL_temp;
		ARD_Sensor = ARD_temp;
		Raspi_Sensor = RASP_temp;
		
		//int Sensor_Selected_Value = User_Modes(I2C_Sensor,ARD_Sensor,Raspi_Sensor);
		User_Modes(I2C_Sensor,ARD_Sensor,Raspi_Sensor,*argv);

		//***********Sending Comparison Analysis data ove socket******
		tmp = (int)(RASP_temp) % 100;
		snprintf(&rasp_msg[0], sizeof(rasp_msg), "Rasp: %d.%d\n", tmp, (int)((RASP_temp - tmp) * 100));
		
		tmp = (int)(ARD_temp) % 100;
		snprintf(&ard_msg[0], sizeof(ard_msg), "Ard: %d.%d\n", tmp, (int)((ARD_temp - tmp) * 100));
		
		tmp = (int)(LOCAL_temp) % 100;
		snprintf(&local_msg[0], sizeof(local_msg), "Local: %d.%d\n", tmp, (int)((LOCAL_temp - tmp) * 100));

		snprintf(&client_msg[0], sizeof(client_msg), "%s%s%s%s\n\n", rasp_msg, ard_msg, local_msg, msg);
		Client_Data(&client_msg[0], strlen(client_msg));
		//Dynamic Time Buffer End....
	}
	    	
	snprintf(&client_msg[0], 200, "EXIT");
	
	Client_Data(&client_msg[0], strlen(client_msg));

   	close(new_socket);

	UART_periph_close(&ARD_file);
	UART_periph_close(&RASP_file);
	
	printf("Exiting...");
	exit(0);
}
return 0;


}


