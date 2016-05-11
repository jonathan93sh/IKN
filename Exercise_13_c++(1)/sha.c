

const std::string getFile_sha_sum(std::string fileName);

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
