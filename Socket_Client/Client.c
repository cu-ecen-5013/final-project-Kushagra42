#include <stdio.h> 
    #include <stdlib.h> 
    #include <errno.h> 
    #include <string.h> 
    #include <netdb.h> 
    #include <sys/types.h> 
    #include <netinet/in.h> 
    #include <sys/socket.h> 
    #include <unistd.h>
    #include <arpa/inet.h>
    #define PORT 9000    /* the port client will be connecting to */

    #define MAXDATASIZE 100 /* max number of bytes we can get at once */
    


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

    int main(int argc, char *argv[])
    {
        Socket_Init();
	while(1)
	{
		Client_Data("Testing client-server connection",100);

	}
        return 0;
    }
