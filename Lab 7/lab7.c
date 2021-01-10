/*Name: Kevin Hom
Date: 5/23/19
Title: Lab 7 - Link state routing 
Description: This program simulates link state routing algorithm  
*/

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
#include <pthread.h>
#include <semaphore.h>

#define MACHINES 4

typedef struct{
    int cost;
    int source;
    int neighbor;
} costData;

typedef struct{
    int machine;
    char ip[9];
    int port;
} router;

void *updateCost(void* this_router);
void *keyboardUpdate(void* this_router);
void *link_state_update(void *this_router);
void runDijsktra(int this_router);
int minDistance(bool known_path[]);
void print_table();
router make_router(int machine, char ip[10], int port);

pthread_t threads[3];
pthread_mutex_t lock; 
int dist[MACHINES];
int cost_table[MACHINES][MACHINES];
router router_table[MACHINES];

int main(int argc, char *argv[]){

    int this_routerz = atoi(argv[1]);

    if (pthread_mutex_init(&lock, NULL) != 0) 
    { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 
    
    FILE *fp1 = fopen(argv[2], "r");

    if(fp1 < 0){
        printf("Error! opening\n");
        exit(0);
    }

    FILE *fp2 = fopen(argv[3], "r"); //opening up info of routers

    if(fp2 < 0){
        printf("Error! opening\n");
        exit(0);
    }

    int i,j;
    int temp, port; 
    char ips[10];    

    for(i = 0; i < MACHINES; i++){
        for(j = 0; j < MACHINES; j++){
          fscanf(fp1, "%d", &temp);
          cost_table[i][j] = temp;
        }
    }
       
    print_table();

    for(j = 0; j < MACHINES; j++){
        fscanf(fp2, "%d %s %d", &temp, ips, &port);
        router_table[j] = make_router(temp, ips, port);
    }

    for(j = 0; j < MACHINES; j++){
        printf("Router_ID = %d IP = %s Port = %d \n", router_table[j].machine, router_table[j].ip, router_table[j].port); 
    }


    fclose(fp1);
    fclose(fp2);
    void *ptr = malloc(sizeof(int));
    *((int*)ptr) = this_routerz;
    pthread_create(&threads[0], NULL, updateCost, ptr);
    pthread_create(&threads[1], NULL, keyboardUpdate, ptr);
    pthread_create(&threads[2], NULL, link_state_update, ptr); //here

    pthread_join(threads[1], NULL);
}

void print_table(){
    int i,j;
    for(i = 0; i < MACHINES; i++){
        for(j = 0; j < MACHINES; j++){
            printf("%d ", cost_table[i][j]);
        }
        printf("\n");
    }
}

router make_router(int machine, char ip[10], int port){
    router r; 
    r.machine = machine;
    strncpy(r.ip,ip,10);
    r.port = port;
    return r;
}

void *updateCost(void *this_router){
    costData new_cost;
    int sock;
    struct sockaddr_in server_addr , client_addr;
    socklen_t addr_len;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket");
        exit(1);
    }

    int this_routerz = *((int*)this_router);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = router_table[this_routerz].port; //initializes port number 
    server_addr.sin_addr.s_addr = INADDR_ANY;

    
    if (bind(sock,(struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("Bind");
        exit(1);
    }

    addr_len = sizeof(struct sockaddr);

    while(1){
        if((recvfrom(sock,&new_cost,sizeof(new_cost),0, (struct sockaddr *)&client_addr, &addr_len)) > 0){
            pthread_mutex_lock(&lock); 
            cost_table[new_cost.source][new_cost.neighbor] = new_cost.cost;
            cost_table[new_cost.neighbor][new_cost.source] = new_cost.cost;
            pthread_mutex_unlock(&lock); 
        }
        
    }
}

void *keyboardUpdate(void *this_router){
    int cost, neighbor;

    int sock;
    struct sockaddr_in server_addr;
    struct hostent *host;
    socklen_t addr_len; 
    host= (struct hostent *) gethostbyname((char *)"127.0.0.1");

    int this_routerz = *((int*)this_router);

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
    perror("socket");
    exit(1);
    }

    // set address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = router_table[this_routerz].port; // initializes port number
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);

   
    int counter;
    int i;
    while(1){
        sleep(5);
        if(scanf("%d %d", &cost, &neighbor) == 2){
            counter++;
            printf("read %d %d\n", cost, neighbor);
            fflush(stdin);
            costData new_cost;
            new_cost.source = this_routerz;
            new_cost.cost = cost;
            new_cost.neighbor = neighbor;
            pthread_mutex_lock(&lock); 
            cost_table[new_cost.source][new_cost.neighbor] = new_cost.cost;
            cost_table[new_cost.neighbor][new_cost.source] = new_cost.cost;
            pthread_mutex_unlock(&lock); 
        
            for(i = 0; i < 4; i++){ 
                if(i != this_routerz){
                    printf("Sent to router %d \n", i);
                    host= (struct hostent *) gethostbyname(router_table[i].ip);
                    server_addr.sin_family = AF_INET;
                    server_addr.sin_port = router_table[i].port; 
                    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
                    sendto(sock, &new_cost, sizeof(new_cost), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
                    printf("Router ID = %d, Neighbor ID = %d, New cost = %d \n", this_routerz, new_cost.neighbor, new_cost.cost);
                    
                } 
            }   

             if(counter == 2){
                sleep(30);
                break;
            } 
        }
        
    }
  
}

void *link_state_update(void *this_router){ 
    while(1){
        sleep(rand() % 5 + 5);
        runDijsktra(*((int*)this_router));  
    }
}

int minDistance(bool known_path[]){
    int min = 2147483647;
    int index;

    int i;

    for(i = 0; i < MACHINES; i++){
        if(known_path[i] == false && dist[i] <= min){
            min = dist[i];
            index = i;
        }
    }
    return index;
}

void runDijsktra(int this_router){
	bool known_path[MACHINES];

	int i,k;

	for(i = 0; i < MACHINES; i++){ 
		dist[i] = 10000;
		known_path[i] = false;
	}

 	dist[this_router] = 0;
   
    pthread_mutex_lock(&lock); 
    for(i = 0; i < MACHINES - 1; i++){
        int u = minDistance(known_path);
        known_path[u] = true;
        for(k = 0; k < MACHINES; k++){ 
            if(!known_path[k] && cost_table[u][k] && dist[u] != 2147483647 && dist[u] + cost_table[u][k] < dist[k]){
                dist[k] = dist[u] + cost_table[u][k];

            }
           
        }
    }
    pthread_mutex_unlock(&lock); 

    printf("Link cost from this router to other routers is ");

    for(i = 0; i < MACHINES; i++){
           printf("%d ", dist[i]);
    }
    printf("\n");
}

