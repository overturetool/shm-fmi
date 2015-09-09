/*
 * freeport.c
 *
 *  Created on: Sep 8, 2015
 *      Author: root
 */
#include "freeport.h"

unsigned short int getFreePort()
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		printf("socket error\n");
		return 0;
	}
	//printf("Opened %d\n", sock);

	struct sockaddr_in serv_addr;
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = 0;
	if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		/*if (errno == EADDRINUSE)
		 {
		 printf("the port is not available. already to other process\n");
		 return 0;
		 } else
		 {
		 printf("could not bind to process (%d) %s\n", errno,
		 strerror(errno));
		 return 0;
		 }*/
		return 0;
	}

	socklen_t len = sizeof(serv_addr);
	if (getsockname(sock, (struct sockaddr *) &serv_addr, &len) == -1)
	{
		perror("getsockname");
		return 0;
	}

	printf("port number %d\n", ntohs(serv_addr.sin_port));

	if (close(sock) < 0)
	{
		//		printf("did not close: %s\n", strerror(errno));
		return 0;
	}
	return serv_addr.sin_port;
}
