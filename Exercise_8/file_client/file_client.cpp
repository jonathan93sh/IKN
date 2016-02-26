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
  	if (argc != 3)
    	error("For faa input argumenter");

  	if(check_File_Exists(extractFileName(argv[2])) != 0)
  		error("Filen findes allerede");

	socketfd = socket(AF_INET,SOCK_STREAM,0); //if socket fails it returns -1
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
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(serverPort);

	if (connect(socketfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) //0 ved succes -1 ved fejl
	{
		close(socketfd);
		error("Kunne ikke oprette forbindelse til serveren");
	}

	cout << "Der er oprettet forbindelse til serveren" << endl;

	//write to serveren
	writeTextTCP(argv[2], socketfd);

	cout << "Filnavn sendt til serveren" << endl;

	receiveFile(extractFileName(argv[2]),socketfd);

  	close(socketfd);
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

	 	cout << "file size: " << size << " bytes" << endl;

	 	string sha256_string = readTextTCP("",sockfd);

	 	cout << "sha256 : " << sha256_string << endl;

	 	ofstream file; 
	 	file.open(fileName.c_str());

	 	double procent = 0;

	 	for(long i = 0; i<size; i++)
	 	{
	 		read(sockfd,&buf, 1);
	 		file.put(buf);
	 		if(procent <= ((double)(i+1)/(double)size)*100)
			{
				cout << "procent done : " << procent << "%" << endl;
				procent+=25;
			}
	 	}
	 	cout << "done"  << endl;

		file.close();

		string modtaget_sha256_string = getFile_sha_sum(fileName);

		cout << "sha256 : " << modtaget_sha256_string << " modtaget" << endl;

		if(sha256_string != modtaget_sha256_string)
		{
			cout << "sha256 passer ikke!" << endl;
			retry = true;
			string nope = "It's not ok yet";

			write(sockfd,nope.c_str(), nope.size()+1);
		}
		else
		{
			cout << "sha256 er ens" << endl;
			retry = false;
			string ok = "ok";
			write(sockfd,ok.c_str(), ok.size()+1);
		}

	} while(retry);
}
