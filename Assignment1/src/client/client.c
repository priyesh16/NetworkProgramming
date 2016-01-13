#include <common.h>

#define USAGE "./myclient <ip address> <port>"

int main(int argc, char *argv[]){
	int sockfd = 0;
	struct sockaddr_in serv_addr; 
	unsigned long port;
	int ret = 0, n = 0;
	char cmd[CMDLEN];
	char buffer[CMDLEN];
	char cmdoutput[OUTLEN];
	char *ackstr;
	char *end = NULL;

	memset(&serv_addr, '0', sizeof(serv_addr)); 
	memset(&buffer, '0', sizeof(buffer)); 
	memset(&cmdoutput, '0', sizeof(cmdoutput)); 


	/* validate address and port provided by the user
	 * and fill in socket structures accordingly
	 */
	if(argc != 3){
		usage(USAGE);
		return -1;
	}  
	if ((ret = retrieveport(argv[2], &port)) != 0) 
		return ret;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port); 
	if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) {
		printf("\n inet_pton error occured\n");
		return 1;
	} 

	/* open a socket and connect to the server */
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("\n Error : Could not create socket \n");
		return 1;
	} 
	if(connect(sockfd, (struct sockaddr *)&serv_addr, 
		    sizeof(serv_addr)) < 0) {
		printf("\n Error : Connect Failed \n");
		return 1;
	} 

	/* get command from the user and send it to server,
	 * then read command output from server and print it
	 * back to the user.
	 */
	while(1) {
		printf(PROMPT);
		/* get user input, remove the new line character and
		 * if user doesn't enter a command continue
		 */
		fgets(cmd, sizeof(cmd), stdin);
		end = strrchr(cmd, '\n');
		if (end)
			*end = '\0';
		if (!strlen(cmd))
			continue;
		snprintf(buffer, strlen(cmd) + 1, "%s", cmd);
		n = write(sockfd, buffer, strlen(buffer));

		/* if user enters "exit" then quit, server on receiving
		 * the "exit" command will close the socket and open a
		 * new one for the next client.
		 */
		if(!strcmp(cmd, EXITCMD))
			break;

		/* read the whole cmd output from the server, the server
		 * demarks messages with an ack. So we are reading till we
		 * see an ack
		 */
		while (1) {	
			n = read(sockfd, cmdoutput, sizeof(cmdoutput));
			if (n <= 0) {
				perror("read error:");
				break;
			}
			cmdoutput[n] = '\0';
			ackstr = cmdoutput + strlen(cmdoutput) - strlen(ACK); 
			if(!strcmp(ackstr, ACK)) {
				cmdoutput[n - strlen(ACK)] = '\0';
				printf("%s", cmdoutput);
				break;
			}
			else {
				printf("%s", cmdoutput);
			}
			fflush(stdout);
		}
	} 
	close(sockfd);
	return 0;
}
