#include <common.h>

#define USAGE "\n ./myserver <port no (1-65535)> \n"
#define CMDLEN  100
#define OUTLEN  1000
#define EXITCMD "exit"
#define NOOUT   "No Output"


int validatearg(int argc){

	if (argc != 2){
		printf("%s \n ", "Invalid No. of Arguments");
		usage(USAGE);
		return EINVAL;
	}
	
	return SUCCESS;
}

int main(int argc, char **argv){
	int ret = SUCCESS; 
	unsigned long port;
	int sockfd;
	char sendBuff[OUTLEN];
	char cmd[CMDLEN];
	int saved_stdout;
	FILE *dummyfp;
	char output[OUTLEN];
	ssize_t n;

    memset(sendBuff, '0', sizeof(sendBuff));
	memset(cmd, '0', sizeof(cmd));

	/* validate and retrieve the port from input arguments
	 * and open a socket bound to the port
	 */

	if ((ret = validatearg(argc)) != 0)
		return ret;
	if ((ret = retrieveport(argv[1], &port)) != 0)
		return ret;

	sockfd = getlistensocket(port);

	/*
	 * Till client requests to exit or until error keep
	 * accepting commands from clients and send back the 
	 * output of the executed command
	 */
		
	while(1){
		if((n = read(sockfd, cmd, sizeof(cmd) - 1)) < 0){
			perror("read:");
			break;
		}
		cmd[n] = '\0';
		printf("\ncommand to execute %s \n", cmd);

		/* exit if client requests it */
		n = strcmp(cmd, EXITCMD);
		if (n == 0)
			break;
		
		/* if command doesn't have produce an output then
		 * let the client know
		 */
		dummyfp = popen(cmd, "r");
		if(!fgets(output, sizeof(output), dummyfp)){
			write(sockfd, NOOUT, strlen(NOOUT)); 
			continue;
		}

		/* print command output on server side also */
		system(cmd);
		fflush(stdout);

		/* instead of writing output to stdout write 
		 * directly to socket
		 */
		saved_stdout = dup(STDOUT_FILENO);
		dup2(sockfd, STDOUT_FILENO);
		system(cmd);
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdout);
	}

	close(sockfd);
	exit(SUCCESS);
}
