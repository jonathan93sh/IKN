#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <Transport.h>
#include <lib.h>
#include <file_client.h>

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
	short fileNameLength = sizeof(extractFileName(argv[1]));
  	Transport::Transport transpo = Transport::Transport(fileNameLength);
	// syntax ./file_client <[sti] + filnavn>
  	if (argc != 2)
    	error("For faa input argumenter");

  	if(check_File_Exists(extractFileName(argv[1])) != 0)
  		error("Filen findes allerede");

  	transpo.send(argv[1], fileNameLength);

  	std::cout << "Besked sendt til transportlageret" << std::endl;

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

 	do
 	{
 		short size;
 		char buf[size];
		do{
		 	size = transport->receive(buf, size);
		 }while(size < 0);
		 
	 	std::cout << "file size: " << size << " bytes" << std::endl;

	 	std::string sha256_string = buf;

	 	std::cout << "sha256 : " << sha256_string << std::endl;

	 	std::ofstream file; 
	 	file.open(fileName.c_str());

	 	double procent = 0;

	 	for(long i = 0; i<size; i++)
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
		}
		else
		{
			std::cout << "sha256 er ens" << std::endl;
			retry = false;
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
	/*
	new file_client(argc, argv);

	Transport::Transport test(1000);

	test.send("1234", 5);

	test.send("og igen", 8);

	test.send("1234", 5);

	test.send("Hej med dig", 12);

	char buf[1000];

	test.receive(buf,1000);

	std::cout << buf << std::endl;
	test.receive(buf,1000);

	std::cout << buf << std::endl;
	test.receive(buf,1000);

	std::cout << buf << std::endl;
	test.receive(buf,1000);

	std::cout << buf << std::endl;
	*/
	return 0;
}
