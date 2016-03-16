
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "myunp.h"
#include "mytimer.h"

#define MAXBUFSIZE 4096 
#define INFINITY -1
#define USAGE "\n ./router nodename(A, B, C etc...) <port no (1-65535)> \n"
#define NBR_CONF_FILE "neighbor.config" 

// Declaring the Timers 
mytimer_t timerA = TIMER_INIT;
mytimer_t timerB = TIMER_INIT;
mytimer_t timerC = TIMER_INIT;
mytimer_t timerD = TIMER_INIT;


// *** Defining a Structure For printing the table ***

typedef struct node_s {
	char  node_name[50];
	int   node_cost; 
	char  node_nbr[50];
	unsigned short node_port;
}node_t;


node_t nodes[20];  

struct  sockaddr_in node_addr;
struct  sockaddr_in nbr_addr[10];
struct  sockaddr_in recv_addr;

int   sockfd;            // The Socket for the Node
int   metric[15];
char  names[10][15];
char  buffer[MAXBUFSIZE];
int   nbr_cnt;
int   node_cnt = 0;

int   i,j;

void *RIP_Receive(void *number){
	struct  sockaddr_in tempaddr;
	int number_of_nodes = (int)number;
	int listen_fd;  
	unsigned short port;

	listen_fd = Socket(AF_INET, SOCK_DGRAM, 0);

	memset((char *) &tempaddr, 0, sizeof(tempaddr)); 
	tempaddr.sin_family      = AF_INET;
	tempaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	tempaddr.sin_port        = htons(atoi(port));

	// Binding the Addresses with the Socket Descriptor
	Bind(listen_fd, (SA *) &tempaddr, sizeof(tempaddr));

	while(1){

	}
}


void update_time(time_t now) {
	int n; 

	for(i = 0; i < nbr_cnt; i++) {
		if ((n = sendto(sockfd, buffer, strlen(buffer),0,(struct sockaddr *) &nbr_addr[i],			   sizeof(struct sockaddr_in)) == -1));
				perror("update_time");
	}
}


/* Prints routing table */
void print_routes(node_t node) {

	printf("\n");
	printf("Node \t %s", node.node_name);
	printf("\t Cost \t %d ", node.node_cost);
	printf("\n");
}

/* Prints usage */
void usage(char *usage) {
		printf("usage is %s \n", usage);  
}

/* Validate the number of arguments and print useage if invalid */
void validate_arg(int argc, int max, char *useage){
	if (argc != max){
		usage(useage);
		exit(-1);
	}
}

node_t selfnode;

/* Parse neighbor.config 
void get_nbr_config() {
	FILE 	*file;
	char 	*token;
	char 	buffer[MAXBUFSIZE];
	int 	distance[15];
	int   check_s = 0;
	int   check_d = 0;
	int 	count=0;
	char  temp_distance[10][15];
	char	Destination[10][15];
char	node_nbr[15];

	file = fopen(NBR_CONF_FILE, "r");

	while (fgets(buffer, sizeof(buffer), file)) {
     	if(strstr(buffer,"#") == NULL) {
			token = strtok(buffer," ");
			while(token != NULL) {
				strcpy(node_nbr[count], token);
				token = strtok(NULL, " ");
				strcpy(Destination[count], token);
				token = strtok(NULL, " ");
				strcpy(temp_distance[count],token);
				distance[count] = atoi(temp_distance[count]);
				token = strtok(NULL, " ");
			} 

			if(count == 0){
				strcpy(names[node_cnt],node_nbr[count]);
				node_cnt++;
				strcpy(names[node_cnt],Destination[count]);
				node_cnt++;
			} else {
				for(i=0;i<node_cnt;i++){
					if(strstr(names[i],node_nbr[count])!=NULL)
						check_s = 1;
					if(strstr(names[i],Destination[count])!= NULL)
						check_d = 1;
				}
			}
			if(check_s == 0){
				strcpy(names[node_cnt],node_nbr[count]);
				node_cnt++;
			}
			if(check_d == 0){
				strcpy(names[node_cnt],Destination[count]);
				node_cnt++;
			}
		}           
		count++;
		check_s = 0;
		check_d = 0;
	}
	fclose(file);
}  
*/

// ********  TILL HERE WE HAVE FIGURED OUT THE NUMBER OF NODES IN THE TOPOLOGY *****

int main(int argc, char **argv){

char	node_name[15];
char	node_nbr[15];
pthread_t   thread; 

FILE 	*neighbor_file;
FILE	*nbr_config;
FILE	*configuration_file;
char 	*token;

char  message[50];
	int 	distance[15];

char	Destination[10][15];
char 	Neighbors[10][15];
int   thread_return;
int   addrlen;
char  Neighbors_Port[50][15];
char  Neighbors_IP[50][15];
char  Node_IP[50];
char  Node_Port[50];
char  temp_node[50];
char  temp_IP[50];
char  temp_port[50];
char  temp_buffer[50];
int   recv_metric[15];


int   check=0;
int 	node_cost_Metric[15];
char  temp_distance[10][15];
int 	count=0;
int 	temp_count = 0;


int   pos;
time_t  timer;
fd_set  readfd,rset;
struct  timeval tv,tv_1;
int   maxfd;
	int n;
	int   check_s = 0;
	int   check_d = 0;


	printf("\n My Node Details: \n");
	validate_arg(argc, 3, USAGE);
    strcpy(selfnode.node_name,argv[1]);
	selfnode.node_port = atoi(argv[2]);

	printf("\n My Node Details: %s:%d \n", selfnode.node_name, selfnode.node_port);

	//get_nbr_config();
	// ****** THIS SECTION IS FOR FIGURING OUT THE NUMBER OF NODES IN THE TOPOLOGY *******

	printf("\n a \n");
		 nbr_config = fopen("neighbor.config","r");

		// printf("\n The configuration file read: \n");

		 while (fgets(buffer, sizeof(buffer), nbr_config)) {
			
		   // printf("\n Inside the while loop.. \n");
			if(strstr(buffer,"#")==NULL){

		   //	printf("\n Inside the if condition.. \n");	

	printf("\n a \n");
			token = strtok(buffer," ");

				while( token != NULL ) 
				 {
					strcpy(selfnode.node_nbr[count],token);
					//printf("\n Just copied %s \n",Source[count]);
	printf("\n 1b \n");
					token = strtok(NULL," ");
					strcpy(Destination[count],token);
					//printf("\n Just copied %s \n",Destination[count]);
	printf("\n 1b \n");
					token = strtok(NULL," ");
					strcpy(temp_distance[count],token);
	printf("\n 1b \n");
					//printf("\n Just copied %s \n",temp_distance[count]);
					distance[count] = atoi(temp_distance[count]);
					token = strtok(NULL," ");

				 } 
				
	printf("\n b \n");
				if(count == 0){
				  strcpy(names[node_cnt],node_nbr[count]);
				  node_cnt++;
				  strcpy(names[node_cnt],Destination[count]);
				  node_cnt++;
				} 
				else{
				  for(i=0;i<node_cnt;i++){
                if(strstr(names[i],node_nbr[count])!=NULL){
                  check_s = 1;
                }
                if(strstr(names[i],Destination[count])!= NULL){
                  check_d = 1;
                }
              }
              if(check_s == 0){
                strcpy(names[node_cnt],node_nbr[count]);
                node_cnt++;
              }
	printf("\n c \n");
              if(check_d == 0){
                strcpy(names[node_cnt],Destination[count]);
                node_cnt++;
              }
            }           


            
            count++;
            check_s = 0;
            check_d = 0;

            
            
      
          } // if condition
          //printf("\n Outside the if condition.. \n");
             
       }   // While loop

       //printf("\n Outside the while loop.. \n");

       fclose(nbr_config);


// ********  TILL HERE WE HAVE FIGURED OUT THE NUMBER OF NODES IN THE TOPOLOGY *****





     /*  printf("\n The nodes are: ");
       for(i=0;i<node_cnt;i++){
        printf("\n node: %s",names[i]);
       } */

       count = 0;

       for(i=0;i<node_cnt;i++){
         if(strstr(selfnode.node_name,names[i])!= NULL){
          pos=i;
          metric[i] = 0;
         // printf("\n The metric at position %d is 0",i);
         }
       }



// **************  THIS PART OF THE CODE IS TO FIGURE OUT THE NEIGHBORS AND TO GET THE DISTANCE METRICS BETWEEN NEIGHBORS *******

       configuration_file = fopen("neighbor.config","r");

    // printf("\n The configuration file reads: \n");

      while (fgets(buffer, sizeof(buffer), configuration_file)) {
        
       // printf("\n Inside the while loop.. \n");
        if(strstr(buffer,"#")==NULL){

       // printf("\n Inside the if condition.. \n");  

         token = strtok(buffer," ");

            while( token != NULL ) 
             {
                strcpy(temp_node,token);
                //printf("\n Just copied %s \n",node_nbr[count]);
                token = strtok(NULL," ");
                strcpy(Destination[count],token);
                //printf("\n Just copied %s \n",Destination[count]);
                token = strtok(NULL," ");
                strcpy(temp_distance[count],token);
                //printf("\n Just copied %s \n",temp_distance[count]);
                distance[count] = atoi(temp_distance[count]);
                token = strtok(NULL," ");

             } 

             if(strstr(selfnode.node_name,node_nbr[count]) != NULL){
               strcpy(Neighbors[temp_count],Destination[count]);
               for(i=0;i<node_cnt;i++){
                if(strstr(names[i],Neighbors[temp_count])!= NULL){
                  metric[i] = distance[count];
                }
               }
               temp_count++;

             }

             else if(strstr(selfnode.node_name,Destination[count])!= NULL){
                strcpy(Neighbors[temp_count], selfnode.node_nbr[count]);
                for(i=0;i<node_cnt;i++){
                  if(strstr(names[i],Neighbors[temp_count])!=NULL) {
                    metric[i] = distance[count];
                  }
                }
                temp_count++;
             }




            count++;
            

            
            
      
          } // if condition
          //printf("\n Outside the if condition.. \n");
             
       }   // While loop

       //printf("\n Outside the while loop.. \n");

       fclose(configuration_file);


// ************** TILL THIS PART WE HAVE FIGURED OUT THE NEIGHBORS AND THEIR METRICS ***************       


       for(i=0;i<node_cnt;i++){
          for(j=0;j<temp_count;j++){
             if(strstr(names[i],Neighbors[j])!=NULL){
              check=1;
             }
          }
          if(check == 0){
            if(i != pos){
              metric[i] = INFINITY;
            }
          }
          check=0;
       }

      // printf("\n The node name is: %s",self.node_name);

       // *** This loop is for getting the neighbors of the node

      /* for(i=0;i<count;i++){
       	 if(strstr(self.node_name,node_nbr[i]) != NULL){
           strcpy(Neighbors[temp_count],Destination[i]);
           node_cost_Metric[temp_count] = distance[i];
           metric[i] = distance[i];
           temp_count++;
       	  }
          else{
            metric[i] = 17;
          }


       }*/

       

       /*printf("\n The Neighbors of node: %s are: \n",self.node_name);
       for(i=0;i<temp_count;i++){
          printf(" node: %s with distance: %d \n",Neighbors[i],node_cost_Metric[i]);
       }*/



// ********** THIS PART OF CODE GETS THE IP AND PORT NUMBERS OF THE NEIGHBORS ***************

       nbr_cnt = temp_count;

       temp_count = 0;

       bzero(buffer,MAXBUFSIZE);


       neighbor_file = fopen("node.config","r");

      // printf("\n The configuration file reads: \n");

       while (fgets(buffer, sizeof(buffer), neighbor_file)) {
        
       // printf("\n Inside the while loop.. \n");
       if(strstr(buffer,"#")==NULL){

       // printf("\n Inside the if condition.. \n");  

            token = strtok(buffer," ");

            while( token != NULL ) 
             {
                strcpy(temp_node,token);
                //printf("\n Just copied %s \n",node_nbr[count]);
                token = strtok(NULL," ");
                strcpy(temp_IP,token);
                //printf("\n Just copied %s \n",Destination[count]);
                token = strtok(NULL," ");
                strcpy(temp_port,token);
                //printf("\n Just copied %s \n",temp_distance[count]);
                token = strtok(NULL," ");

             } 
                       
            
      
            } // if condition
          //printf("\n Outside the if condition.. \n");

            if(strstr(temp_node,selfnode.node_name) != NULL){
              strcpy(Node_IP,temp_IP);
              strcpy(Node_Port,temp_port);
            }
            else{
              for(i=0;i<nbr_cnt;i++){
                if(strstr(temp_node,Neighbors[i])!= NULL){
                  strcpy(Neighbors_IP[i],temp_IP);                  
                  strcpy(Neighbors_Port[i],temp_port);
                 }
              }   // for loop
            }    // else condition
             
          }   // While loop

          //printf("\n Outside the while loop.. \n");

        fclose(neighbor_file);

// *********** TILL HERE WE HAVE GOT THE IP ADDRESS AND THE PORT NUMBERS OF THE NEIGHBORS  *************



        printf("\n The node %s has an IP %s and port %s",selfnode.node_name,Node_IP,Node_Port);

         printf("\n And the details of their neighbours are as follows:");

        for(i=0;i<nbr_cnt;i++){
          printf("\n The neighbour %s has an IP %s and a port %s",Neighbors[i],Neighbors_IP[i],Neighbors_Port[i]);
        }

        
        for(i=0;i<node_cnt;i++){
          strcpy(nodes[i].node_name,names[i]);
          nodes[i].node_cost = metric[i]; 
          strcpy(nodes[i].node_nbr,selfnode.node_name);
        }


// **************** This piece of code is for initializing the node socket and the node_addr structure *****************
        // Initializing the Socket Descriptor for the Node
        sockfd = Socket(AF_INET, SOCK_DGRAM, 0);


        memset((char *) &node_addr, 0, sizeof(node_addr)); 
        node_addr.sin_family      = AF_INET;
        node_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        node_addr.sin_port        = htons((atoi(Node_Port))+1);
    
        // Binding the Addresses with the Socket Descriptor
        Bind(sockfd, (SA *) &node_addr, sizeof(node_addr));

// ***************** TILL HERE ***********************************************************
        time(&timer);

        printf("\n The update happened at %s",ctime(&timer));

        for(i=0;i<node_cnt;i++){
          print_routes(nodes[i]);
        }
   


//************  This piece of code is for initializing the nbr_addr structures for the neighbors ****************


        for(i=0;i<nbr_cnt;i++){
          memset((char *) &nbr_addr[i], 0, sizeof(nbr_addr[i])); 
          nbr_addr[i].sin_family = AF_INET;
          nbr_addr[i].sin_port   = htons(atoi(Neighbors_Port[i]));                         //Converting the port number to the network order
          if (inet_pton(AF_INET, Neighbors_IP[i], &nbr_addr[i].sin_addr) <= 0)       // Converting the IP address specified from string to network order
          err_quit("inet_pton error for IP: %s", Neighbors_IP[i]);
        }  


        thread_return = pthread_create(&thread,NULL,RIP_Receive,(void *) &nbr_cnt);
        if(thread_return){
           printf("\n There was an error in creating the Thread..");
        }

        // Starting the timer which will send the updates every 5 seconds 
        timer_start_periodic(&timerA, 5, update_time); 

//****************** TILL HERE **********************************************************
 
while(1){
         
        


        tv_init(&tv);
        tv_timer(&tv, &timerA);
        tv_timer(&tv, &timerB);
        tv_timer(&tv, &timerC);
        tv_timer(&tv, &timerD);


// ******************* CREATING THE MESSAGE BUFFER WITH ALL THE METRICS *****************
        strcpy(buffer,"UPDATE");

        for(i=0;i<node_cnt;i++){
          sprintf(message," %d",metric[i]);
          strcat(buffer,message);
        }
// ********************** TILL HERE *****************************************************




         maxfd = sockfd+1;   // *******  IMPORTANT PART OF SELECT **********
         bzero(buffer,MAXBUFSIZE);
         addrlen = sizeof(recv_addr);


// **************** THIS IS THE CODE FOR CHECKING THE TIMERS **********************

        FD_ZERO(&rset);

        if (select(1, &rset, NULL, NULL, &tv) < 0)
        {
            err_quit("select() < 0, strerror(errno) = %s\n", strerror(errno));
        }

        timer_check(&timerA);
        timer_check(&timerB);
        timer_check(&timerC);
        timer_check(&timerD);

// ***************  TILL HERE *********************************************                 

         FD_ZERO(&readfd);
         FD_SET(sockfd,&readfd);
         tv_1.tv_sec = 2;
         tv_1.tv_usec = 0;
         select (maxfd, &readfd, NULL, NULL, &tv_1);
         printf("\n The Select is not blocking.. \n");

         if(FD_ISSET(sockfd,&readfd)){
             n = recvfrom(sockfd, buffer, MAXBUFSIZE, 0, (struct sockaddr *) &recv_addr, &addrlen);
          }

          if(n == -1){
            printf("\n There was an error in receiving the message.. \n");
          }
          

// ********** THIS CODE STRIPS UP THE RECEIVED MESSAGE **************
          count = 0;
          token = strtok(buffer," ");
          token = strtok(NULL," ");

            while( token != NULL ) 
             {
                recv_metric[count] = atoi(token);
                //printf("\n just copied %d",recv_metric[count]);
                count++; 
                token = strtok(NULL," ");

             } 

// *********** TILL HERE **********************



          
          

}








        






















    



     return 0;




}   // Main Function
