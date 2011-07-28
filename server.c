#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "config.h"
#include "errreport.h"

extern int get_local_address(struct sockaddr_in *addr_out);
extern int addr_convert(char* addr, struct in_addr *addr_out); 

int main(int argc, char** argv)
{
	int n_socketfd = 0;
	struct sockaddr_in serv_addr;
	
	// init socket
	n_socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (0 > n_socketfd)
	{
		PRINT_ERR("init socket error")
		PRINT_ERR(strerror(errno))
		exit(EXIT_FAILURE);
	} // end of if

	sprintf(test_buf, "%s, %s", argv[0], argv[1]);
	PRINT_TEST(test_buf)
	
	// init address data	
	if (argc >= 2 &&
		FUC_SUCCESS == addr_convert(argv[1], &serv_addr.sin_addr)
		);
	else
	{ // if can't use arguments, use local address instead
		PRINT_ERR("can not recognize the ip address or hostname")
		PRINT_ERR("use local address")

		bzero(&serv_addr, sizeof(serv_addr));
		if (FUC_FAILURE == get_local_address(&serv_addr))
		{
			PRINT_ERR("can not get local address")
			exit(EXIT_FAILURE);	
		} // end of if
	} // end of else
	
	// bind socket to address
	if (bind(n_socketfd, 
			(struct sockaddr*)&serv_addr, 
			sizeof(serv_addr)) < 0)
	{
		PRINT_ERR("bind error")
		PRINT_ERR(strerror(errno))
		exit(EXIT_FAILURE);
	} // end of if

	// change socket to listen state
	if (listen(n_socketfd, BACK_LOG) < 0)
	{
		PRINT_ERR("listen error")
		PRINT_ERR(strerror(errno))
		exit(EXIT_FAILURE);
	} // end of if
	
	return EXIT_SUCCESS;
} // end of main


/*
 * Function: get local ip and port,
 * 	success return FUC_SUCCESS, failure return FUC_FAILURE.
 */
int get_local_address(struct sockaddr_in *addr_out)
{
	addr_out->sin_family = AF_INET;
	addr_out->sin_port = htons(PORT);
	addr_out->sin_addr.s_addr = htonl(INADDR_ANY);
////if (0 == inet_aton(IP, &(addr_out->sin_addr)))
////{
////	PRINT_ERR(strerror(errno))
////	return FUC_FAILURE;
////} // end of if
	return FUC_SUCCESS;
} // end of get_local_address()


/*
 * Function: convert the string of ip address or hostname
 * 	to struct inaddr, success return FUC_SUCCESS,
 * 	failure return FUC_FAILURE
 */
int addr_convert(char *addr, struct in_addr *addr_out)
{
	struct hostent *he;

	if (1 == inet_aton(addr, addr_out))
	{
		return FUC_SUCCESS;
	} // end of if

	he = gethostbyname(addr);
	if (NULL != he)
	{
		*addr_out = *((struct in_addr *)(he->h_addr_list[0]));
		return FUC_SUCCESS;
	} // end of if 

	return FUC_FAILURE;
} // end of addr_convert()
