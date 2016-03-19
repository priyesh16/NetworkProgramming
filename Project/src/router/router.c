#include "router.h"

mytimer_t rxtimer = TIMER_INIT;

/* check if ip address is valid */
int is_ip_addr(char *addr) {
	struct sockaddr_in sa;
	return inet_pton(AF_INET, addr, &(sa.sin_addr));
}

/* check if the port number is valid */
int is_port(unsigned long port) {
	int err = SUCCESS;

	if (port <= 0 || port >= MAXPORTNO){
		printf("port %lu out of range", port);
		return ERANGE;
	}
	return err;
}

/* given a string input of port number convert to an unsigned long */
void retrieve_port(const char* portstr, unsigned short *portno) {
	unsigned short port;
	char *endptr;
	int err = SUCCESS;

	port = strtoul(portstr, &endptr, BASE);
	if((err = is_port(port)) != SUCCESS)
	err_sys("%s \n ", "Invalid Port");
	*portno = port;
}
							
/* print usage */
void usage(char *usage) {
	printf("usage is %s \n", usage);
}
						

/* validate the number of arguments and print useage
 *  if invalid
 */
void validate_arg(int argc, int max, char *useage){
	if (argc != max){
		usage(useage);
		err_sys("%s \n ", "Invalid No. of Arguments");
	}
}

void bellmanford(int n,int v) {	
	int i, j, k, u;

	// Fill up cost table
	for (i = 1; i < totalnodes; i++) {
		for (j = 1; j < totalnodes; j++) {
			cost[i][j] = 17;
		}
	}

	// Run Bellman Ford
	for(i = 1; i <= n; i++)
		distance[i] = cost[v][i];

	for(k = 2; k <= n-1; k++) {
		for(u = 1; u <= n; u++) {
			if(u == v)
				continue;
			for(i = 1; i<=n; i++) {
				if(i == v)
					continue;
				if(distance[u] > distance[i] + cost[i][u])
					distance[u] = distance[i] + cost[i][u];
			}
		}
	}
}

void print_nbr_table() {
	int i;
	char address[MAXADDRSIZE];
	int cost;
	time_t secs = time(NULL);
	struct tm *parse = localtime(&secs);
	char str[MAXTIMESIZE];
	unsigned short port;
	char name;

	strftime(str, 100, "%A, %B %d %Y%l:%M %p", parse);
	printf("%s: \n", str);
	printf("Neighbour\tCost\t\tAddress\t\tPort\n");
	for (i = 0; i < mynbrcnt; i++) {
		strcpy(address, inet_ntoa(mynbr[i].nbraddr));
		cost = mynbr[i].nbr_cost;
		name = mynbr[i].nbr_name;
		port = mynbr[i].nbrport;

		if (cost >= 17)
			printf("\t%c\t%s\t%s\t%d \n", name, "infinity", address, port);
		else
			printf("\t%c\t%d\t\t%s\t%d \n", name, cost, address, port);
	}
}

void get_nbrs() {
	FILE *fp; 
	char *token;
	char buffer[MAXLINELEN];

	mynbrcnt = 0;

	if ((fp = fopen(NBR_CONF_FILE, "r")) == NULL)  
		perror("get_nbr_config: Open Error");

	// Read file line by line
	while(fgets(buffer, sizeof(buffer), fp)) {
		if ((strchr(buffer, '#')) || (!strchr(buffer, selfnode.node_name)))
			continue;

		token = strtok(buffer, " ");
		if (!strchr(token, selfnode.node_name)) {
			mynbr[mynbrcnt].nbr_name = token[0];
			token = strtok(NULL, " ");
		}
		else {
			token = strtok(NULL, " ");
			mynbr[mynbrcnt].nbr_name = token[0];
		}
		token = strtok(NULL, " ");
		mynbr[mynbrcnt].nbr_cost = atoi(token);
		mynbr[mynbrcnt].nbr_oldcost = atoi(token);
		mynbrcnt++;
	}
}

void start_communication() {
	int i;
	int nbr[MAXNBRS];

	Pthread_create(&(mysthread), NULL, &parthread_rx, NULL);
	Pthread_create(&(mysthread), NULL, &parthread_timer, NULL);
	for (i = 0; i < mynbrcnt; i++) {
		nbr[i] = i;
		Pthread_create(&(mynbr[i].thread), NULL, &thread_func, &nbr[i]);
	}
	for (i = 0; i < mynbrcnt; i++) {
		Pthread_join(mynbr[i].thread, NULL);
	}
}

void timer_callback(time_t now) {
	int i;

	sleep(TIMER_CB);
	
	for (i = 0; i < mynbrcnt; i++) {
		if (mynbr[i].nbr_stillrx == 1) {
			if (mynbr[i].nbr_alive != ALIVE) {
				mynbr[i].nbr_cost = mynbr[i].nbr_oldcost;
				mynbr[i].nbr_alive = ALIVE;
				printf("%c is alive \n", mynbr[i].nbr_name);
				print_nbr_table();
			}
		}
		else {
			if (mynbr[i].nbr_alive != DEAD) {
				mynbr[i].nbr_cost = INFINITY;
				mynbr[i].nbr_alive = DEAD;
				printf("%c is dead \n", mynbr[i].nbr_name);
				print_nbr_table();
			}
		}
	}
}

void *parthread_timer(void *nbr) {
	fd_set readfd;
	struct timeval tv;
	int maxfd = mysock + 1;

	timer_start_periodic(&rxtimer, PER_TIMER, timer_callback);

	while(1) {
		tv_init(&tv);
		tv_timer(&tv, &rxtimer);
		FD_ZERO(&readfd);
		Select(maxfd, &readfd, NULL, NULL, &tv);
		timer_check(&rxtimer);
	}
	return NULL;
}


void *parthread_rx(void *nbr) {
	struct sockaddr_in dstsockaddr;
	int i;
	fd_set readfd;
	struct timeval tv;
	int maxfd = mysock + 1;
	unsigned short dstport;
	struct in_addr dstaddr;
	socklen_t size;
	char buffer[MAXBUFSIZE];

	while(1) {
		FD_ZERO(&readfd);
		FD_SET(mysock, &readfd);
		tv.tv_sec = RX_TIMER;
		tv.tv_usec = 0;
		Select(maxfd, &readfd, NULL, NULL, &tv);
		if (!FD_ISSET(mysock, &readfd)) {
			for (i = 0; i < mynbrcnt; i++) {
				mynbr[i].nbr_retry--;
				if (mynbr[i].nbr_retry < 0) {
						mynbr[i].nbr_stillrx = 0;
						mynbr[i].nbr_retry = RX_RETRYTIMER;
				}
			}
			continue;
		}
		Recvfrom(mysock, buffer, MAXBUFSIZE, RECVFLAG, 
				(struct sockaddr *)&dstsockaddr, &size);
		dstaddr = dstsockaddr.sin_addr;
		dstport = dstsockaddr.sin_port;
		for (i = 0; i < mynbrcnt; i++) {
			mynbr[i].nbr_retry--;
			if (mynbr[i].nbr_retry < 0) {
				mynbr[i].nbr_stillrx = 0;
				mynbr[i].nbr_retry = RX_RETRYTIMER;
			}
			if ((mynbr[i].nbraddr.s_addr == dstaddr.s_addr) && 
					(mynbr[i].nbrport == dstport)) {
				//printf("Got from %c \n", mynbr[i].nbr_name); 
				mynbr[i].nbr_stillrx = 1;
			}
		}
	}
	return NULL;
}


void *thread_func(void *nbr) {
	int nbrno = *(int *)nbr;
	char buffer[MAXBUFSIZE];
	struct sockaddr *destaddrp = 
		(struct sockaddr *)&(mynbr[nbrno].sockaddr);
	//int n;

	while(1) {
		bzero(buffer, MAXBUFSIZE);
		sprintf(buffer, "%d", mynbr[nbrno].nbr_cost);
		Sendto(mysock, buffer, strlen(buffer) + 1, 
				SENDFLAG, destaddrp, SOCKADDRSZ);
		//if (n)
		//	printf("Sent %d to %x:%d\n", atoi(buffer), ((struct sockaddr_in *)destaddrp)->sin_addr, ((struct sockaddr_in *)destaddrp)->sin_port);
		sleep(TX_TIMER);
	}
	return NULL;
}

void get_nbr_info() {
	FILE *fp; 
	char *token;
	char line[MAXLINELEN];
	char *buffer;
	char *ip;
	int i;

	totalnodes = 0;
	if ((fp = fopen(NODE_CONF_FILE, "r")) == NULL)  
		perror("get_nbr_config: Open Error");

	// Read file line by line
	while(fgets(line, MAXLINELEN, fp)) {
		if (strchr(line, '#')) 
			continue;
		totalnodes++;
		if (line[0] == myname) {
			buffer = strdup(line);
			token = strtok(buffer, " "); // Tokenized name
			token = strtok(NULL, " "); // Tokenized address
			token = strtok(NULL, " "); // Tokenized port
			retrieve_port(token, &selfnode.node_port);
			printf("\nMy Node Details: %c:%d \n", myname, selfnode.node_port);
			continue;
		}

		for (i = 0; i < mynbrcnt; i++) {	
			buffer = strdup(line);
			token = strtok(buffer, " ");
			if (token[0] != mynbr[i].nbr_name)
				continue;
			mynbr[mynbrcnt].nbrfam = AF_INET;
			token = strtok(NULL, " ");
			ip = strdup(token);
			inet_aton(ip, &(mynbr[i].nbraddr));
			token = strtok(NULL, " ");
			retrieve_port(token, &(mynbr[i].nbrport));
			free(buffer);
			break;
		}
	}
	print_nbr_table();
}

void create_socket() {
	struct sockaddr_in sockaddr = {0};

	sockaddr.sin_family      = AF_INET;
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddr.sin_port        = myport;

	mysock = Socket(AF_INET, SOCK_DGRAM, 0);

	Bind(mysock, (SA *)&sockaddr, SOCKADDRSZ);
}

int main(int argc, char **argv) {
	/* Validate and retrieve the port from input arguments
	 * and open a socket bound to the port
	 */
	validate_arg(argc, 3, USAGE);
	selfnode.node_name = argv[1][0];

	// Get neighbour details from the neibour config file 
	get_nbrs();
	get_nbr_info();
	// Create and bind a socket
	create_socket();
	start_communication();

	
	return 0;
}
