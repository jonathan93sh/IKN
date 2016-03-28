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
#include <sys/time.h>

using namespace std;

int main(int argc, char *argv[])
{
 	int length, n, serverPort = PORT;
	socklen_t from_length;
 	sockaddr_in addr_s, addr_c;
 	char buf[1024];

 	int sockfd = socket(AF_INET, SOCK_DGRAM,0);

 	if(sockfd<0)
	{
		close(sockfd);
		error("Kunne ikke oprette velkommen socket!\n");
	}
	cout << "Socket oprettet" << endl;

	length = sizeof(addr_s);
	bzero(&addr_s, length);


	addr_s.sin_family = AF_INET;
    addr_s.sin_addr.s_addr = INADDR_ANY;
    addr_s.sin_port = htons(serverPort);
    if(bind(sockfd,(sockaddr*) &addr_s, sizeof(addr_s)))
	{
		close(sockfd);
		error("Kunne ikke binde sig til server socket!\n");
	}

	cout << "forbundet til socket" << endl;

	from_length = sizeof(struct sockaddr_in);


	while(1)
	{
		n =recvfrom(sockfd, buf, 1024, 0,(struct sockaddr *)&addr_c, &from_length);
		if (n < 0)error("fejl i recvfrom");

		cout << "har modtaget noget" << endl;


			if(buf[0] == 'U' || buf[0] == 'u')
			{
				ifstream file; 
				file.open("/proc/uptime", fstream::in);
				char filebuf[1024];
				file.getline(filebuf, 1024);
				file.close();
				cout << filebuf << endl;
				n = sendto(sockfd, filebuf, strlen(filebuf)+1, 0, (struct sockaddr *)&addr_c, from_length);
				if (n < 0)error("fejl i sendto");
			}
			else if(buf[0] == 'L' || buf[0] == 'l')
			{
				ifstream file; 
				file.open("/proc/loadavg", fstream::in);
				char filebuf[1024];
				file.getline(filebuf, 1024);
				file.close();
				cout << filebuf << endl;
				n = sendto(sockfd, filebuf, strlen(filebuf)+1, 0, (struct sockaddr *)&addr_c, from_length);
				if (n < 0)error("fejl i sendto");
			}


	}
	return 0;
}
