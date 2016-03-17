#include "router.h"


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


void print_nbr_table() {
	int i;

	for (i = 0; i < mynbrcnt; i++) {
		printf("Neighbour:Cost:Address:Port %c:%d:%x:%d \n", mynbr[i].nbr_name, mynbr[i].nbr_cost, mynbr[i].nbraddr, mynbr[i].nbrport);
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
		if (!strchr(token, selfnode.node_name))
			mynbr[mynbrcnt].nbr_name = token[0];
		else {
			token = strtok(NULL, " ");
			mynbr[mynbrcnt].nbr_name = token[0];
		}
		token = strtok(NULL, " ");
		mynbr[mynbrcnt].nbr_cost = atoi(token);
		mynbrcnt++;
	}
}

void get_nbr_info() {
	FILE *fp; 
	char *token;
	char line[MAXLINELEN];
	char *buffer;
	char *ip;
	int i;

	if ((fp = fopen(NODE_CONF_FILE, "r")) == NULL)  
		perror("get_nbr_config: Open Error");

	// Read file line by line
	while(fgets(line, MAXLINELEN, fp)) {
		if (strchr(line, '#')) 
			continue;
		if (line[0] == myname) {
			buffer = strdup(line);
			token = strtok(buffer, " "); // Tokenized name
			token = strtok(NULL, " "); // Tokenized address
			token = strtok(NULL, " "); // Tokenized port
			retrieve_port(token, &selfnode.node_port);
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
	sockaddr.sin_port        = htons(myport);

	mysock = Socket(AF_INET, SOCK_DGRAM, 0);

	Bind(mysock, (SA *)&sockaddr, SOCKADDRSZ);
}
int main(int argc, char **argv) {
	/* Validate and retrieve the port from input arguments
	 * and open a socket bound to the port
	 */
	validate_arg(argc, 3, USAGE);
	selfnode.node_name = argv[1][0];

	retrieve_port(argv[2], &selfnode.node_port);

	printf("\nMy Node Details: %c:%d \n", myname, selfnode.node_port);
	
	// Get neighbour details from the neibour config file 
	get_nbrs();
	get_nbr_info();

	// Create and bind a socket
	create_socket();
	
	exit(SUCCESS);
}
