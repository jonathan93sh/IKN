#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <Transport.h>
#include <lib.h>
#include <file_server.h>

using namespace std;

/// <summary>
/// The BUFSIZE
/// </summary>
#define BUFSIZE 1000

/// <summary>
/// Initializes a new instance of the <see cref="file_server"/> class.
/// </summary>
file_server::file_server ()
{
	Transport::Transport transpo(BUFSIZE);
	char buf[BUFSIZE];
	int n;
	
	bzero(buf,BUFSIZE);
	
	n = transport.receive(buf, BUFSIZE); 
	if(n<0)
	{
		cout << "Intet modtaget" << endl;
	}
	string fileName = string(buf);
	
	cout << "Klienten vil gerne hente filen: " << fileName << endl;
	
	long size = check_File_Exists(fileName);

	if(size <= 0)
	{
		cout << "No file" << endl;
		//break;
	}
	
	cout << "sendt size til klient" << endl;
	string sha256_str = getFile_sha_sum(fileName);
	cout << "sha256 : " << sha256_str << endl;
	
	if(size == 0)
	{
		error("Filen findes ikke!");
	}
	else
	{
		if(sendFile(fileName, size, &transpo))
		{
			transport->receive(buffer,BUFSIZE);
			string check = string(buffer);

			if(check != "ok")
			{
				retry = true;
				retrys++;
				cout << "fejl i transsmision" << endl;
			}
			else
			{
				cout << "Filen er sendt lukker socket mellem server og klient" << endl;
			}
		}
		// else
		// {
		//		break;
		// }
	}
}

/// <summary>
/// Sends the file.
/// </summary>
/// <param name='fileName'>
/// File name.
/// </param>
/// <param name='fileSize'>
/// File size.
/// </param>
/// <param name='transport'>
/// Transport lag.
/// </param>
bool file_server::sendFile(std::string fileName, long fileSize, Transport::Transport *transport)
{
	char buffer[BUFSIZE];
	long point = 0;

	sprintf(buffer, "%ld",fileSize);
	transport->send(buffer, strlen(buffer));

	bzero(buffer,BUFSIZE);

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
			file.get(buffer[i]);
		}

		streamSize = transport->send(buffer,streamSize);
		//streamSize = write(outToClient, buffer, streamSize);

		//cout << "sendt : " << streamSize << " point: " << point << "/" << fileSize << endl;

		if(streamSize == -1)
		{
			cout << "fail lost connection to client!!" << endl;
			file.close();
			return false;
		}

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

/// <summary>
/// The entry point of the program, where the program control starts and ends.
/// </summary>
/// <param name='args'>
/// The command-line arguments.
/// </param>
int main(int argc, char **argv)
{
	new file_server();
	
	
	/*
	char buf[1000];

	Transport::Transport test(1000);

	test.receive(buf,1000);

	std::cout << buf << std::endl;

	test.receive(buf,1000);

	std::cout << buf << std::endl;

	test.receive(buf,1000);

	std::cout << buf << std::endl;

	test.receive(buf,1000);

	std::cout << buf << std::endl;


	test.send("Hej med dig", 12);

	test.send("Hej med dig ;) 1", 17);

	test.send("Hej med dig ;) 2", 17);

	test.send("Hej med dig ;) 3", 17);
	*/
	return 0;
}
