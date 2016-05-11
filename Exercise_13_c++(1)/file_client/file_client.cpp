#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <Transport.h>
#include <lib.h>
#include <file_client.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

/// <summary>
/// The BUFSIZE
/// </summary>
#define BUFSIZE 1000

/// <summary>
/// Initializes a new instance of the <see cref="file_client"/> class.
/// 
/// file_client metoden opretter en peer-to-peer forbindelse
/// Sender en forspørgsel for en bestemt fil om denne findes på serveren
/// Modtager filen hvis denne findes eller en besked om at den ikke findes (jvf. protokol beskrivelse)
/// Lukker alle streams og den modtagede fil
/// Udskriver en fejl-meddelelse hvis ikke antal argumenter er rigtige
/// </summary>
/// <param name='args'>
/// Filnavn med evtuelle sti.
/// </param>
file_client::file_client(int argc, char **argv)
{
  	Transport::Transport transpo(BUFSIZE);

  	receiveFile(extractFileName(argv[1]),&transpo);
}

/// <summary>
/// Receives the file.
/// </summary>
/// <param name='fileName'>
/// File name.
/// </param>
/// <param name='transport'>
/// Transportlaget
/// </param>
void file_client::receiveFile (std::string fileName, Transport::Transport *transport)
{
	bool retry = false;
  	char buf[BUFSIZE];
  	short size, fileSize;

 	do
 	{
 	  	transport->send(fileName.c_str(),fileName.length()+1);
 
		do{
			bzero(buf,BUFSIZE);
			size = transport->receive(buf, BUFSIZE);
			//size = atol(buf);
		 }while(size <= 0);
		 
	 	std::cout << "file size: " << size << " bytes" << std::endl;

	 	std::string sha256_string = buf;

	 	std::cout << "sha256 : " << sha256_string << std::endl;

	 	std::ofstream file;
	 	file.open(fileName.c_str());

	 	double procent = 0;
		fileSize = size;
		
	 	for(long i = 0; i<fileSize; i++)
	 	{
	 		transport->receive(buf,size);
	 		file.put(*buf);
	 		if(procent <= ((double)(i+1)/(double)size)*100)
			{
				std::cout << "procent done : " << procent << "%" << std::endl;
				procent+=25;
			}
	 	}
	 	std::cout << "done"  << std::endl;

		file.close();
	 	
		std::string modtaget_sha256_string = getFile_sha_sum(fileName);

		std::cout << "sha256 : " << modtaget_sha256_string << " modtaget" << std::endl;

		if(sha256_string != modtaget_sha256_string)
		{
			std::cout << "sha256 passer ikke!" << std::endl;
			retry = true;
			std::string nope = "It's not ok yet";

			transport->send(nope.c_str(), nope.size()+1);
		}
		else
		{
			std::cout << "sha256 er ens" << std::endl;
			retry = false;
			std::string ok = "ok";

			transport->send(ok.c_str(), ok.size()+1);
		}

	} while(retry);
}		

/// <summary>
/// The entry point of the program, where the program control starts and ends.
/// </summary>
/// <param name='args'>
/// First argument: Filname
/// </param>
int main(int argc, char** argv)
{
	// syntax ./file_client <[sti] + filnavn>
  	if (argc != 2)
    	error("Forkert antal input argumenter");

  	if(check_File_Exists(extractFileName(argv[1])) != 0)
  		error("Filen findes allerede");

	new file_client(argc, argv);  	

	return 0;
}
