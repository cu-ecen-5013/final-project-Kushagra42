#include<stdio.h>
#include "syslog.h"

int main(void)
{
	printf("\n\rHello World!");
	//Open syslog
	openlog("HELLOWORLD",LOG_PID|LOG_CONS,LOG_USER);
	syslog(LOG_INFO,"Message from HelloWorld!");
	return 0;
}
