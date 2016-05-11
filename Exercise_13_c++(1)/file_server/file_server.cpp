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
	// TO DO Your own code
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

		transport->send(buf,streamSize);
		//streamSize = write(outToClient, buf, streamSize);

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
