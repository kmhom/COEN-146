//Name: Kevin Hom
//Date: 4/22/19
//Title: Lab 3 - UDP client file sending
//Description: This program acts as a UDP client and sends data to the server. 

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

int main()
{
int sock;
struct sockaddr_in server_addr;
struct hostent *host;
char send_data[1024];
socklen_t addr_len; 
host= (struct hostent *) gethostbyname((char *)"127.0.0.1");

//FILE *fp;

int fp = open("src.txt", O_RDONLY); //open file that is going to be read from

if(fp < 0){ //check if the file opened successfully
    printf("Error! opening\n");
    exit(0);
}

char buffer[1024]; //defines a buffer for data to pass through

// open socket
if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) //make sure socket is connected
{
perror("socket");
exit(1);
}

// set address
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(5000); // initializes port number
server_addr.sin_addr = *((struct in_addr *)host->h_addr);

	int bytes_read;
	int total_bytes = 0;
    
    while((bytes_read = read(fp, buffer, sizeof(buffer)))> 0){ //The bytes_read keeps track of the number of bytes that are read into the buffer. 
    
       sendto(sock, buffer, bytes_read, 0,
              (struct sockaddr *)&server_addr, sizeof(struct sockaddr)); //send to server and make sure to include the size of buffer as bytes_read since the client may not read a total of 1024 into the buffer
                                                                         //it sends data that was read into the buffer into the socket and uses the server and socket address as a destination point
    
       total_bytes += bytes_read;
       printf("%d \n", total_bytes); //This is for debugging
       
    }

    close(fp);


}

