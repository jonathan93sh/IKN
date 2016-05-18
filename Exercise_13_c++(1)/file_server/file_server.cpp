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
#include <netinet/in.h>
#include <lib.h>
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <Transport.h>

#define TIME_OUT 5000

#define BUFSIZE 1000

using namespace std;

bool sendFile(string fileName, long fileSize, Transport::Transport * socket);

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

 	char buf[BUFSIZE];
 	short rcvSize = -1;

 	Transport::Transport socket(BUFSIZE);
 	cout << "Server is running" << endl;
	while(1)
	{
		socket.restart();
		rcvSize = socket.receive(buf, BUFSIZE);
		string file = buf;
		cout << "en klient har oprettet forbindelse til denne server!" << endl;
		cout << "klienten vil gerne hente filen: " << file << endl;

		long size = check_File_Exists(file);
		sprintf(buf, "%ld", size);
		cout << "file size : " << size << " bytes" << endl;
		socket.send(buf, strlen(buf)+1);
		cout << "sendt size til klient" << endl;

		if(size >= 1)
		{

			string sha256_str = getFile_sha_sum(file);
			cout << "sha256 : " << sha256_str << endl;
			socket.send(sha256_str.c_str(), sha256_str.size()+1);

			if(sendFile(file, size, &socket))
			{
				cout << "filen er sendt" << endl;
			}
		}
		else
		{
			cout << "filen findes ikke" << endl;
		}
	}
		
	return 0;
}

/**
 * Sender filen som har navnet fileName til klienten
 *
 * @param fileName Filnavn som skal sendes til klienten
 * @param fileSize Størrelsen på filen, 0 hvis den ikke findes
 * @param outToClient Stream som der skrives til socket
	 */
bool sendFile(string fileName, long fileSize, Transport::Transport * socket)
{
	char buf[BUFSIZE];
	long point = 0;


	ifstream file; 
	file.open(fileName.c_str(), fstream::in);
	double procent = 0;
	while(point < fileSize)
	{
		int streamSize = BUFSIZE;
		if((point + BUFSIZE) > fileSize)
		{
			streamSize = fileSize -point;
		}
			
		for(int i = 0; i < streamSize; i++)
		{
			
			file.get(buf[i]);
		}

		socket->send(buf, streamSize);

		point += streamSize;
		if(procent <= ((double)point/(double)fileSize)*100)
		{
			cout << "procent done : " << procent << "%" << endl;
			procent+=25;
		}
	} 

	file.close();
	return true;		 
}

