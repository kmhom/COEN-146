//Name: Kevin Hom
//Date: 4/22/19
//Title: Lab 3 - Reading and writing to a file using write and read
//Description: This program reads and writes to a file using system calls 

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <sys/wait.h>
#include <sys/types.h> 
#include <sys/ipc.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc,char *argv[]){

char buffer[1000];


int fp = open("src.txt", O_RDONLY);
int fp_1 = open("dest2.txt", O_WRONLY | O_CREAT, 0640);


if(fp < 0){
    printf("Error! opening\n");
    exit(0);
}

if(fp_1 < 0){
    printf("Error! opening writing file \n");
    exit(0);
}

int nr = 1;
int nw = 0;

int bytes_read;

while((bytes_read = read(fp, buffer, sizeof(buffer))) > 0){
     
     write(fp_1, buffer, bytes_read);
}

close(fp);
close(fp_1);

}