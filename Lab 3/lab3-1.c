//Name: Kevin Hom d
//Date: 4/22/19
//Title: Lab 3 - Reading and writing to a file using fwrite and fread
//Description: This program acts as reads and writes to file using function calls

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <sys/wait.h>
#include <sys/types.h> 
#include <sys/ipc.h>


int main(int argc,char *argv[]){

char buffer[1000];

FILE *fp;
FILE *fp_1;

fp = fopen(argv[1], "r");
fp_1 = fopen(argv[2], "w");

int bytes_read;

if(fp){

    while((bytes_read = fread(buffer, 1, sizeof(buffer),fp)) > 0){
     fwrite(buffer, 1, bytes_read, fp_1);
    }
    
    fclose(fp);
    fclose(fp_1);

}

return 0;
}