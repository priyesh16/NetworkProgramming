#include <common.h>
#include "cliutil.h"

#define USAGE "./myclient <ip address> <port>"
#define FILENAMESIZE 50

long firstchunksize;
long otherchunksize;

void calculate_chunk_size() {
	long feasiblecnt = userservcnt;
	int i;
	int availcnt = 0;
	int ret = SUCCESS;
	fileaddress_t *tmpfileaddrp;
	int first = 0;
	int servernoarr[MAXLINES];
	printf("%ld", filesize);
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

	printf("calculate_ck_size ?? %ld %ld %ld\n", filesize, otherchunksize, firstchunksize);
	for (i = 0; i < userservcnt; i++) {
		tmpfileaddrp = &fileaddrsp[i];
		if (tmpfileaddrp->servavail == 0)
			continue;
		tmpfileaddrp->cksize = otherchunksize;
		tmpfileaddrp->ckoff  = firstchunksize + ((availcnt - 1) * otherchunksize); 
		
		if (availcnt == 0) {
			tmpfileaddrp->cksize = firstchunksize;
			tmpfileaddrp->ckoff = 0;
		}
		printf("calculate_ck_size %ld %ld \n", tmpfileaddrp->cksize, tmpfileaddrp->ckoff);
		availcnt++;
		servernoarr[i] = i;
		Pthread_create(&tmpfileaddrp->thread, NULL, thread_func, (&servernoarr[i]));
	}
	for (i = 0; i < userservcnt; i++) {
		tmpfileaddrp = &fileaddrsp[i];
		if (tmpfileaddrp->thread == 0)
			continue;
		Pthread_join(tmpfileaddrp->thread, NULL);				
	}
}

void thread_func(int *serverno) {
	printf("\nthread_func\n");
	printf("thread_func sockfd %d %d \n", *serverno, fileaddrsp[*serverno].sockfd);
	
	send_chunk_info(*serverno);
	get_file_data(*serverno); 		
}

int main(int argc, char *argv[]){
	tlv_t *bufstp;
	int count = 0;
	char address[10];
	int i;
	const char *addrfilename = argv[1];
	chunkinfo_t threadckinfo;

	if (pthread_mutex_init(&lock, NULL) != 0)
    	{
        	printf("\n mutex init failed\n");
        	return 1;
    	}

	buffer = (char *)malloc(MAXBUFSIZE * sizeof(char));
	bzero(buffer, MAXBUFSIZE * sizeof(char));

	userservcnt = atoi(argv[2]);

	/* validate address and port provided by the user
	 * and fill in socket structures accordingly
	 */
	validate_arg(argc, 3, USAGE);
	get_ipaddr_list(addrfilename); 
	check_serv_avail(addrfilename);
	printf("main 1 \n");
	
	get_file_from_user();
	printf("main 2 \n");
	
	send_filename();
	printf("main 3 \n");
	
	get_file_status();
	printf("main 4 \n");
	calculate_chunk_size();
	printf("4");
	pthread_mutex_destroy(&lock);
	return 0;
}
