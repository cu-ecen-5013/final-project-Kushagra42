#include<stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include<syslog.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>

#define PACKET_SIZE_MAX		1024
#define PACKET_COUNT_MAX	100
#define FILE_SIZE_MAX		PACKET_SIZE_MAX * PACKET_COUNT_MAX

char *Port = "9000";

int soc_fd, store_fd;
int new_soc_fd, stoe_new_cos_fd;
char ipstr[INET6_ADDRSTRLEN];

struct sockaddr_in peer_addr;
struct addrinfo hints;
struct addrinfo *servinfo = NULL;
struct addrinfo *p = NULL;

socklen_t peer_addr_size;

FILE *ref;

char *my_buffer;

int flag = 0;
int mode = 0;

void forking()
{
	/* Our process ID and Session ID */
        pid_t pid, sid;
        
        /* Fork off the parent process */
	
        pid = fork();
      
	if (pid < 0) {
                exit(EXIT_FAILURE);
        }
        /* If we got a good PID, then
           we can exit the parent process. */
        if (pid > 0) {
                exit(EXIT_SUCCESS);
        }

        /* Change the file mode mask */
        umask(0);
                
        /* Open any logs here */        
                
        /* Create a new SID for the child process */
        sid = setsid();
        if (sid < 0) {
                /* Log the failure */
                exit(EXIT_FAILURE);
        }
        

        
        /* Change the current working directory */
        if ((chdir("/")) < 0) {
                /* Log the failure */
                exit(EXIT_FAILURE);
        }
        
        /* Close out the standard file descriptors */
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

}

void get_fd()
{
 
    soc_fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if(soc_fd<0)
    {
        syslog(LOG_ERR,"Error at get_FD() is:%s",gai_strerror(soc_fd));
	if(mode == 0) printf("Error at get_FD() is:%s\n ",gai_strerror(soc_fd));
    }
    
store_fd = soc_fd;
}

void get_addr()
{
    int ret;
       
    ret=getaddrinfo(NULL,    
                       "9000",  
                       &hints,
                      &servinfo);

    if(ret!=0)
    {
	printf("Error found is in get_addr:%s\n",gai_strerror(ret));
        syslog(LOG_ERR,"Error found is in get_addr:%s",gai_strerror(ret));
        
    }


}
int bind_soc()
{
    int ret;
    int yes=1;
    ret=bind(store_fd,servinfo->ai_addr,servinfo->ai_addrlen);
    
    if(ret<0)
    {
	printf("Error found is in bind:%s\n",gai_strerror(ret));
        syslog(LOG_ERR,"Error found is in bind:%s",gai_strerror(ret));
        return -1;
    }
    
    if (setsockopt(store_fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) 
    {
        perror("setsockopt");
        exit(1);
	return -1;
    } 
	freeaddrinfo(servinfo);
	return 0;
}

void listen_soc()
{
    int ret;
    ret = listen(store_fd,5);
    
    if(ret<0)
    {
	if(mode == 0) printf("Error found is in listen:%s\n",gai_strerror(ret));
        syslog(LOG_ERR,"Error found is in listen:%s",gai_strerror(ret));
        
    }


}

void accept_soc()
{

	new_soc_fd=accept(store_fd,(struct sockaddr*)&peer_addr,&peer_addr_size);

	if(flag == 0)
	{
		if(new_soc_fd<0)
		{	
			if(mode == 0) printf("Error found is in accept:%s \n",gai_strerror(new_soc_fd));
			syslog(LOG_ERR,"Error found is in accept:%s",gai_strerror(new_soc_fd));
		}
		stoe_new_cos_fd = new_soc_fd;
	}    
}

int receive_soc(char *buf)
{
	int ret = 0;
	ret=recv(stoe_new_cos_fd, buf, PACKET_SIZE_MAX,0);
	if(flag == 0)
	{
		if(mode == 0) printf("%s",buf);
		if(strcmp(buf, "EXIT") == 0)	flag = 1;
		if(ret<0)
		{
			if(mode == 0) printf("Error found is in receive:%s\n ",gai_strerror(ret));
			syslog(LOG_ERR,"Error found is in receive:%s ",gai_strerror(ret));
		}
	}
	return ret;
}


// sigterm handler
void sig_exit(int value)
{
	flag = 1;
	if(mode == 0) printf("\nSIGINT / SIGTERM Received, Exiting...\n");
	syslog(LOG_INFO, "Caught signal, exiting");
}

// sigterm setup
void sigact_setup(void)
{
   struct sigaction sig_act;
   memset(&sig_act, 0, sizeof(struct sigaction));
   sig_act.sa_handler = &sig_exit;
   sigaction(SIGTERM, &sig_act, 0);
   sigaction(SIGINT, &sig_act, 0);

}

int main(int argc, char *argv[])
{
	if(argc > 1)
	{
		if(strcmp(argv[1], "-d") == 0)
		{
			if(mode == 0) printf("daemon mode\n");
			mode = 1;
		}
		else
		{
			if(mode == 0) printf("invalid argument\n");
		}
	}
	else
	{
		if(mode == 0) printf("running in regular mode\n");
	}

	//opening syslog capability
	openlog("SYslog_PID",LOG_PID, LOG_USER);
	printf("\n*****MY PID: %d*****\n\n", getpid());

	sigact_setup();

	memset(&hints, 0, sizeof hints);

	/*****************Allocating Buffer Space*************/

	my_buffer = malloc(PACKET_SIZE_MAX);

	/*****************Socket Functions*************/

	hints.ai_family=PF_INET;    
	hints.ai_socktype=SOCK_STREAM;  
	hints.ai_flags=AI_PASSIVE;    
	hints.ai_protocol=0;

	printf("get_addr\n");
	get_addr();

	printf("get_fd\n");
	get_fd();

	printf("bind_soc\n");
	if(bind_soc() == 0)
	{
		if(mode == 1)
		{
			forking();
		}
	}

	if(mode == 0) printf("listen_soc\n");
	listen_soc();

	if(mode == 0) printf("accept_soc\n");
	accept_soc();

	

	while(1)
	{
		
		memset(my_buffer, 0, PACKET_SIZE_MAX);

		if(mode == 0) printf("receive_soc\n");
		int byte_cnt;
		byte_cnt = receive_soc(my_buffer);

		if(flag == 1)
		{
			break;
		}

		if(byte_cnt < 0)
		{
			break;
		}

		else if(byte_cnt > 0)
		{
			if(mode == 0) printf("packet complete\n");
		}
		if(flag == 1)
		{
			break;
		}
	}

	shutdown(store_fd, SHUT_RDWR);

	closelog();
	return 0;
}
