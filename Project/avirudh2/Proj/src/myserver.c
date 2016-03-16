/*
*   File:       myserver.c
*
*   Purpose:    This is a skeleton file for a server.
*/

#include "myunp.h"
#include "myunp.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>

int adsf(int argc, char **argv)
{

	int				  server_socket, client_socket, n;
	struct      sockaddr_in	servaddr;
	char		    send_buffer[256];
	char 			  recv_buffer[256];
  char        buffer[256];
	//time_t			  ticks;
	int         choice = 0;
	int         bytes;
	int 			  port_number;
	char		    conv[256];
	int 			  connection_queue = 10;
	FILE         *f_ptr;
	char 			  cond[256];
	char 			  check[20];
    int        x = 1;
    int 			option = 1;         
    int       file_option = 1;   // Flag for file's existence (1 if present,0 if not)
    int       file_size = 0;
    int       bytes_read;
    int       bytes_written;
    int       file_d;
    int       offset_size;
    int       chunk_size;
    int       err;
    int       size_check;  // To calculate the number of times the while loop has to run
    char       name[256];

    struct stat s = {0};

	if(argc != 2){
		printf("\n The Port Number was not specified..");
	}

    // Cretaing the Server Socket
   	server_socket = Socket(AF_INET, SOCK_STREAM, 0);

	if(server_socket == -1){
		printf("\n There was an error in creating the socket..");
	}

	bzero(&servaddr, sizeof(servaddr));
	port_number = atoi(argv[1]); // Assigning the Port Number

	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(port_number);
    
    // Binding the Addresses with the Socket Descriptor
	Bind(server_socket, (SA *) &servaddr, sizeof(servaddr));
   
    if(n == -1){
    	printf("\n There was an error in Binding the socket..");
    }

    // Listening for the connections on the specified port
    Listen(server_socket, connection_queue);

    if(n == -1){
    	printf("\n There was an error in Listening.. ");
    }
    else{
    	printf("\n The Server is UP and is Listening.. ");
    }


    for( ; ; ){

    	client_socket = Accept(server_socket, (SA *) NULL, NULL);

    	if(client_socket == -1){
    		printf("\n There was an error in Accepting the Connection..");
    	}
    	else{
    		printf("\n The Client has connected.. ");
    	}

    	bzero(recv_buffer,256);

      if( (n = read(client_socket, recv_buffer, 256)) != 0){
            if(strstr(recv_buffer, "FILE-CHECK") != NULL){
                 option = 1;
                 bzero(recv_buffer,256);    
                
                 bzero(send_buffer,256);
                 strcpy(send_buffer,"ACK");
                 bytes =  Write(client_socket, send_buffer, strlen(send_buffer));

                 if(bytes == -1){
                printf("\n Writing error..");
                   }

            }else{
              if(strstr(recv_buffer, "GET-DATA") != NULL){
                 option = 2;
                 bzero(recv_buffer,256);                                      //Setting the option flag as 1 i.e. client is requesting the file size
            

                bzero(send_buffer,256);
                strcpy(send_buffer,"ACK");
                bytes =  Write(client_socket, send_buffer, strlen(send_buffer));

                if(bytes == -1){
                 printf("\n Writing error..");
                }
            }
       }
      } 

        if(option == 1){

    	         // option = 0;
                if( (n = read(client_socket, recv_buffer, 256)) != 0){
            
                char *strlocation;
                strlocation = strstr(recv_buffer, "End of file");
                //strcpy(strlocation,"\0");
                printf("\n The file name sent by Client is: %s",recv_buffer);  
              //  option = 2; 
                
              }
    	  	    
    	  	  
            
        
                // Code for checking if a file exists or not

                if( access(recv_buffer, F_OK ) != -1 ) {
                 file_option = 1;
                } 
                else {
                 file_option = 0;
                }   

                //bzero(recv_buffer,256);


            // Code for getting the size of a file using stat 
            bzero(send_buffer,256);  
            

            //char *fd = recv_buffer;
            if(file_option == 1){      
            stat(recv_buffer, &s);
            file_size = s.st_size;
            // printf("\n The File Size given by stat is: %d",file_size);
            sprintf(send_buffer, "%d", file_size);
           // printf("\n Send_buffer: %s",send_buffer);

           }
           else{
            strcpy(send_buffer,"NO FILE");
           }

            // Sending the file size or NO FILE message to the client
            bytes =  Write(client_socket, send_buffer, strlen(send_buffer));
            if(bytes == -1){
                printf("\n Writing error..");
            }
         }
            
         else if(option == 2){

          bzero(recv_buffer,256);  

          //Reading the name of the file from the client
          if( (n = read(client_socket, recv_buffer, 256)) != 0){
            
                char *strlocation;
                strlocation = strstr(recv_buffer, "End of file");
                strcpy(name,recv_buffer);
                //strcpy(strlocation,"\0");
              //  printf("\n The file name sent by Client is: %s",recv_buffer);  
              //  option = 2; 
                
              }

              // Sending an ACK for the name

              bzero(send_buffer,256);
              strcpy(send_buffer,"ACK");
              bytes =  Write(client_socket, send_buffer, strlen(send_buffer));

              if(bytes == -1){
              printf("\n Writing error..");
               }

           

          // Waiting for the Offset_Size from the Client
            while( (n = read(client_socket, buffer, 256)) != 0){
            if(strstr(buffer, "ACK") != NULL){
                 break;
            }
            else{
               offset_size = atoi(buffer);
            }
          }
          // Sending the ACK for Offset_Size
          bzero(send_buffer,256);
          strcpy(send_buffer,"ACK");
          bytes =  Write(client_socket, send_buffer, strlen(send_buffer));

           if(bytes == -1){
              printf("\n Writing error..");
          }

          // Waiting for the chunk_size from the client
          while( (n = read(client_socket, buffer, 256)) != 0){
            if(strstr(buffer, "ACK") != NULL){
                 break;
            }
            else{
               chunk_size = atoi(buffer);
            }
          }
        
          printf("\n The offset size received is: %d",offset_size);
          printf("\n The chunk size received is: %d",chunk_size);
            

            
            // Sending the Data to the file

            
            
            f_ptr = fopen(name,"r");
            if(f_ptr==NULL)
              {
                printf("File open error");
                return 1;   
              }  

            err = fseek(f_ptr, offset_size, SEEK_SET);
            if(err < 0){
              printf("\n There was an error in traversing the file");
            }

            size_check = chunk_size/256;   // Calculating the times the loop has to iterate
            
            bzero(send_buffer,256);

            n = 0;


            while (n != (size_check+1)) {
           
               bytes_read = fread(send_buffer,1,256,f_ptr);
                
                if(bytes_read > 0)
                 {
                   // printf("Sending \n");
                    write(client_socket, send_buffer, bytes_read);
                 }

                 if (bytes_read < 256)
                {
                if (feof(f_ptr))
                    printf("End of file\n");
                if (ferror(f_ptr))
                    printf("Error reading\n");
                     break;
                }
                n++;
              }

           

           bzero(send_buffer,256);
           strcpy(send_buffer,"ENDOFFILE");
           
           bytes =  Write(client_socket, send_buffer, strlen(send_buffer));
           
          if(bytes == -1){
              printf("\n Writing error..");
            }


     }


      close(client_socket);

       
    	  	  
          //recv_buffer[n] = 0;	/* null terminate */
        

    }

    
    return 0;
}
