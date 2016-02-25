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
	int socketfd, var, serverPort = PORT;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	// syntax ./file_client <file_server’s ip-adr.> <[sti] + filnavn>
  	if (argc < 2)
    	error("For faa input argumenter");

  	if(check_File_Exists(extractFileName(argv[1])) != 0)
  		error("Filen findes allerede");

	sockfd = socket(AF_INET,SOCK_STREAM,0); //if socket fails it returns -1
	if(sockfd < 0)
	{
		close(sockfd);
		error("Kunne ikke oprette socket");
	}

	server = gethostbyname(argv[0]); //hvis systemet ikke kan finde navnet bliver server til NULL
	if (server == NULL)
	{
		close(sockfd);
		error("Den angivne host kan ikke findes");
	}

	//udfylder serv_addr structen med info
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(serverPort);

	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) //0 ved succes -1 ved fejl
	{
		close(sockfd);
		error("Kunne ikke oprette forbindelse til serveren");
	}

	//write to serveren
	var = writeTextTCP(argv[1], sockfd);
	if(var < 0)
		error("Kunne ikke skrive til socketen");

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
 	bool retry = false;

 	do
 	{
	 	char buf;

	 	long size = getFileSizeTCP(sockfd);
	 	string md5 = readTextTCP(fileName,sockfd);

	 	ofstream file; 
	 	file.open(fileName);

	 	for(long i = 0; i<size; i++)
	 	{
	 		read(sockfd,&buf, 1);
	 		file.write(buf);
	 	}

		file.close();
		if(md5 != getFile_md5_sum(fileName))
		{
			retry = true;
			string nope = "It's not ok yet";
			write(sockfd,nope.c_str(), nope.size());
		}
		else
		{
			retry = false;
			string ok = "ok";
			write(sockfd,ok.c_str(), ok.size());
		}

	} while(retry);
}
