#include "myunp.h"
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>


int main(int argc, char **argv){

char	Node_Name[10];
int 	Port_Number;  
FILE 	*configuration_file;
FILE 	*neighbor_file;
char 	read_buffer[4096];
char 	write_buffer[4096];

     if(argc < 3){
		printf("\n The Node Name and the Port Number have not been mentioned properly..");
     }

     strcpy(Node_Name,argv[1]);    // Copying the node name specified into the Array
     Port_Number = atoi(argv[2]);  // Copying the port number to the datatype

     printf("\n The node name is: %s",Node_Name);
     printf("\n The port number of the node is: %s",Port_Number);

     configuration_file = fopen("neighbor.config","r");

     printf("\n The configuration file reads: \n");

     while (fgets(read_buffer, sizeof(read_buffer), configuration_file)) {
            printf("%s ", read_buffer); 
     }  // While loop




     return 0;




}   // Main Function