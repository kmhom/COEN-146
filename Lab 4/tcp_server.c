//Name: Kevin Hom
//Date: 4/24/2019
//Title: Lab 3 - TCP Server
//Description: This program setups a TCP server to write to a file.

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

int main (int, char *[]); 


int main (int argc, char *argv[])
{
	char *p; 
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 
    char buff[1024]; //create a buffer of size 1024

	// set up
    memset (&serv_addr, '0', sizeof (serv_addr));
    memset (buff, '0', sizeof (buff)); 

    int fp_1 = open("dest.txt", O_WRONLY | O_CREAT, 0640); //open the file that is going to be written to


	if(fp_1 < 0){ //checks to make sure the destination file was opened
	    printf("Error! opening writing file \n");
	    exit(0);
	}


    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
    serv_addr.sin_port = htons (5000); //assign port number

	// create socket, bind, and listen
    if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) < 0) 
	{
		printf ("socket error\n");
		return 1;
	}

    if (bind (listenfd, (struct sockaddr*)&serv_addr, sizeof (serv_addr)) < 0) //make sure that binding to the socket was successful
		printf ("bind error\n");
		return 1;
	}

    if (listen (listenfd, 10) < 0)
	{
		printf ("listen error\n");
		return 1;
	}

	int bytes_read;

	// accept and interact
    while (1)
    {
        if ((connfd = accept (listenfd, (struct sockaddr*)NULL, NULL)) < 0) //This is the ACK part, where the server acknowledges that a connection has been established between client and server
		{
			printf ("accept error\n");
			return 1;
		} 

		// receive data and reply, assuming string fits in buff tom simplify
		while ((bytes_read = read(connfd, buff, sizeof (buff))) > 0) //read data that was sent into the socket into the buffer
		{
			
        	write (fp_1, buff, bytes_read); //write to the file using data from the buffer and bytes_read to keep track of how much data was actually read
		}

        close (connfd);
        close(fp_1);
     }
}
