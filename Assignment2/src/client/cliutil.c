#include "cliutil.h"


int get_ipaddr_list(const char *filename) {
	int fd;
	fileaddress_t curfileaddr = {{0},{0},0};
	struct sockaddr_in *cursockaddrp;	
	int count = 0;
	char line[MAXLINELEN];
	int n;
	char *ipstr;
	char *portstr;
	unsigned long int port;
	char address[30];
	int err = 0;
	const char *errstr;
	
	fd = Open(filename, O_RDONLY, 0);
	fileaddrsp = (fileaddress_t *)malloc(MAXLINES * sizeof(fileaddress_t));
	bzero(fileaddrsp, MAXLINES * sizeof(fileaddress_t));

	while(count < MAXLINES) {
		bzero(address, sizeof(address));
		bzero(line, sizeof(line));
		n = Readline(fd, line, MAXLINELEN);
		if (line[0] ==  NEWLINE)
			continue;
		if (!n)
			break;
		if (n < 7)
			continue;
		ipstr = strtok(line, " ");
		if (!strcmp(ipstr, LOCALHOSTSTR))
			strcpy(ipstr, LOCALHOST);
		portstr = strtok(NULL, " ");
		
		cursockaddrp = &(curfileaddr.addr); 
		/* Convert ip and port from string to respective types */
		err = inet_pton(AF_INET, ipstr, &(cursockaddrp->sin_addr));

		if (err != 1) {
			printf("Error : Invalid address %s\n", address);
			continue;		
		}
		cursockaddrp->sin_family = AF_INET;
		retrieve_port(portstr, &port);
		cursockaddrp->sin_port = htons(port);
        	errstr = inet_ntop(AF_INET, &(curfileaddr.addr.sin_addr), address, INET_ADDRSTRLEN);

		if (errstr == NULL) {
			printf("Error : Invalid address %s\n", address);
			continue;		
		}
		memcpy(&fileaddrsp[count], &curfileaddr, sizeof(fileaddress_t));
		count++;		
	}
	
	fileservcnt = count++;
	
	return SUCCESS;
}


/* return a listen socket bound on  given port */
int get_udp_socket(unsigned long port) {
	int sockfd = 0;
	struct sockaddr_in serv_addr; 

	/* fill socket data structure given a port */
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port); 

	/* listen to client and return an accepted socket */
	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
	Bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	return sockfd;
}



void check_serv_avail() {
	int sockfd;
	int i = 0;
	int err = SUCCESS;
	char address[MAXADDRSIZE];
	int count = 0;
	struct	sockaddr_in *addrp;
	struct sockaddr *castaddrp;

	printf("Connecting to available servers...(connect might take 1 sec) \n");
	for (i = 0; i < fileservcnt; i++) {	
	 	/* open a socket and connect to the server */
		addrp = &(fileaddrsp[i].addr);
		castaddrp = (struct sockaddr *)addrp;
		sockfd = Socket(AF_INET, SOCK_STREAM, 0);
		err = connect_timeo(sockfd, castaddrp, sizeof(struct sockaddr), 1);
		inet_ntop(AF_INET, &(addrp->sin_addr), address, INET_ADDRSTRLEN);
		if (err < 0) {			
			printf("Error : Could not connect to address:port %s:%d \n", address, ntohs(addrp->sin_port));
			fileaddrsp[i].servavail = 0;
			continue;
		}
		fileaddrsp[i].servavail = 1;
		printf("Success : Connected to address:port %s:%d \n", address, ntohs(addrp->sin_port));
		fileaddrsp[i].sockfd = sockfd;	
		count++;
	}
	if (count == 0) {
		printf("Sorry no servers to connect at this point \n");
		myfree(buffer);
		exit(1);
	}
	availservcnt = count; 
}

void send_chunk_info(int serverno) {
	int sockfd = fileaddrsp[serverno].sockfd;
	chunkinfo_t *infop = &(fileaddrsp[serverno].chunkinfo);

	printf("\nThread %d writing from offset %ld till %ld \n", serverno, infop->off, infop->off + infop->size); 
	create_buffer(CHUNKINFO, infop, buffer);
	Write(sockfd, buffer, MAXBUFSIZE);
}

void send_query_info(int serverno) {
	int sockfd = fileaddrsp[serverno].sockfd;
	chunkinfo_t *infop = &(fileaddrsp[serverno].chunkinfo);

	printf("sending chunkinfo ... \n");		
	create_buffer(QUERYINFO, infop, buffer);
	Write(sockfd, buffer, MAXBUFSIZE);
}

void get_ack(int sockfd, char *buffer) {
	tlv_t *bufstp;
	Read(sockfd, buffer, MAXBUFSIZE * sizeof(char));
	retrieve_buffer(buffer, &bufstp);
	myfree(bufstp);
}

void get_file_from_user() {
	char buffer[MAXBUFSIZE];
	char *end;
	
	while(1) {
		printf(PROMPTSTR);
		bzero(buffer, MAXBUFSIZE);
		/* get user input, remove the new line character and
		 * if user doesn't enter a command continue
		 */
		fgets(filename, MAXFILENAMESIZE, stdin);
		snprintf(buffer, MAXFILENAMESIZE, "%s", filename);
		end = strrchr(filename, '\n');
		if (end)
			*end = '\0';
		if (!strlen(filename))
			continue;
		if(!strcmp(filename, EXITCMD))
			exit(1);
		break;
	}
}

void send_filename() {
	int sockfd;
	int i;
	int count = 0;
	fileaddress_t *tmpfileaddrp;

	printf("sending filename ... \n");		

	for (i = 0; i < MAXLINES; i++) {
		if (count >= userservcnt) 
			break;
		tmpfileaddrp = &fileaddrsp[i];
		if (tmpfileaddrp->servavail == 0)
			continue;
		count++;
		sockfd = fileaddrsp[i].sockfd;
		create_buffer(FILENAME, filename, buffer);
		Write(sockfd, buffer, MAXFILENAMESIZE);
	}
}

void get_file_status() {
	FILE *fp;
	int sockfd; 
	tlv_t *bufstp = NULL;
	fileaddress_t *tmpfileaddrp;
	int i = 0;
	printf("getting file status ... \n");		
	for (i = 0; i < userservcnt; i++) {
		tmpfileaddrp = &fileaddrsp[i];

		if (tmpfileaddrp->servavail == 0)
			continue;
		
		sockfd	= tmpfileaddrp->sockfd;
		send_query_info(i);
		sleep(1);
		//get_ack(sockfd, buffer);
	
		Read(sockfd, buffer, MAXBUFSIZE);
		retrieve_buffer(buffer, &bufstp);
		if (bufstp->buf_err == 0) {
			fileinforx = 1;
			myfree(bufstp);
			break;	 
		}
		else {
			myfree(bufstp); 
			printf(FILEINVALIDSTR);
			exit(1);
		}
	}

	filesize = bufstp->buf_cksize;
	filefd = Open(filename, O_CREAT|O_WRONLY|O_TRUNC, FILE_MODE);
	ftruncate(filefd, filesize); 
	fp = fdopen(filefd, "w");
	fclose(fp);
}

#define MAXTXSIZE 10
void get_file_data(int serverno) {
	FILE *fp;
	size_t count = 0;
	chunkinfo_t *infop = &(fileaddrsp[serverno].chunkinfo);
	long totalsize = infop->size;
	long seek = infop->off;
	int i = 0;
	char buf[MAXTXSIZE + 1];
	int sockfd = fileaddrsp[serverno].sockfd;
	
	pthread_mutex_lock(&lock);
	filefd = Open(filename, O_CREAT|O_WRONLY, FILE_MODE);
	fp = fdopen(filefd, "w");

	fseek(fp, seek, SEEK_SET);

	do {
		if (totalsize - MAXTXSIZE < 0)
			count = totalsize;
		else
			count = MAXTXSIZE;
		Read(sockfd, buf, count); 	
		Write(filefd, buf, count);
		totalsize -= MAXTXSIZE;
		i++;
	} while(totalsize >= 0);	
	fclose(fp);
	pthread_mutex_unlock(&lock);

}



void calculate_chunk_size() {
	long feasiblecnt = userservcnt;
	int i;
	int availcnt = 0;
	fileaddress_t *tmpfileaddrp;
	int servernoarr[MAXLINES];
	int count;

	if (pthread_mutex_init(&lock, NULL) != 0)
    	{
        	printf("\n mutex init failed\n");
        	exit(1);
    	}

	/* logic to get chunk size of each server and client */
	if (availservcnt < userservcnt) {
	        printf("Sorry only %d servers available \n", availservcnt);
		feasiblecnt = availservcnt;
	}
	
	if (feasiblecnt == 1) {
		otherchunksize = 0;
		firstchunksize = filesize;
	}	
	else {
		otherchunksize = filesize/feasiblecnt;
		firstchunksize = filesize - ((feasiblecnt - 1) * otherchunksize);
	}	
	count = 0;
	for (i = 0; i < MAXLINES; i++) {
		if (count >= userservcnt)
			break;
		tmpfileaddrp = &fileaddrsp[i];
		if (tmpfileaddrp->servavail == 0) 
			continue;
		count++;
		tmpfileaddrp->cksize = otherchunksize;
		tmpfileaddrp->ckoff  = firstchunksize + ((availcnt - 1) * otherchunksize); 
		
		if (availcnt == 0) {
			tmpfileaddrp->cksize = firstchunksize;
			tmpfileaddrp->ckoff = 0;
		}
		availcnt++;
		servernoarr[i] = i;
		Pthread_create(&tmpfileaddrp->thread, NULL, &thread_func, (&servernoarr[i]));
	}
	count = 0;
	for (i = 0; i < MAXLINES; i++) {
		if (count >= userservcnt)
			break;		
		tmpfileaddrp = &fileaddrsp[i];
		if (tmpfileaddrp->thread == 0)
			continue;
		count++;
		Pthread_join(tmpfileaddrp->thread, NULL);				
	}
	pthread_mutex_destroy(&lock);

}

void *thread_func(void *servernop) {
	int *serverno = (int *)servernop;

	send_chunk_info(*serverno);
	get_file_data(*serverno); 
	return NULL;		
}

