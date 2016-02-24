//============================================================================
// Name        : file_server.cpp
// Author      : Lars Mortensen
// Version     : 1.0
// Description : file_server in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <lib.h>
#include <iostream>
#include <fstream>


using namespace std;

void sendFile(string fileName, long fileSize, int outToClient);

/**
 * main starter serveren og venter på en forbindelse fra en klient
 * Læser filnavn som kommer fra klienten.
 * Undersøger om filens findes på serveren.
 * Sender filstørrelsen tilbage til klienten (0 = Filens findes ikke)
 * Hvis filen findes sendes den nu til klienten
 *
 * HUSK at lukke forbindelsen til klienten og filen nÃ¥r denne er sendt til klienten
 *
 * @throws IOException
 *
 */
int main(int argc, char *argv[])
{
 	int serverPort = PORT;
 	sockaddr_in addr_s, addr_c;

 //	struct sockaddr_in {
 //       short   sin_family;
 //       u_short sin_port;
 //       struct  in_addr sin_addr;
 //       char    sin_zero[8];
//	};

	addr_s.sin_family = AF_INET;
    addr_s.sin_addr.s_addr = INADDR_ANY;
    addr_s.sin_port = htons(serverPort);

	int sockfd = socket(AF_INET, SOCK_STREAM,0);
	if(sockfd<0)
	{
		error("Kunne ikke oprette velkommen socket!\n");
	}
	if(bind(sockfd,(sockaddr*) &addr_s, sizeof(addr_s)))
	{
		error("Kunne ikke binde sig til server socket!\n");
	}

	listen(sockfd, 1); // 1 = max 1 klient

	socklen_t clilen = sizeof(addr_c);

	int newSockfd = accept(sockfd,(sockaddr*) &addr_c, &clilen);

	if(newSockfd<0)
	{
		error("Kunne ikke oprette forbindelse til klient!\n");
	}

	string file = readTextTCP("", newSockfd);

	long size = check_File_Exists(file);

	char buf_string;

	string Ssize;



	Ssize = size;

	write(newSockfd, Ssize.c_str(), Ssize.size()+1);

	if(size == 0)
	{
		close(newSockfd);
		close(sockfd);
		error("Filen findes ikke!\n");
	}

	sendFile(file, size, newSockfd);

	return 0;
}

/**
 * Sender filen som har navnet fileName til klienten
 *
 * @param fileName Filnavn som skal sendes til klienten
 * @param fileSize Størrelsen på filen, 0 hvis den ikke findes
 * @param outToClient Stream som der skrives til socket
	 */
void sendFile(string fileName, long fileSize, int outToClient)
{
	char buf[BUFSIZE];
	long point = 0;


	ifstream file; 
	file.open(fileName.c_str(), fstream::in);

	while(point < fileSize)
	{
		int streamSize = BUFSIZE;
		if((point + BUFSIZE) > fileSize)
		{
			streamSize = fileSize -point;
		}

		file.get(buf, streamSize);
		write(outToClient, buf, streamSize);

		point += streamSize;

	} 

	file.close();
		 
}

