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

#define BUFFER_SIZE 100
#define SLAVE_ADDR 0x76
//Raspi UART Function

#define START_CHAR		(char)'1'
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
#define DELAY 1000000

/************************USER MODES********************************/
#define REFERENCE_MODE
//#define STATISTIC_MODE
//#define COMPARISON_MODE
/************************USER MODES********************************/


char *msg;
static int file_i2c = 0;
// Sensor calibration data
static int calT1,calT2,calT3;
static int calP1, calP2, calP3, calP4, calP5, calP6, calP7, calP8, calP9;
static int calH1, calH2, calH3, calH4, calH5, calH6;

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
	
	// Prepare pressure calibration data
	calP1 = ucCal[6] + (ucCal[7] << 8);
	calP2 = ucCal[8] + (ucCal[9] << 8);
	if (calP2 > 32767) calP2 -= 65536; // signed short
	calP3 = ucCal[10] + (ucCal[11] << 8);
	if (calP3 > 32767) calP3 -= 65536;
	calP4 = ucCal[12] + (ucCal[13] << 8);
	if (calP4 > 32767) calP4 -= 65536;
	calP5 = ucCal[14] + (ucCal[15] << 8);
	if (calP5 > 32767) calP5 -= 65536;
	calP6 = ucCal[16] + (ucCal[17] << 8);
	if (calP6 > 32767) calP6 -= 65536;
	calP7 = ucCal[18] + (ucCal[19] << 8);
	if (calP7 > 32767) calP7 -= 65536;
	calP8 = ucCal[20] + (ucCal[21] << 8);
	if (calP8 > 32767) calP8 -= 65536;
	calP9 = ucCal[22] + (ucCal[23] << 8);
	if (calP9 > 32767) calP9 -= 65536;

	// Prepare humidity calibration data
	calH1 = ucCal[24];
	calH2 = ucCal[25] + (ucCal[26] << 8);
	if (calH2 > 32767) calH2 -= 65536;
	calH3 = ucCal[27];
	calH4 = (ucCal[28] << 4) + (ucCal[29] & 0xf);
	if (calH4 > 2047) calH4 -= 4096; // signed 12-bit
	calH5 = (ucCal[30] << 4) + (ucCal[29] >> 4);
	if (calH5 > 2047) calH5 -= 4096;
	calH6 = ucCal[31];
	if (calH6 > 127) calH6 -= 256; // signed char

	ucTemp[0] = 0xf2; // control humidity register
	ucTemp[1] = 0x01; // humidity over sampling rate = 1
	rc = write(file_i2c, ucTemp, 2);

	ucTemp[0] = 0xf4; // control measurement register
	ucTemp[1] = 0x27; // normal mode, temp and pressure over sampling rate=1
	rc = write(file_i2c, ucTemp, 2);

	ucTemp[0] = 0xf5; // CONFIG
	ucTemp[1] = 0xa0; // set stand by time to 1 second
	rc = write(file_i2c, ucTemp, 2);
	if (rc < 0) {} // suppress warning

	return 0;

} 

int bme280ReadValues(int *T, int *P, int *H)
{
unsigned char ucTemp[16];
int i,rc;
int t, p, h; // raw sensor values
int var1,var2,t_fine;
int64_t P_64;
int64_t var1_64, var2_64;


	ucTemp[0] = 0xf7; // start of data registers we want
	rc = write(file_i2c, ucTemp, 1); // write address of register to read
	i = read(file_i2c, ucTemp, 8);
	if (rc < 0 || i != 8)
	{
		return -1; // something went wrong
	}
	p = (ucTemp[0] << 12) + (ucTemp[1] << 4) + (ucTemp[2] >> 4);
	t = (ucTemp[3] << 12) + (ucTemp[4] << 4) + (ucTemp[5] >> 4);
	h = (ucTemp[6] << 8) + ucTemp[7];

	var1 = ((((t >> 3) - (calT1 <<1))) * (calT2)) >> 11;
	var2 = (((((t >> 4) - (calT1)) * ((t>>4) - (calT1))) >> 12) * (calT3)) >> 14;
	t_fine = var1 + var2;
	*T = (t_fine * 5 + 128) >> 8;
	*T /= 100;
   	*T *= 1.8;
    	*T += 32;
     	


	// Calculate calibrated pressure value
	var1_64 = t_fine - 128000;
	var2_64 = var1_64 * var1_64 * (int64_t)calP6;
	var2_64 = var2_64 + ((var1_64 * (int64_t)calP5) << 17);
	var2_64 = var2_64 + (((int64_t)calP4) << 35);
	var1_64 = ((var1_64 * var1_64 * (int64_t)calP3)>>8) + ((var1_64 * (int64_t)calP2)<<12);
	var1_64 = (((((int64_t)1)<<47)+var1_64))*((int64_t)calP1)>>33;
	if (var1_64 == 0)
	{
		*P = 0;
	}
	else
	{
		P_64 = 1048576 - p;
		P_64 = (((P_64<<31)-var2_64)*3125)/var1_64;
		var1_64 = (((int64_t)calP9) * (P_64>>13) * (P_64>>13)) >> 25;
		var2_64 = (((int64_t)calP8) * P_64) >> 19;
		P_64 = ((P_64 + var1_64 + var2_64) >> 8) + (((int64_t)calP7)<<4);
		*P = (int)P_64 / 100;
	}
	// Calculate calibrated humidity value
	var1 = (t_fine - 76800);
	var1 = (((((h << 14) - ((calH4) << 20) - ((calH5) * var1)) + 
		(16384)) >> 15) * (((((((var1 * (calH6)) >> 10) * (((var1 * (calH3)) >> 11) + (32768))) >> 10) + (2097152)) * (calH2) + 8192) >> 14));
	var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * (calH1)) >> 4));
	var1 = (var1 < 0? 0 : var1);
	var1 = (var1 > 419430400 ? 419430400 : var1);
	*H = var1 >> 12;

	return *T;

} 

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
	fd_set fds;
	struct timeval tout;
	int resp;
	
	FD_ZERO(&fds);
	FD_SET(*file, &fds);
	
	tout.tv_sec = 0;
	tout.tv_usec = 0;
	
	resp = select(*file + 1, NULL, &fds, NULL, &tout);
	if(resp <= 0)
	{
		perror("UART_send_cmd select() failed\n");
		return false;
	}
	
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

	fd_set fds;
	struct timeval tout;
	int resp;
	
	FD_ZERO(&fds);
	FD_SET(*file, &fds);
	
	tout.tv_sec = 0;
	tout.tv_usec = SELECT_R_WAIT_MS * MS_TO_US;
	
	resp = select(*file + 1, &fds, NULL, NULL, &tout);
	if(resp <= 0)
	{
		perror("UART_receive_temp select() failed\n");
		return false;
	}
	
	//fcntl(*file, F_SETFL, 0);
	
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

bool SYNC_init(struct timespec *prev_t, uint32_t *sleep_time)
{
	*sleep_time = SYNC_TIME_S * S_TO_NS;
	
	if(clock_gettime(CLOCK_REALTIME, prev_t) != 0)
	{
		perror("SYNC_init failed\n");
		return false;
	}
	
	return true; 
}



int main(int argc, char *argv[])
{
	
	int ARD_file, RASP_file, resp;
	uint8_t i;
	float LOCAL_temp, ARD_temp, RASP_temp;
	int I2C_Sensor, ARD_Sensor, Raspi_Sensor;
	struct timespec prev_t;
	uint32_t sleep_time;
	char client_msg[200];
	
	int j;
	int T, P, H; // calibrated values

/*********************BME280***********************/
	j = bme280Init(1, 0x76);
	if (j != 0)
	{
		return -1; // problem - quit
	}
	printf("BME280 device successfully opened.\n");
	usleep(1000000); // wait for data to settle for first read
	LOCAL_temp = bme280ReadValues(&T, &P, &H);
	printf("Calibrated temp. = %3.2f C, pres. = %6.2f Pa, hum. = %2.2f%%\n", (float)T/100.0, (float)P/256.0, (float)H/1024.0);
	usleep(DELAY);
/*********************BME280***********************/
	
	if(UART_periph_init(&ARD_file, ARD_UART_PATH) == false)		printf("UART for ARD failed to Initialize... Path: %s\n", ARD_UART_PATH);
	
	if(UART_periph_init(&RASP_file, RASP_UART_PATH) == false)		printf("UART for RASP failed to Initialize... Path: %s\n", RASP_UART_PATH);
	
	if(SYNC_init(&prev_t, &sleep_time) == false)		printf("Getting time to initialize SYNC failed\n");
	
	Socket_Init();

	for(i = 0; i < 10; i ++)
	{
		
		// Add a fix offset to previous absoulte time
		prev_t.tv_sec = prev_t.tv_sec + SYNC_TIME_S;

		// Sleep till the absolute time supplied by prev_t
		do//
		{
			resp = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &prev_t, NULL);
		}while(resp != 0);//
		
		// Dynamic Time Buffer Start....
		
		if(UART_send_cmd(&ARD_file) == false)	printf("UART for ARD failed to Send... Path: %s\n", ARD_UART_PATH);
		if(UART_send_cmd(&RASP_file) == false)	printf("UART for RASP failed to Send... Path: %s\n", RASP_UART_PATH);
		
				
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

