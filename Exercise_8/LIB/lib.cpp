
/*
 * lib.c
 *
 *  Created on: Sep 8, 2012
 *      Author: Lars Mortensen
 */
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>
#include <lib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>


 extern "C" {
#include <openssl/sha.h>
}

/**
 * Udskriver fejlmeddelelse og stoppe programmet
 *
 * @param msg Fejlmeddelelse
 */
void error(const std::string msg)
{
    perror(msg.c_str());
    exit(0);
}

/**
 * Udtrækker et filnavn fra en tekststreng
 *
 * @param fileName Filnavn incl. evt. stinavn
 * @return Kun filnavn
 */
const std::string extractFileName(const std::string fileName)
{
	return fileName.substr(fileName.find_last_of("/\\")+1);
}

/**
 * Læser en tekststreng fra en socket
 *
 * @param inFromServer stream that holds data from server
 * @return En streng modtaget fra en socket
 */
const std::string readTextTCP(std::string inText, int inFromServer)
{
    char ch;

    read(inFromServer, &ch, 1);
    while(ch != 0)
    {
   		inText += ch;
        read(inFromServer, &ch, 1);
    }
    return inText;
}

/**
 * Skriver en tekststreng til en socket med 0 terminering
 *
 * @param outToServer Stream hvortil der skrives data til socket
 * @param line Teksten der skal skrives til socket
 */
void writeTextTCP(std::string line, int outToServer)
{
	write(outToServer, line.c_str(), line.length());
	write(outToServer, "\0", 1);
}

/**
 * Konverter en streng som indeholder filstÃ¸rrelsen in ascii format til en talvÃ¦rdi
 *
 * @param inFromServer Stream som indeholder data fra socket
 * @return Filstørrelsen som talværdi
 */
const long getFileSizeTCP(int inFromServer)
{
	std::string fileName = "";
   	return atol(readTextTCP(fileName, inFromServer).c_str());
}

/**
 * Se om filen fileName eksisterer
 *
 * @param fileName Filnavn
 * @return Størrelsen på filen, 0 hvis den ikke findes
 */
const long check_File_Exists(std::string fileName)
{
	struct stat sts;
	if ((stat (fileName.c_str(), &sts)) != 0)
		return 0;

	return sts.st_size;
}


const std::string getFile_sha_sum(std::string fileName)
{
	char result[65];
	std::string sha_sum_str = "";
	unsigned char* file_buffer;
	long size = check_File_Exists(fileName);
	if(size == 0)
		return "";

	int file_descript = open(fileName.c_str(), O_RDONLY);
	if(file_descript < 0)
		return "";

	
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	const int bufSize = 32768;
	file_buffer = (unsigned char*)mmap(0,(unsigned long) size, PROT_READ, MAP_SHARED, file_descript, 0);

	for(long i = 0; i < size; i+=bufSize)
	{
		int tempBufSize = bufSize;
		if(i + tempBufSize >= size)
		{
			tempBufSize = size - (i + tempBufSize);
		}

		SHA256_Update(&sha256, &file_buffer[i], tempBufSize);

	} 

	SHA256_Final(hash, &sha256);

	for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		char buf[5];
		sprintf(buf, "%02x", hash[i]);
		sha_sum_str += buf;
	}
	munmap(file_buffer, (unsigned long) size);
	return sha_sum_str;

}

	/*
const std::string getFile_md5_sum(std::string fileName)
{
	unsigned char result[MD5_DIGEST_LENGTH];
	int file_descript;
	void* file_buffer;
	std::string md5_sum_str = "";

	long size = check_File_Exists(fileName);
	if(size == 0)
		return "";

	file_descript = open(fileName.c_str(), O_RDONLY);
	if(file_descript < 0)
		return "";

	file_buffer = mmap(0,(unsigned long) size, PROT_READ, MAP_SHARED, file_descript, 0);
	MD5((const unsigned char*) file_buffer, (unsigned long) size, result);
	char buf[3];
	for(int i = 0; i << MD5_DIGEST_LENGTH; i++)
	{
		sprintf(buf, "%02x", result[i]);
		md5_sum_str += buf;
	}
	munmap(file_buffer, (unsigned long) size);
	return md5_sum_str;
}
	*/
