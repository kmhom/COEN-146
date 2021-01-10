//Name: Kevin Hom
//Date: 4/22/19
//Title: Lab 3 - UDP server file sending
//Description: This program acts as a UDP server and recieves data from the client to be written to a file. 


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


int main()
{
    int sock;
    int bytes_read = 1 ;
    int connfd = 0;
    char recv_data[1024]; //creates a buffer of size 1024
    struct sockaddr_in server_addr , client_addr;
    socklen_t addr_len;

        if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
            perror("Socket");
            exit(1);
        }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000); //initializes port number 
    server_addr.sin_addr.s_addr = INADDR_ANY;

    int fp_1 = open("dest.txt", O_WRONLY | O_CREAT, 0640);

 //Bind socket to address
    if (bind(sock,(struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("Bind");
        exit(1);
    }
    addr_len = sizeof(struct sockaddr); //length of the socket address 
		
	printf("\t\t\t\t\t\t\nUDPServer Waiting for client on port 5000\n");

	while ((bytes_read = recvfrom(sock,recv_data,1024,0, (struct sockaddr *)&client_addr, &addr_len)) > 0) //The server receives the data from the socket and reads it into the buffer recv_data.  B
                                                                                                           //Bytes_read is used to keep track of how many bytes were transmitted to the server, so that when writing the data that is contained in the buffer of the file, it will only write the amount of bytes that were read instead of the whole 1024 bytes of buffer                                                                                                            
	{
       write(fp_1, recv_data, bytes_read); //write to the destination file using data from the recv_data and capping off the amount of data written using bytes_read.
    }

    close(fp_1);    

    

  return 0;
}
