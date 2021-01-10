//Name: Kevin Hom
//Date: 4/24/2019
//Title: Lab 3 - TCP Client
//Description: This program setups a TCP client to read from a file.

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <fcntl.h>

int main (int, char *[]);

int main (int argc, char *argv[])
{
    int sockfd = 0, n = 0;
    char buff[1024];
    struct sockaddr_in serv_addr; 

    if (argc != 2) //Makes sure that there are two arguments given during it execution
    {
		printf ("Usage: %s <ip of server> \n",argv[0]);
		return 1;
    } 
  
    memset (&serv_addr, '0', sizeof (serv_addr)); 

   	int fp = open("src.txt", O_RDONLY); //opens the file to be read from

	if(fp < 0){  //make sure that the file to be read from was opened successfully
    	printf("Error! opening\n");
    	exit(0);
	}

    if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
		printf ("socket error\n");
		return 1;
    } 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons (5000); 

    if (inet_pton (AF_INET, argv[1], &serv_addr.sin_addr) <= 0) //checks whether the given ip address is valid by converting the text ip address into binary form. If it can't then it will return an error
    {
		printf ("inet_pton error\n");
		return 1;
    } 

	
    if (connect (sockfd, (struct sockaddr *)&serv_addr, sizeof (serv_addr)) < 0) //makes sure the connection to the socket is successful
    {
		printf ("connect error\n");
		return 1;
    } 

	int bytes_read;

	while ((bytes_read = read (fp, buff, sizeof (buff))) > 0) //read from the file into the buffer and keep track of the amount of bytes read
	{
		
		write (sockfd, buff, bytes_read);
		
	} 

	close (sockfd);
	close(fp);

    return 0;
}
