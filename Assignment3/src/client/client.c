#include <common.h>
#include "cliutil.h"

#define USAGE "./myclient <file name> <no of connections>"
#define FILENAMESIZE 50

int main(int argc, char *argv[]) {
	const char *addrfilename = argv[1];
	
	validate_arg(argc, 3, USAGE);

	buffer = (char *)malloc(MAXBUFSIZE * sizeof(char));
	bzero(buffer, MAXBUFSIZE * sizeof(char));

	userservcnt = atoi(argv[2]);

	/* validate address and port provided by the user
	 * and fill in socket structures accordingly
	 */
	get_ipaddr_list(addrfilename); 

	/* check all servers that are available from the
	 * list and update structures.
	 */
	check_serv_avail(addrfilename);

	/* get filename from user and send it to all the
	 * the available servers
	 */	
	get_file_from_user();
	send_filename();

	/* get file status from first available server */		
	get_file_status();

	/* calculate number of threads to be created and
	 * the chunk size and offset required for each thread
	 */ 
	calculate_chunk_size();

	myfree(buffer);
	return 0;
}


