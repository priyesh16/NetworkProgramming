#include <common.h>

#define USAGE "\n ./myserver <port no (1-65535)> \n"

int main(int argc, char **argv){
	int ret = SUCCESS; 
	int err = SUCCESS; 
	unsigned long port;
	int sockfd;
	char cmd[CMDLEN];
	int saved_stdout;
	FILE *dummyfp;
	char output[OUTLEN];
	ssize_t n;
	int nextclient = 1;
	socklen_t len = sizeof(err);

	bzero(cmd, sizeof(cmd));

	/* validate and retrieve the port from input arguments
	 * and open a socket bound to the port
	 */
	validatearg(argc, 2, USAGE);
	retrieveport(argv[1], &port);

	/*
	 * connect to a client, accept commands from it
	 * and sent it the output of the command
	 */
		
	while(1){
		len = sizeof(err);
		err = SUCCESS; 
		/* connect to a new client */
		if (nextclient == 1) {
			sockfd = getlistensocket(port);
			nextclient = 0;
		}

		/* read command to be executed from the client, if
		 * read fails for reasons like the client hit Ctrl+C
		 * then connect to next client.
		 */
		printf(PROMPT);
		fflush(stdout);
		if((n = read(sockfd, cmd, sizeof(cmd) - 1)) <= 0){
			close(sockfd);
			perror("read:");
			nextclient = 1;
			continue;
		}
		cmd[n] = '\0';
		printf("%s\n", cmd);

		/* if exit command was sent close connection with 
		 * the current client and open a new connection
		 */
		n = strcmp(cmd, EXITCMD);
		if (n == 0) {
			nextclient = 1;
			close(sockfd);
			continue;
		}
		
		/* if command doesn't have produce an output then
		 * let the client know
		 */
		dummyfp = popen(cmd, "r");
		if(!fgets(output, sizeof(output), dummyfp)){
			write(sockfd, NOOUT, strlen(NOOUT)); 
			write(sockfd, ACK, strlen(ACK)); 
			continue;
		}
		pclose(dummyfp);

		/* print command output on server side also */
		system(cmd);

		/* instead of writing output to stdout write 
		 * directly to socket and then send an ack
		 */
		saved_stdout = dup(STDOUT_FILENO);
		dup2(sockfd, STDOUT_FILENO);
		system(cmd);
		sendack();
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdout);
	}

	/* close socket descriptor and exit */
	close(sockfd);
	exit(SUCCESS);
}
