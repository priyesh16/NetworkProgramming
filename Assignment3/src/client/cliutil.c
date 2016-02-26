#include "cliutil.h"

int get_ipaddr_list(const char *filename) {
	int fd;
	fileaddress_t curfileaddr;
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

	printf("reading file ...\n");
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
		bzero(&curfileaddr, sizeof(fileaddress_t));

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

<<<<<<< HEAD
void check_serv_avail() {
	int sockfd;
	long int i = 0;
=======

/*
int get_udp_socket(unsigned long port) {
	int sockfd = 0;
	struct sockaddr_in serv_addr; 

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port); 

	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
	Bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	return sockfd;
}
*/
void check_serv_avail() {
	int sockfd;
	int i = 0;
>>>>>>> 11e74c0cdc029b8f29f6e56cfb8c95f1a1251990
	int err = SUCCESS;
	char address[MAXADDRSIZE];
	int count = 0;
	struct	sockaddr_in *addrp;
	struct sockaddr *castaddrp;
	struct sockaddr_in myaddr;

	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(0);

	Bind(sockfd, (struct sockaddr *)&myaddr, sizeof(struct sockaddr));
	printf("Connecting to available servers...(connect might take 1 sec) \n");
	for (i = 0; i < fileservcnt; i++) {	
	 	/* open a socket and connect to the server */
		addrp = &(fileaddrsp[i].addr);
		castaddrp = (struct sockaddr *)addrp;
<<<<<<< HEAD
		inet_ntop(AF_INET, &(addrp->sin_addr), address, INET_ADDRSTRLEN);

		send_ack(sockfd, buffer, castaddrp);
		err = get_ack(sockfd, buffer);
		if (err == 1) {			
=======
		send_ack(sockfd, buffer, castaddrp);
		err = get_ack(sockfd, buffer);
		printf("ack or not : %d \n", err);
		inet_ntop(AF_INET, &(addrp->sin_addr), address, INET_ADDRSTRLEN);
		if (err == 0) {			
>>>>>>> 11e74c0cdc029b8f29f6e56cfb8c95f1a1251990
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
	struct sockaddr *destaddr = (struct sockaddr *)&(fileaddrsp[serverno].addr);
	chunkinfo_t *infop = &(fileaddrsp[serverno].chunkinfo);

	printf("\nThread %d writing from offset %ld till %ld \n", serverno, infop->off, infop->off + infop->size); 
	create_buffer(CHUNKINFO, infop, buffer);
	Sendto(sockfd, buffer, MAXBUFSIZE, SENDFLAG, destaddr, SOCKADDRSZ);
}

void send_query_info(int serverno) {
	int sockfd = fileaddrsp[serverno].sockfd;
	struct sockaddr *destaddr = (struct sockaddr *)&(fileaddrsp[serverno].addr);
	chunkinfo_t *infop = &(fileaddrsp[serverno].chunkinfo);
<<<<<<< HEAD
	struct sockaddr_in *destaddr_inp= (struct sockaddr_in *)destaddr;
	unsigned long port = htons(destaddr_inp->sin_port);
	char address[20];


	inet_ntop(AF_INET, &(destaddr_inp->sin_addr), address, INET_ADDRSTRLEN);
		
	

	//printf("sending chunkinfo ... \n");		
	create_buffer(QUERYINFO, infop, buffer);
	Sendto(sockfd, buffer, MAXBUFSIZE, SENDFLAG, destaddr, SOCKADDRSZ);
	printf("sent chunkinfo to  %s:%lu ... \n", address, port);		

=======

	printf("sending chunkinfo ... \n");		
	create_buffer(QUERYINFO, infop, buffer);
	Sendto(sockfd, buffer, MAXBUFSIZE, SENDFLAG, destaddr, SOCKADDRSZ);
>>>>>>> 11e74c0cdc029b8f29f6e56cfb8c95f1a1251990
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
<<<<<<< HEAD
	int count = 0;
	fileaddress_t *tmpfileaddrp;
	struct sockaddr *destaddr;
	struct sockaddr_in *destaddr_inp; 
	short port; 
	char address[20];


=======
	int n;
	int count = 0;
	fileaddress_t *tmpfileaddrp;
	struct sockaddr *destaddr;

	printf("sending filename ... \n");		
>>>>>>> 11e74c0cdc029b8f29f6e56cfb8c95f1a1251990

	for (i = 0; i < MAXLINES; i++) {
		if (count >= userservcnt) 
			break;
		tmpfileaddrp = &fileaddrsp[i];
		if (tmpfileaddrp->servavail == 0)
			continue;
		count++;
		sockfd = fileaddrsp[i].sockfd;
		destaddr = (struct sockaddr *)&(fileaddrsp[i].addr);
<<<<<<< HEAD
		destaddr_inp = (struct sockaddr_in *)destaddr;
		port = htons(destaddr_inp->sin_port);
		inet_ntop(AF_INET, &(destaddr_inp->sin_addr), address, INET_ADDRSTRLEN);
		create_buffer(FILENAME, filename, buffer);
		Sendto(sockfd, buffer, MAXBUFSIZE, SENDFLAG, destaddr, SOCKADDRSZ);
		printf("sent filename to  %s:%u ... \n", address, port);		
=======
		printf("filename %s \n", filename);
		create_buffer(FILENAME, filename, buffer);
		n = sendto(sockfd, buffer, MAXBUFSIZE, SENDFLAG, destaddr, SOCKADDRSZ);
		printf("\n i %d bufsize %ld \n", n , strlen(buffer));
>>>>>>> 11e74c0cdc029b8f29f6e56cfb8c95f1a1251990
	}
}

void get_file_status() {
	FILE *fp;
	int sockfd; 
	tlv_t *bufstp = NULL;
	fileaddress_t *tmpfileaddrp;
	int i = 0;
	int n = 0;
	int count = 0;
	printf("getting file status ... \n");		
	for (i = 0; i < MAXLINES; i++) {
		if (count >= userservcnt)
			break;
		tmpfileaddrp = &fileaddrsp[i];
		
		if (tmpfileaddrp->servavail == 0)
			continue;
		count++;
		
		sockfd	= tmpfileaddrp->sockfd;
		send_query_info(i);
		sleep(1);
		//get_ack(sockfd, buffer);
	
		n = Read(sockfd, buffer, MAXBUFSIZE);
		if (n == 0) {
			printf("Getting file status failed, try again \n");
			exit(-1);
		}
		retrieve_buffer(buffer, &bufstp);
		if (bufstp->buf_err == 0) {
			fileinforx = 1;
			break;	 
		}
		else {
			myfree(bufstp); 
			printf(FILEINVALIDSTR);
			exit(1);
		}
	}

	if (bufstp == NULL) {
		printf("Getting file status failed, try again \n");
		exit(-1);
	}
	filesize = bufstp->buf_cksize;
	filefd = Open(filename, O_CREAT|O_WRONLY|O_TRUNC, FILE_MODE);
	ftruncate(filefd, filesize); 
	fp = fdopen(filefd, "w");
	fclose(fp);
	myfree(bufstp);
}


<<<<<<< HEAD
=======
void get_file_data(int serverno) {
	FILE *fp;
	size_t count = 0;
	chunkinfo_t *infop = &(fileaddrsp[serverno].chunkinfo);
	long totalsize = infop->size;
	long leftover = totalsize;
	long seek = infop->off;
	int i = 1;
	char buf[PACKETSIZE + 1];
	int sockfd = fileaddrsp[serverno].sockfd;
	char *data = buf;
	long start = seek;
	long end = seek;
	

	printf("getting file data ...\n");
	pthread_mutex_lock(&lock);
	filefd = Open(filename, O_CREAT|O_WRONLY, FILE_MODE);
	fp = fdopen(filefd, "w");

	fseek(fp, seek, SEEK_SET);

	printf("tx %lu\n", HEADERLEN);
	do {
		bzero(buf, PACKETSIZE + 1);
		bzero(buffer, MAXBUFSIZE);
		data = buf;
		if (leftover - MAXTXSIZE < 0)
			count = leftover;
		else
			count = MAXTXSIZE;
		start = end;
		if (count != MAXTXSIZE)
			end = totalsize;
		else
			end = seek + (i * count);

		data = get_packet_ident(sockfd, buf, data, start, end);
 	
		Write(filefd, data, count);
		leftover -= MAXTXSIZE;
		i++;
	} while(leftover >= 0);	
	fclose(fp);
	pthread_mutex_unlock(&lock);

}

char *get_packet_ident(int sockfd, char *buf, char *data, int start, int end) {
	long ident;

	Read(sockfd, buf,  PACKETSIZE);
	printf("type %s \n", data);
	data += sizeof(type_t);
	printf("len %s \n", data);
	data += sizeof(size_t);
	printf("err %s \n", data); 	
	data += sizeof(int);
	printf("ident %s \n", data);	
	ident = atol(data); 	
	data += sizeof(unsigned long);
	printf("string %s \t total %lu \n", data, strlen(data)); 
	printf("Read packet %lu from %lu to %lu \n", ident, start, end); 
	return data;
}

>>>>>>> 11e74c0cdc029b8f29f6e56cfb8c95f1a1251990

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
<<<<<<< HEAD
}

void *thread_func(void *servernop) {

	int *serverno = (int *)servernop;
	pthread_mutex_lock(&lock);
	send_chunk_info(*serverno);
	get_file_data(*serverno); 
	pthread_mutex_unlock(&lock);
	return NULL;		
}


void get_file_data(int serverno) {
	FILE *fp;
	size_t count = 0;
	chunkinfo_t *infop = &(fileaddrsp[serverno].chunkinfo);
	long totalsize = infop->size;
	long leftover = totalsize;
	long newsize;
	long newoff;
	long seek = infop->off;
	long savedseek;
	long i = 1;
	long ident;
	long savedident;
	char buf[PACKETSIZE + 1];
	char *data = buf;
	long start = seek;
	long end = seek;
	int retransmit = 0;
	int once = 1;
	char u = 'u';
	char dummystr[MAXTXSIZE];
	//int locked = 0;

	memset(dummystr, u, MAXTXSIZE);
	dummystr[MAXTXSIZE] = '\0';
	
	//printf("getting file data ...\n");
	//if (locked == 0)
		//pthread_mutex_lock(&lock);
	filefd = Open(filename, O_CREAT|O_WRONLY, FILE_MODE);
	fp = fdopen(filefd, "w");

	fseek(fp, seek, SEEK_SET);

	do {
		bzero(buf, PACKETSIZE + 1);
		bzero(buffer, MAXBUFSIZE);
		data = buf;
		
		if (leftover - MAXTXSIZE < 0)
			count = leftover;
		else
			count = MAXTXSIZE;
		start = end;
		if (count != MAXTXSIZE)
			end = seek + totalsize;
		else
			end = seek + (i * count);
		data = get_packet_ident(serverno, buf, data, &start, &end, &ident, &i, &retransmit);
		if (data == NULL)
			break;
		if (retransmit == 1) {
			if (once == 1) {
				strcat(dummystr, data);
				data = dummystr;
				count = count + MAXTXSIZE;
				leftover -= MAXTXSIZE;
				savedseek = seek;
				savedident = ident - 2;
				//printf("savedidnent %d %d\n", ident, savedident);
			}
			once = 0;
		}
		Write(filefd, data, count);
		leftover -= MAXTXSIZE;
		i++;
	} while(leftover > 0);	
	if (retransmit == 1) {
		printf("Packet recieved out of order, retransmitting ... \n");
		//printf("seek %lu %lu \n", savedseek, savedident);
		newoff = savedseek + (savedident * MAXTXSIZE);
		newsize = MAXTXSIZE;
		//printf("newsize %d %d \n", newoff, newsize);
		fileaddrsp[serverno].cksize = newsize;
		fileaddrsp[serverno].ckoff = newoff;			 
		//locked = 1;
		send_chunk_info(serverno);
		get_file_data(serverno);
	}
	fclose(fp);
	//locked = 0;
	//pthread_mutex_unlock(&lock);
}

char *get_packet_ident(int serverno, char *buf, char *data, long *start, long *end, long *ident, long *i, int *retransmit) {
	int n = 0;
	fd_set readfd;
	struct timeval tv;
	int sockfd = fileaddrsp[serverno].sockfd;
	int maxfd = sockfd + 1;
	struct sockaddr_in *destaddr_inp; 
	short port; 
	char address[20];

	destaddr_inp = &(fileaddrsp[serverno].addr);
	port = htons(destaddr_inp->sin_port);
	inet_ntop(AF_INET, &(destaddr_inp->sin_addr), address, INET_ADDRSTRLEN);

	FD_ZERO(&readfd);
	FD_SET(sockfd, &readfd);
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	Select(maxfd, &readfd, NULL, NULL, &tv);

	if (FD_ISSET(sockfd, &readfd))
		n = Read(sockfd, buf,  PACKETSIZE);
	if (n == 0) {
		printf("Nothing more to read ... \n");
		return NULL;	
	}
		
	//printf("type %s \n", data);
	data += sizeof(type_t);
	//printf("len %s \n", data);
	data += sizeof(size_t);
	//printf("err %s \n", data); 	
	data += sizeof(int);
	//printf("ident %s \n", data);	
	*ident = atol(data); 	
	data += sizeof(unsigned long);
	//printf("string %s \t total %lu \n", data, strlen(data)); 
	if (*i != *ident ) {
		//printf("inside i = ident %d %d \n", *i , *ident);
		*i += 1;
		*start += MAXTXSIZE;
		*end += MAXTXSIZE;
		*retransmit = 1;
	}
	printf("Read packet %lu from %lu to %lu from %s:%d\n", *ident, *start, *end, address, port); 

	return data;
}


=======

}

void *thread_func(void *servernop) {
	int *serverno = (int *)servernop;

	send_chunk_info(*serverno);
	get_file_data(*serverno); 
	return NULL;		
}

>>>>>>> 11e74c0cdc029b8f29f6e56cfb8c95f1a1251990
