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

 	long test = check_File_Exists("fil1");

 	cout << test << endl;

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
		close(sockfd);
		error("Kunne ikke oprette velkommen socket!\n");
	}

	cout << "Socket oprettet" << endl;

	if(bind(sockfd,(sockaddr*) &addr_s, sizeof(addr_s)))
	{
		close(sockfd);
		error("Kunne ikke binde sig til server socket!\n");
	}

	cout << "forbundet til socket" << endl;

	listen(sockfd, 5); // 5 = max 5 klient



	socklen_t clilen = sizeof(addr_c);

	while(1)
	{
		cout << "venter på klient" << endl;
		int newSockfd = accept(sockfd,(sockaddr*) &addr_c, &clilen);

		if(newSockfd<0)
		{
			close(newSockfd);
			cout << "Kunne ikke oprette forbindelse til klient!" << endl;
		}
		else
		{
			bool retry = false;
			int retrys = 0;
			cout << "en klient har oprettet forbindelse til denne server!" << endl;
			string file = readTextTCP("", newSockfd);

			cout << "klienten vil gerne hente filen: " << file << endl;

			do
			{
				if(retry)
				{
					cout << "fejl " << file << endl;

				}
				retry = false;

				long size = check_File_Exists(file);

				char charSize[50];

				sprintf(charSize, "%ld", size);

				cout << "file size : " << size << " bytes" << endl;

				write(newSockfd, charSize, strlen(charSize));

				cout << "sendt size til klient" << endl;

				string MD5_str = getFile_md5_sum(file);

				write(newSockfd, MD5_str.c_str(), MD5_str.size());

				if(size == 0)
				{
					cout << "Filen findes ikke!" << endl;
				}
				else
				{
					sendFile(file, size, newSockfd);
					string check = readTextTCP("", newSockfd);

					if(check != "ok")
					{
						retry = true;
						retrys++;
					}


					cout << "filen er sendt lukker socket mellem server og klient" << endl;
				}
			}while(retry && retrys < 5);

		}
		close(newSockfd);
	}

	close(sockfd);
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
		cout << streamSize << endl;



		//file.get(buf, streamSize);

		for(int i = 0; i < streamSize; i++)
		{
			file.get(buf[i]);
		}

		write(outToClient, buf, streamSize);

		point += streamSize;

	} 

	file.close();
		 
}

