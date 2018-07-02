#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int wait = 0;
void *Wait(void *sockfd)
{
	char buffer[256];
	int _sockfd = (int)(*((int*)sockfd));
	int n = read(_sockfd, buffer, 255);		//To get the '!' from server
	pthread_mutex_lock(&mutex1);
   	wait++;
   	pthread_mutex_unlock(&mutex1);
	return 0;
}

int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[256];
	portno = 2050;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		printf("ERROR opening socket");
		exit(1);
	}
	server = gethostbyname("127.0.0.1");
	if (server == NULL)
	{
		printf("ERROR, no such host");
		exit(0);
	}
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("ERROR connecting");
		exit(1);
	}
	printf("Please enter the message:\n" );

	pthread_t thread;
	pthread_attr_t attr;
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&thread, &attr, Wait, (void *)&sockfd);
	while(1)
	{
		bzero(buffer, 256);
		gets(buffer);
		n = write(sockfd, buffer, strlen(buffer));
		sleep(1);
		if (strcmp(buffer, ":q") == 0)
    	{
    		printf("Clinet closing...");
    		close(sockfd);
    		return 0;
    	}
    	else
    	{
    		pthread_mutex_lock(&mutex1);	//decide whether the server is ready
    		if(wait == 1)					//on serivce
    		{
    			if (n < 0) 
        		{
        			printf("ERROR: writing from socket\n");
        			continue;
        		}
    			bzero(buffer,256);
    			n = read(sockfd, buffer, 255);
    			if (n < 0) 
        		{
        			printf("ERROR: reading from socket\n");
        			continue;
        		}
    			printf("From server:%s\n", buffer);
    		}
    		else							//wait state
    			printf("Please wait...\n");
   			pthread_mutex_unlock(&mutex1);
    	}
	}

	return 0;
}