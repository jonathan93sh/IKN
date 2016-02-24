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

void receiveFile(string fileName, int socketfd);

int main(int argc, char *argv[])
{
	int socketfd, var;
	struct sockaddr_in serv_addr;
	struct hostent *server;

    if (argc < 3)
    	error("Too few input arguments");

	sockfd = socket(AF_INET,SOCK_STREAM,0); //if socket fails it returns -1
	if(sockfd < 0)
		error("Couldn't open socket");

	server = gethostbyname(argv[1]); //hvis systemet ikke kan finde navnet bliver server til NULL
	if (server == NULL)
		error("There is no such host");

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(PORT);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) //0 ved succes -1 ved fejl
		error("Coulnd't establish connection to server");
	
	//write argumenterne her
	
  	close(sockfd);
  	return 0;
}

/**
 * Modtager filstørrelsen og udskriver meddelelsen: "Filen findes ikke" hvis størrelsen = 0
 * ellers
 * Åbnes filen for at skrive de bytes som senere modtages fra serveren (HUSK kun selve filnavn)
 * Modtag og gem filen i blokke af 1000 bytes indtil alle bytes er modtaget.
 * Luk filen, samt input output streams
 *
 * @param fileName Det fulde filnavn incl. evt. stinavn
 * @param sockfd Stream for at skrive til/læse fra serveren
 */
void receiveFile(string fileName, int sockfd)
{

	// TO DO Your own code
}
