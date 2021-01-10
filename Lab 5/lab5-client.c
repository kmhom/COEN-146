//Name: Kevin Hom
//Date: 5/17/19
//Title: Lab 5 - UDP client 
//Description: This program acts as a UDP client with rdt 2.2 and sends data to the server. 

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
#include <stdbool.h>
#include <sys/select.h>
#include <sys/time.h>

typedef struct{
    int seq_ack;
    int len;
    int cksum;
} Header;


typedef struct{
    Header head;
    char data[10];
} Packet;

void packetCorrupt(Packet *packet){ //Has a 10% chance to corrupt a packet
    if(rand() % 10 == 1){
        packet->head.cksum = 0;
        printf("Packet is corrupt \n");
    }
}

int get_checksum(Packet packet){
    int i;
    int cksum  = 0;
    packet.head.cksum = 0;
    char *ptr = (char*) &packet;

    if(packet.head.len == 0){
    	for(i = 0; i < sizeof(packet.head); i++){ //If there is no data in the packet, only check sum the header
    		cksum ^= ptr[i];
    	}
    }

    else{
    	for(i = 0; i < (sizeof(packet.head) + packet.head.len) ; i++){ //If there is data in the packet, check sum the header and amount of data that is in the packet
            cksum ^= ptr[i];
      }

    }

    return cksum;
}

Packet makePacket(int seq, char buffer[], int len){
    Packet newPacket;
    newPacket.head.seq_ack = seq;
    newPacket.head.len = len;
    memcpy(newPacket.data, buffer, len);
    return newPacket;
}

int main() {
int sock;
struct sockaddr_in server_addr;
struct hostent *host;
socklen_t addr_len; 
host = (struct hostent *) gethostbyname((char *)"127.0.0.1");

if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
perror("socket");
exit(1);
}

server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(5000); 
server_addr.sin_addr = *((struct in_addr *)host->h_addr);

int fp = open("src2.dat", O_RDONLY); 

if(fp < 0){
    printf("Error! opening\n");
    exit(0);
}


int bytes_read;

char buffer[10];

Packet send_packet, receive_packet;

int seq_num = 0;

struct timeval tv;

int rv;

fd_set readfds;
fcntl(sock,F_SETFL,O_NONBLOCK);

FD_ZERO(&readfds);
FD_SET(sock, &readfds);

tv.tv_sec = 1;
tv.tv_usec = 0;


 	bytes_read = read(fp, buffer, sizeof(buffer));
 		send_packet = makePacket(seq_num, buffer, bytes_read);
 		send_packet.head.cksum = get_checksum(send_packet);
		//packetCorrupt(send_packet);
	 		sendto(sock, &send_packet, sizeof(send_packet), 0,
              	(struct sockaddr *)&server_addr, sizeof(struct sockaddr));	
			printf("Packet sent from client to server: Seq: %d, len: %d, check_sum: %d \n", send_packet.head.seq_ack, send_packet.head.len, send_packet.head.cksum); //Send out initial packet with seq 0 and then enter the while loop
 	

  	while(recvfrom(sock,&receive_packet,sizeof(receive_packet),0, (struct sockaddr *)&server_addr, &addr_len)){

  		if(receive_packet.head.seq_ack != seq_num || (receive_packet.head.cksum != get_checksum(receive_packet))){ //if the received packet seq num does not match the expected seq num or the packet is corrupt, resend the packet
			printf("check sum of packet received from in header is %d and check sum of the whole received packet is %d \n", receive_packet.head.cksum, get_checksum(receive_packet));
          
 				send_packet = makePacket(seq_num, buffer, bytes_read);
 				send_packet.head.cksum = get_checksum(send_packet);
        	sendto(sock, &send_packet, sizeof(send_packet), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
			rv = select(sock + 1, &readfds, NULL, NULL, &tv);
			printf("Packet sent from client to server: Seq: %d, len: %d, check_sum: %d \n", send_packet.head.seq_ack, send_packet.head.len, send_packet.head.cksum);
		}

		else{ //If received packet has correct check sum and seq num, then read from the file, make a new packet with the new data, and send it out
			if((bytes_read = read(fp, buffer, sizeof(buffer))) > 0){ 
 				seq_num = 1 - seq_num;
 				send_packet = makePacket(seq_num, buffer, bytes_read);
 				send_packet.head.cksum = get_checksum(send_packet);
				packetCorrupt(&send_packet);
	 			sendto(sock, &send_packet, sizeof(send_packet), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
	 			rv = select(sock + 1, &readfds, NULL, NULL, &tv);
	 			if(rv == 0){
	 				//packetCorrupt(&send_packet);
	 				sendto(sock, &send_packet, sizeof(send_packet), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));

	 			}
				printf("Packet sent from client to server: Seq: %d, len: %d, check_sum: %d \n", send_packet.head.seq_ack, send_packet.head.len, send_packet.head.cksum);
		    }
			else{
				seq_num = 1 - seq_num; //When getting the last packet, send out the last ack and break out of the loop
 				send_packet = makePacket(seq_num, buffer, bytes_read);
 				send_packet.head.cksum = get_checksum(send_packet);
				packetCorrupt(&send_packet);
	 			sendto(sock, &send_packet, sizeof(send_packet), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
	 			rv = select(sock + 1, &readfds, NULL, NULL, &tv);
				printf("Packet sent from client to server: Seq: %d, len: %d, check_sum: %d \n", send_packet.head.seq_ack, send_packet.head.len, send_packet.head.cksum);
				break;
		    }
		}
			
		
  	}

   close(fp);
   close(sock);
}

