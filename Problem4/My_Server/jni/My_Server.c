#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
void *serve(void * newsockfd);
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int busy = 2;

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno, clilen, n;
	struct sockaddr_in serv_addr, cli_addr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		printf("ERROR opening socket\n");
		exit(1);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	portno = 2050;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("Error on binding\n");
		exit(1);
	}
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	printf("Server initiating...\n");
	int i;
	pthread_t thread[5];
	for (i = 0; i < 5; ++i)
	{
		int n;
		n = pthread_create(&thread[i], NULL, serve, (void *)&sockfd); //Each thread connect to one client
		if(n)
		{
			printf("ERROR; return code is %d\n",n);
			return EXIT_FAILURE;
		}
	}
	for (i = 0; i < 5; ++i)
	{
		pthread_join(thread[i], NULL);
	}
	close(sockfd);
	return 0;
}

void *serve(void *sockfd)
{
	while(1)
	{
		int flag = 0;
		char buffer[256];
		int newsockfd = accept((int)(*((int*)sockfd)), NULL, NULL);
		
		while(1)
		{
			bzero(buffer, 256);
			int n = read(newsockfd, buffer, 255);
			int jump_1 = 0;
			pthread_mutex_lock(&mutex1);		//To make sure that only 2 thread are giving service
   			if(busy > 0)
   			{
   				jump_1 = 1;
   				busy--;
   			}
  			pthread_mutex_unlock(&mutex1);
  			if (jump_1 == 1)
  				break;
  			if(n >= 0 && strcmp(buffer, ":q") == 0)
			{
				printf("Server thread closing...\n");
				close(newsockfd);
				flag = 1;
				break;
			}
		}
			
		if(flag == 1) continue;

		int n,i;
		n = write(newsockfd, "!", 1);		//send a '!' to client to let it know that the server is ready
		if (n < 0) 
		{
			printf("ERROR: writing to socket\n");
			close(newsockfd);
			pthread_mutex_lock(&mutex1);
   			busy++;								
   			pthread_mutex_unlock(&mutex1);
			continue;
		}

		while(1)
		{
			if (n < 0)
			{
				printf("ERROR: reading from socket\n");
				close(newsockfd);
				pthread_mutex_lock(&mutex1);
   				busy++;
   				pthread_mutex_unlock(&mutex1);
				break;
			}
			else if (n > 0 && strcmp(buffer, ":q") == 0)//Link disconnect
			{
				printf("Server thread closing...\n");
				close(newsockfd);
				pthread_mutex_lock(&mutex1);
   				busy++;
   				pthread_mutex_unlock(&mutex1);
				break;
			}
			else if(n >= 0 && strcmp(buffer, ":q") != 0)
			{
				if (n > 0) 
					printf("Receiving message:%s\n", buffer);
				for(i = 0; i < 256; ++i)//Caesar cipher: move back 3 letters
				{
					if(buffer[i] >= 'A' && buffer[i] <= 'Z')
						buffer[i] = (buffer[i] -'A' + 3) % 26 + 'A';
					if(buffer[i] >= 'a' && buffer[i] <= 'z')
						buffer[i] = (buffer[i] -'a' + 3) % 26 +'a';
				}
				n = write(newsockfd, buffer, strlen(buffer));
				if (n < 0) 
				{
					close(newsockfd);
					printf("ERROR: writing to socket\n");
					pthread_mutex_lock(&mutex1);
   					busy++;//releae the counter for next thread
   					pthread_mutex_unlock(&mutex1);
					break;
				}
			}
			bzero(buffer, 256);
			n = read(newsockfd, buffer, 255);
		}
	}
}