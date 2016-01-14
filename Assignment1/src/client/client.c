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

	bzero(&serv_addr, sizeof(serv_addr)); 
	bzero(buffer, sizeof(buffer)); 
	bzero(cmdoutput, sizeof(cmdoutput)); 

	/* validate address and port provided by the user
	 * and fill in socket structures accordingly
	 */
	validatearg(argc, 3, USAGE);
	retrieveport(argv[2], &port);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port); 
	if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) {
		printf("\n inet_pton error occured\n");
		return 1;
	} 

	/* open a socket and connect to the server */
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	Connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

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
	Close(sockfd);
	return 0;
}
