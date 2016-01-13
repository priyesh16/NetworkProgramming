#include <common.h>

#define USAGE "./myclient <ip address> <port>"
#define POLLTIMEOUT 10000
#define EXITCMD "exit"
#define BUFFSIZE 65535 

int main(int argc, char *argv[]){
	int sockfd = 0, n = 0;
	int ret;
	char sendBuff[BUFFSIZE];
	char cmdoutput[BUFFSIZE];
	struct sockaddr_in serv_addr; 
	unsigned long port;
	char cmd[100];
	struct pollfd pollst[1];
	char *end = NULL;
	int ptr = 0;

	if(argc != 3){
		usage(USAGE);
		return 1;
	}  

	if ((ret = retrieveport(argv[2], &port)) != 0) 
		return ret;

	memset(cmdoutput, '0',sizeof(cmdoutput));
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("\n Error : Could not create socket \n");
		return 1;
	} 

	pollst[0].fd = sockfd;
	pollst[0].events = POLLIN;

	memset(&serv_addr, '0', sizeof(serv_addr)); 
	memset(&sendBuff, '0', sizeof(sendBuff)); 
	memset(&cmdoutput, '0', sizeof(cmdoutput)); 

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port); 

	if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) {
		printf("\n inet_pton error occured\n");
		return 1;
	} 

	if(connect(sockfd, (struct sockaddr *)&serv_addr, 
		    sizeof(serv_addr)) < 0) {
		printf("\n Error : Connect Failed \n");
		return 1;
	} 

	while(1) {
		printf(PROMPT);
		fgets(cmd, sizeof(cmd), stdin);
		end = strrchr(cmd, '\n');
		if (end)
			*end = '\0';
		if (!strlen(cmd))
			continue;

		snprintf(sendBuff, strlen(cmd) + 1, "%s", cmd);
		n = write(sockfd, sendBuff, strlen(sendBuff));
		if(!strcmp(cmd, EXITCMD))
			break;
		ret = poll(pollst, 1, POLLTIMEOUT);
		while (pollst[0].revents & POLLIN) {	
			n = read(sockfd, cmdoutput + ptr, sizeof(cmdoutput)-ptr);
			if (n <= 0) {
				perror("read error:");
				break;
			}
			ptr += n;
			ret = poll(pollst, 1, 0);
			/*
			printf("%s", "asdffad");
			*/
		}
		cmdoutput[ptr] = '\0';
		printf("%s", cmdoutput);
		fflush(stdout);
	} 
	close(sockfd);
	return 0;
}
