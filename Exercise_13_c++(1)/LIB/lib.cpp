
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
	return fileName.substr(fileName.find_last_of("/")+1);
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
	if ((stat (fileName.c_str(), &sts)) == -1)
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

		if(size <= bufSize)
		{
			tempBufSize = size;
		}else if(i + tempBufSize >= size)
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