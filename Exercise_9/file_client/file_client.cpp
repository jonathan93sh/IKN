//============================================================================
// Name        : file_client.cpp
// Author      : Lars Mortensen
// Version     : 1.0
// Description : file_client in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <lib.h>

using namespace std;

int main(int argc, char *argv[])
{
	int socketfd, n, serverPort = PORT;
	char buffer[256], letter[] = {0};
	unsigned int length;
	struct sockaddr_in serv_addr, from;
	struct hostent *server;


	// syntax ./file_client <file_server’s ip-adr.> <[sti] + filnavn>
  	if (argc != 3)
    	error("For faa input argumenter");
  	
  	if(strcmp(argv[2],"u") == 0 || strcmp(argv[2],"U") == 0)
		strcpy(letter, "U\0");
	else if(strcmp(argv[2],"L") == 0 || strcmp(argv[2],"l") == 0)
		strcpy(letter, "L\0");
	else
		error("Der blev ikke skrevet u/U eller l/L");

	socketfd = socket(AF_INET,SOCK_DGRAM,0); //if socket fails it returns -1
	if(socketfd < 0)
	{
		close(socketfd);
		error("Kunne ikke oprette socket");
	}
	cout << "Der er oprettet forbindelse til socket" << endl;

	server = gethostbyname(argv[1]); //hvis systemet ikke kan finde navnet bliver server til NULL
	if (server == NULL)
	{
		close(socketfd);
		error("Den angivne host kan ikke findes");
	}

	//udfylder serv_addr structen med info
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr, server->h_length);
	serv_addr.sin_port = htons(serverPort);

	length = sizeof(struct sockaddr_in);

	n = sendto(socketfd, letter, strlen(letter), 0, (const struct sockaddr *)&serv_addr, length);
	if(n < 0)
		error("Beskeden kunne ikke sendes");
	
	n = recvfrom(socketfd, buffer, 256,0, (struct sockaddr *)&from, &length);
	if(n < 0)
		error("Beskeden kunne ikke modtages");

	//write(1, "Der blev modtaget: ", 9);
	//write(1, buffer, n);

	cout << "Succes: Socket lukkes" << endl;

  	close(socketfd);
  	return 0;
}