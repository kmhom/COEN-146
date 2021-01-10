//Name: Kevin Hom
//Date: 5/17/19
//Title: Lab 5 - UDP server 
//Description: This program acts as a UDP server with rdt 2.2 and recieves data from the client to be written to a file. 

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
#include <stdbool.h>

typedef struct{
    int seq_ack;
    int len;
    int cksum;
} Header;

typedef struct{
    Header head;
    char data[10];
} Packet;

int get_checksum(Packet packet){
    int i;
    int cksum  = 0;
    packet.head.cksum = 0;
    char *ptr = (char*) &packet;

    if(packet.head.len == 0){
        for(i = 0; i < sizeof(packet.head); i++){
            cksum ^= ptr[i];
        }
    }

    else{
        for(i = 0; i < (sizeof(packet.head) + packet.head.len); i++){
            cksum ^= ptr[i];
      }

    }

    return cksum;
}

Packet makeEmptyPacket(int seq){
    Packet newPacket;
    newPacket.head.seq_ack = seq;
    newPacket.head.len = 0;
    memset(newPacket.data, 0, 10);
    return newPacket;
}

void packetCorrupt(Packet *packet){
    if(rand() % 10 == 1){
        packet->head.cksum = 0;
        printf("Packet is corrupt \n");
    }
}

void packetLoss(Packet ){
    
}

bool isCorrupt(Packet packet){ //Checks if the received packet has corrupted data
        int cksum_client = packet.head.cksum;

        packet.head.cksum = 0;

        int cksum_server = get_checksum(packet);

        if(cksum_server != cksum_client){
            return true;
        }

        else{
            return false;
        }              
}

int main() {

    int sock;
    int connfd = 0;
    struct sockaddr_in server_addr , client_addr;
    socklen_t addr_len;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000); //initializes port number 
    server_addr.sin_addr.s_addr = INADDR_ANY;

    int fp_1 = open("dest.dat", O_WRONLY | O_CREAT, 0640);

 //Bind socket to address
    if (bind(sock,(struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1){
        perror("Bind");
        exit(1);
    }

    addr_len = sizeof(struct sockaddr); 

	printf("\t\t\t\t\t\t\nUDPServer Waiting for client on port 5000\n");

    Packet receive_packet, ack_packet;

    int seq_num = 0;
    int i;
    int bytes_read;

  
    recvfrom(sock,&receive_packet,sizeof(receive_packet),0, (struct sockaddr *)&server_addr, &addr_len); //receive initial packet

	while(receive_packet.head.len != 0){        //keep the server open as long as the received packets have data
                                                                                                                                                                           
       if(receive_packet.head.seq_ack != seq_num || isCorrupt(receive_packet) == true){ //if not correct seq num or corrupted packet, send back wrong ack. 
          
            //printf("Check sum of client_packet in header is %d and check sum of the whole received packet is %d \n", receive_packet.head.cksum, get_checksum(receive_packet));
            ack_packet = makeEmptyPacket(1 - seq_num);
            ack_packet.head.cksum = get_checksum(ack_packet);
           packetCorrupt(&ack_packet);
            sendto(sock, &ack_packet, sizeof(ack_packet.head), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
            recvfrom(sock,&receive_packet,sizeof(receive_packet),0, (struct sockaddr *)&server_addr, &addr_len);
          
       }

       else{
                write(fp_1, receive_packet.data, receive_packet.head.len); //if correct ack and not corrupt, then write the data from the packet into the destination file and send out an ack and update seq num.
                ack_packet = makeEmptyPacket(seq_num);
                ack_packet.head.cksum = get_checksum(ack_packet);
                packetCorrupt(&ack_packet);
                sendto(sock, &ack_packet, sizeof(ack_packet.head), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
                seq_num = 1 - seq_num; //change the expected sequence number
                printf("Packet sent from server to client: Seq: %d, len: %d, check_sum: %d \n", ack_packet.head.seq_ack, ack_packet.head.len, ack_packet.head.cksum);
                recvfrom(sock,&receive_packet,sizeof(receive_packet),0, (struct sockaddr *)&server_addr, &addr_len);
       }
    }
    
        ack_packet = makeEmptyPacket(seq_num); //Handle last packet sent by client by sending last ack
        ack_packet.head.cksum = get_checksum(ack_packet);
        sendto(sock, &ack_packet, sizeof(ack_packet.head), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));


    close(fp_1);    

  return 0;
}
