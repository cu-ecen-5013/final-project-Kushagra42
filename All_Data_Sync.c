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

#define BUFFER_SIZE 100
#define SLAVE_ADDR 0x76
//Raspi UART Function

#define START_CHAR		(char)'1'
#define ARD_UART_PATH	"/dev/ttyO4"
#define RASP_UART_PATH	"/dev/ttyO5"

#define S_TO_NS			(uint32_t)1000000000
#define MS_TO_NS		(uint32_t)1000000
#define MS_TO_US		(uint32_t)1000

#define SELECT_R_WAIT_MS	500

#define SYNC_TIME_S		1
#define ADDITIONAL_MS	100

#define PORT 9000    /* the port client will be connecting to */

#define MAXDATASIZE 100 /* max number of bytes we can get at once */

int Client_Data(char *str, uint32_t len,int argc,char* argv[])
{

       int sockfd,numbytes;  
        char buf[MAXDATASIZE];
        struct hostent *he;
        struct sockaddr_in their_addr; /* connector's address information */

        if (argc != 2) {
            fprintf(stderr,"usage: client hostname\n");
            exit(1);
        }

        if ((he=gethostbyname(argv[1])) == NULL) {  /* get the host info */
            herror("gethostbyname");
            exit(1);
        }

        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket");
            exit(1);
        }

        their_addr.sin_family = AF_INET;      /* host byte order */
        their_addr.sin_port = htons(PORT);    /* short, network byte order */
        their_addr.sin_addr = *((struct in_addr *)he->h_addr);
        bzero(&(their_addr.sin_zero), 8);     /* zero the rest of the struct */

        if (connect(sockfd, (struct sockaddr *)&their_addr, \
                                              sizeof(struct sockaddr)) == -1) {
            perror("connect");
            exit(1);
        }
	
		if (send(sockfd, str, len, 0) == -1){
                      perror("send");
		      exit (1);
		}
		printf("After the send function \n");

        	if ((numbytes=recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
            		perror("recv");
            		exit(1);
		}	

	        buf[numbytes] = '\0';

        	printf("Received in pid=%d, text=: %s \n",getpid(), buf);
	
   close(sockfd);

   return 0;
   
}

// return false if error - true if pass
bool UART_send_cmd(int *file)
{
	char cmd = START_CHAR;
	fd_set fds;
	struct timeval tout;
	int resp;
	
	FD_ZERO(&fds);
	FD_SET(*file, &fds);
	
	tout.tv_sec = 0;
	tout.tv_usec = 0;
	
	resp = select(*file + 1, NULL, &fds, NULL, &tout);
	if(resp < 0)
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
	if(resp < 0)
	{
		perror("UART_receive_temp select() failed\n");
		return false;
	}
	
	fcntl(*file, F_SETFL, 0);
	
	count = read(*file, (void*)&receive[0], BUFFER_SIZE);	//receive the data
	
	if(count < 0)
	{   
		perror("UART ARD receive failed\n");
		*temp = 0;
		return false;
	}
	
	if(count == 0) printf("UART ARD no data to read\n");
	else 
	{
		temperature = atoi(receive);
		printf("UART ARD got integer temp: %d\n",temperature);
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
	
	int ARD_file, RASP_file, resp;
	uint8_t i;
	float LOCAL_temp, ARD_temp, RASP_temp;
	struct timespec prev_t;
	uint32_t sleep_time;
	
	if(UART_periph_init(&ARD_file, ARD_UART_PATH) == false)		printf("UART for ARD failed to Initialize... Path: %s\n", ARD_UART_PATH);
	
	if(UART_periph_init(&RASP_file, RASP_UART_PATH) == false)		printf("UART for RASP failed to Initialize... Path: %s\n", RASP_UART_PATH);
	
	if(SYNC_init(&prev_t, &sleep_time) == false)		printf("Getting time to initialize SYNC failed\n");
	
	for(i = 0; i < 10; i ++)
	{
		
		// Add a fix offset to previous absoulte time
		prev_t.tv_sec = prev_t.tv_sec + SYNC_TIME_S;

		// Sleep till the absolute time supplied by prev_t
		do
		{
			resp = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &prev_t, NULL);
		}while(resp != 0);
		
		// Dynamic Time Buffer Start....
		
		if(UART_send_cmd(&ARD_file) == false)	printf("UART for ARD failed to Send... Path: %s\n", ARD_UART_PATH);
		if(UART_send_cmd(&RASP_file) == false)	printf("UART for RASP failed to Send... Path: %s\n", RASP_UART_PATH);
		
		LOCAL_temp = Get_Temperature();

		if(UART_receive_temp(&ARD_file, &ARD_temp) == false)	printf("UART for ARD failed to Read... Path: %s\n", ARD_UART_PATH);

		if(UART_receive_temp(&RASP_file, &RASP_temp) == false)	printf("UART for RASP failed to Read... Path: %s\n", RASP_UART_PATH);
		
		// *********** TEMPERATURE COMPARING HERE ***********

		
		// *********** SOCKET SENDING HERE ***********
		printf("Ard Temp: %.2f\nRasp Temp: %.2f\nLocal Temp: %.2f\n",ARD_temp,RASP_temp,LOCAL_temp);	// CHANGE TO SYSLOG
		char client_msg[200];
		snprintf(&client_msg[0], 200, "Ard Temp: %.2f\nRasp Temp: %.2f\nLocal Temp: %.2f\n",ARD_temp, RASP_temp, LOCAL_temp);
		Client_Data(&client_msg[0], 200,argc,argv);

		// Dynamic Time Buffer End....
	}
	
	UART_periph_close(&ARD_file);
	UART_periph_close(&RASP_file);
	
	printf("Exiting...");
	exit(0);
}

