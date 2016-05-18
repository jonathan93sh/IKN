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
#include <Transport.h>

#define BUFSIZE 1000

using namespace std;

void receiveFile(char * fileName, Transport::Transport * socket);

int main(int argc, char *argv[])
{

	char buf[BUFSIZE];
 	short rcvSize = -1;

 	Transport::Transport socket(BUFSIZE);


	// syntax ./file_client <file_server’s ip-adr.> <[sti] + filnavn>
  	if (argc != 2)
    	error("For faa input argumenter");

  	if(check_File_Exists(extractFileName(argv[1])) != 0)
  		error("Filen findes allerede");
  	
	cout << "Der er oprettet forbindelse til serveren" << endl;



	receiveFile(argv[1],&socket);

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
void receiveFile(char * fileName, Transport::Transport * socket)
{	
 	char buf[BUFSIZE];
 	short rcvSize = -1;

 	bool retry = false;

 	do
 	{
 		//write to serveren
		socket->send(fileName, strlen(fileName)+1);
		cout << "Filnavn sendt til serveren" << endl;

		rcvSize = socket->receive(buf, BUFSIZE);
		long size;
		sscanf(buf, "%ld", &size);

	 	if (size <= 0)
	 	{
	 		cout << "fil findes ikke!" << endl;
	 		return;
	 	}
	 	cout << "file size: " << size << " bytes" << endl;

	 	rcvSize = socket->receive(buf, BUFSIZE);
	 	string sha256_string = buf;
	 	cout << "sha256 : " << sha256_string << endl;

	 	ofstream file; 
	 	file.open(extractFileName(fileName).c_str());

	 	double procent = 0;
	 	long i = 0;
	 	while(i<size)
	 	{
	 		rcvSize = socket->receive(buf,BUFSIZE);

	 		for(int a = 0; a < rcvSize; a++)
	 		{
	 			file.put(buf[a]);
		 		if(procent <= ((double)(i+a+1)/(double)size)*100)
				{
					cout << "procent done : " << procent << "%" << endl;
					procent+=25;
				}
	 		}
	 		i +=rcvSize;
	 	}
	 	cout << "done"  << endl;

		file.close();

		string modtaget_sha256_string = getFile_sha_sum(extractFileName(fileName));

		cout << "sha256 : " << modtaget_sha256_string << " modtaget" << endl;

		if(sha256_string != modtaget_sha256_string)
		{
			cout << "sha256 passer ikke!" << endl;
			retry = true;
			string nope = "It's not ok yet";

		}
		else
		{
			cout << "sha256 er ens" << endl;
			retry = false;
			string ok = "ok";
		}

	} while(retry);
}
