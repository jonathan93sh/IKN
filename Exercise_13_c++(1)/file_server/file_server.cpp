#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <Transport.h>
#include <lib.h>
#include <file_server.h>

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
void file_server::sendFile(std::string fileName, long fileSize, Transport::Transport *transport)
{
	// To do Your own code
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

	return 0;
}
