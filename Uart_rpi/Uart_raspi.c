// //https://www.raspberrypi.org/forums/viewtopic.php?f=33&t=44972&p=372182#p372182
// //https://www.raspberrypi.org/forums/viewtopic.php?f=33&t=7500&sid=7c55b988e36afcaf2a01db466aaec9f7&start=25
// //https://www.raspberrypi.org/forums/viewtopic.php?t=7500&p=93257
// #include <stdio.h>
// #include <string.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <errno.h>
// #include <termios.h>
// //#include "Uart_raspi.h"
// //#include "../Temp_i2c/Temperature_i2c.h"

// //int main(int argc, char ** argv) 

// int main()
// {
//   int fd;
//   // Open the Port. We want read/write, no "controlling tty" status, and open it no matter what state DCD is in
//   fd = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);
//   if (fd == -1)
//   {
//     perror("open_port: Unable to open /dev/ttyAMA0 - ");
//     return(-1);
//   }

//   // Turn off blocking for reads, use (fd, F_SETFL, FNDELAY) if you want that
//   fcntl(fd, F_SETFL, 0);


//   struct termios options;
//   tcgetattr(fd, &options);

//     int baud_in=cfsetispeed(&options, B9600);
//   if (baud_in == -1)
//   {
//     perror("Error in setting input required baud rate\n ");
//     return(-1);
//   }
//   int baud_out=cfsetospeed(&options, B9600);
//   if (baud_out==-1)
//   {
//     perror("Error in setting output required baud rate\n ");
//     return(-1);
//   }
//       /* input flags */
//     options.c_iflag &= ~ IGNBRK;                       /* enable ignoring break */
//     options.c_iflag &= ~(IGNPAR | PARMRK);             /* disable parity checks */
//     options.c_iflag &= ~ INPCK;                        /* disable parity checking */
//     options.c_iflag &= ~ ISTRIP;                       /* disable stripping 8th bit */
//     options.c_iflag &= ~(INLCR | ICRNL);               /* disable translating NL <-> CR */
//     options.c_iflag &= ~ IGNCR;                        /* disable ignoring CR */
//     options.c_iflag &= ~(IXON | IXOFF);                /* disable XON/XOFF flow control */
//     /* output flags */
//     options.c_oflag  &= ~ OPOST;                        /* disable output processing */
//     options.c_oflag  &= ~(ONLCR | OCRNL);               /* disable translating NL <-> CR */
//     /* not for FreeBSD */
//     options.c_oflag &= ~ OFILL;                        /* disable fill characters */
//     /* control flags */
//     options.c_cflag  |=   CLOCAL;                       /* prevent changing ownership */
//     options.c_cflag  |=   CREAD;                        /* enable reciever */
//     options.c_cflag  &= ~ PARENB;                       /* disable parity */
//     options.c_cflag  &= ~ CSTOPB;                       /* disable 2 stop bits */
//     options.c_cflag  &= ~ CSIZE;                        /* remove size flag... */
//     options.c_cflag  |=   CS8;                          /* ...enable 8 bit characters */
//     options.c_cflag  |=   HUPCL;                        /* enable lower control lines on close - hang up */
//     options.c_cflag &= ~CRTSCTS;	

//         /* local flags */
//     options.c_lflag &= ~ ISIG;                         /* disable generating signals */
//     options.c_lflag &= ~ ICANON;                       /* disable canonical mode - line by line */
//     options.c_lflag &= ~ ECHO;                         /* disable echoing characters */
//     options.c_lflag &= ~ ECHONL;                       /* ??? */
//     options.c_lflag &= ~ NOFLSH;                       /* disable flushing on SIGINT */
//     options.c_lflag &= ~ IEXTEN;                       /* disable input processing */

//         /* control characters */
//     memset(options.c_cc,0,sizeof(options.c_cc));
// /*
//   int baud_in=cfsetispeed(&options, B115200);
//   if (baud_in == -1)
//   {
//     perror("Error in setting input required baud rate\n ");
//     return(-1);
//   }
//   int baud_out=cfsetospeed(&options, B115200);
//   if (baud_out==-1)
//   {
//     perror("Error in setting output required baud rate\n ");
//     return(-1);
//   }
// */  

// //  tcsetattr(fd, TCSANOW, &options);
//   tcsetattr(fd, TCSAFLUSH, &options);
//       /* enable input & output transmission */
//  //   tcflow(fd, TCOON | TCION);
//   // Write to the port
//   unsigned char tx_buffer[20]= "Hello Aesd";
// //  unsigned char *p_tx_buffer;
//   printf("Testing uart ");	
// //  p_tx_buffer = &tx_buffer[0];
// //  *p_tx_buffer++ = 'H';
// //  *p_tx_buffer++ = 'e';
// //  *p_tx_buffer++ = 'l';
// //  *p_tx_buffer++ = 'l';
// //  *p_tx_buffer++ = 'o';
// //size_t count = strlen(tx_buffer);
// //  int n = write("Hi testing uart functionality for raspi\n");
//   int n = write(fd, &tx_buffer, 11);		//Filestream, bytes to write, number of bytes to write
// //  int n = write(fd,&tx_buffer[0],(p_tx_buffer - &tx_buffer[0]));
//   if (n < 0) 
//   {
//     perror("Write failure ");
//     return -1;
//   }
  
//   usleep(10000);
//   // Read up to 255 characters from the port if they are there
//   char buf[256];
//   int r= read(fd, (void*)buf, 20);
//   if (r < 0) 
//   {
//     perror("Read failure ");
//     return -1;
//   }
//   else if (r == 0) 
// 	printf("No data on port\n");
//   else 
//   {
//     buf[r] = '\0';
//     printf("%i bytes read : %s\n", r, buf);
//   }

//   // Don't forget to clean up
//   close(fd);
//   printf("End of uart test");
//   return 0;
// //  return 0;
// }

// //https://www.raspberrypi.org/forums/viewtopic.php?f=33&t=44972&p=372182#p372182
// //https://www.raspberrypi.org/forums/viewtopic.php?f=33&t=7500&sid=7c55b988e36afcaf2a01db466aaec9f7&start=25
// //https://www.raspberrypi.org/forums/viewtopic.php?t=7500&p=93257
// #include <stdio.h>
// #include <string.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <errno.h>
// #include <termios.h>
// //#include "../Temp_i2c/Temperature_i2c.h"

// //int main(int argc, char ** argv) 
// int uart(char tx_buffer[20])
// {
//   int fd;
//   // Open the Port. We want read/write, no "controlling tty" status, and open it no matter what state DCD is in
//   fd = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);
//   if (fd == -1)
//   {
//     perror("open_port: Unable to open /dev/ttyAMA0 - ");
//     return(-1);
//   }

//   // Turn off blocking for reads, use (fd, F_SETFL, FNDELAY) if you want that
//   fcntl(fd, F_SETFL, 0);


//   struct termios options;
//   tcgetattr(fd, &options);

//     int baud_in=cfsetispeed(&options, B9600);
//   if (baud_in == -1)
//   {
//     perror("Error in setting input required baud rate\n ");
//     return(-1);
//   }
//   int baud_out=cfsetospeed(&options, B9600);
//   if (baud_out==-1)
//   {
//     perror("Error in setting output required baud rate\n ");
//     return(-1);
//   }
//       /* input flags */
//     options.c_iflag &= ~ IGNBRK;                       /* enable ignoring break */
//     options.c_iflag &= ~(IGNPAR | PARMRK);             /* disable parity checks */
//     options.c_iflag &= ~ INPCK;                        /* disable parity checking */
//     options.c_iflag &= ~ ISTRIP;                       /* disable stripping 8th bit */
//     options.c_iflag &= ~(INLCR | ICRNL);               /* disable translating NL <-> CR */
//     options.c_iflag &= ~ IGNCR;                        /* disable ignoring CR */
//     options.c_iflag &= ~(IXON | IXOFF);                /* disable XON/XOFF flow control */
//     /* output flags */
//     options.c_oflag  &= ~ OPOST;                        /* disable output processing */
//     options.c_oflag  &= ~(ONLCR | OCRNL);               /* disable translating NL <-> CR */
//     /* not for FreeBSD */
//     options.c_oflag &= ~ OFILL;                        /* disable fill characters */
//     /* control flags */
//     options.c_cflag  |=   CLOCAL;                       /* prevent changing ownership */
//     options.c_cflag  |=   CREAD;                        /* enable reciever */
//     options.c_cflag  &= ~ PARENB;                       /* disable parity */
//     options.c_cflag  &= ~ CSTOPB;                       /* disable 2 stop bits */
//     options.c_cflag  &= ~ CSIZE;                        /* remove size flag... */
//     options.c_cflag  |=   CS8;                          /* ...enable 8 bit characters */
//     options.c_cflag  |=   HUPCL;                        /* enable lower control lines on close - hang up */
//     options.c_cflag &= ~CRTSCTS;	

//         /* local flags */
//     options.c_lflag &= ~ ISIG;                         /* disable generating signals */
//     options.c_lflag &= ~ ICANON;                       /* disable canonical mode - line by line */
//     options.c_lflag &= ~ ECHO;                         /* disable echoing characters */
//     options.c_lflag &= ~ ECHONL;                       /* ??? */
//     options.c_lflag &= ~ NOFLSH;                       /* disable flushing on SIGINT */
//     options.c_lflag &= ~ IEXTEN;                       /* disable input processing */

//         /* control characters */
//     memset(options.c_cc,0,sizeof(options.c_cc));
// /*
//   int baud_in=cfsetispeed(&options, B115200);
//   if (baud_in == -1)
//   {
//     perror("Error in setting input required baud rate\n ");
//     return(-1);
//   }
//   int baud_out=cfsetospeed(&options, B115200);
//   if (baud_out==-1)
//   {
//     perror("Error in setting output required baud rate\n ");
//     return(-1);
//   }
// */  

// //  tcsetattr(fd, TCSANOW, &options);
//   tcsetattr(fd, TCSAFLUSH, &options);
//       /* enable input & output transmission */
//  //   tcflow(fd, TCOON | TCION);
//   // Write to the port
// //  unsigned char tx_buffer[20]= "Hello Aesd";
// //  unsigned char *p_tx_buffer;
//   printf("Testing uart ");	
// //  p_tx_buffer = &tx_buffer[0];
// //  *p_tx_buffer++ = 'H';
// //  *p_tx_buffer++ = 'e';
// //  *p_tx_buffer++ = 'l';
// //  *p_tx_buffer++ = 'l';
// //  *p_tx_buffer++ = 'o';
// //size_t count = strlen(tx_buffer);
// //  int n = write("Hi testing uart functionality for raspi\n");
//   int n = write(fd, &tx_buffer, 11);		//Filestream, bytes to write, number of bytes to write
// //  int n = write(fd,&tx_buffer[0],(p_tx_buffer - &tx_buffer[0]));
//   if (n < 0) 
//   {
//     perror("Write failure ");
//     return -1;
//   }
  
//   usleep(10000);
//   // Read up to 255 characters from the port if they are there
//   char buf[256];
//   int r= read(fd, (void*)buf, n);
//   if (r < 0) 
//   {
//     perror("Read failure ");
//     return -1;
//   }
//   else if (r == 0) 
// 	printf("No data on port\n");
//   else 
//   {
//     buf[r] = '\0';
//     printf("%i bytes read : %s\n", r, buf);
//   }

//   // Don't forget to clean up
//   close(fd);
//   printf("End of uart test");
//   return 0;
// //  return 0;
// }


#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdint.h>

int main(void)
{
    int fd1, count;
    struct termios options;
    unsigned char tx[20] = "Hello TM4C123GXL", rx[20];

    printf("Testing uart implementation");

    if ((fd1 = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
    {
        perror("open\n");
        return -1;
    }

    tcgetattr(fd1, &options);
    if((cfsetispeed(&options, B9600)) == -1)
    {
        perror("Input baud rate\n");
        return -1;
    }
    if((cfsetospeed(&options, B9600)) == -1)
    {
        perror("Output baud rate\n");
        return -1;
    } 

    options.c_cflag |= (CLOCAL | CS8);
    options.c_iflag &= ~(ISTRIP | IXON | INLCR | PARMRK | ICRNL | IGNBRK);
    options.c_oflag = 0;
    options.c_lflag = 0;

    tcsetattr(fd1, TCSAFLUSH, &options);

    printf("Sending: '%s'\n", tx);
    if ((count = write(fd1, &tx, 17)) < 0)
    {
        perror("write\n");
        return -1;
    }

    usleep(100000);

    printf("Receive characters\n");

    if ((count = read(fd1, (void*)rx, 100)) < 0)
    {
        perror("read\n");
        return -1;
    }

    if(count)
    {
        printf("Received-> %s, %d chars", rx, count);
    }

    close(fd1);
    return 0;



}
