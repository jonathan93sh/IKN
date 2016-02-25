/*
 * lib.h
 *
 *  Created on: Sep 8, 2012
 *      Author: Lars Mortensen
 */

#ifndef LIB_H_
#define LIB_H_

#include <iostream>
<<<<<<< HEAD
=======
#include <openssl/md5.h>
>>>>>>> ace4e1fa37f92696df7e5dc6440b9b4c4e6286e9

#define PORT 9000
#define BUFSIZE 1000

void error(const std::string msg);
const std::string extractFileName(const std::string fileName);
const std::string readTextTCP(std::string inText, int inFromServer);
void writeTextTCP(std::string line, int outToServer);
const long getFileSizeTCP(int inFromServer);
const long check_File_Exists(std::string fileName);
const std::string getFile_md5_sum(std::string fileName);

#endif /* LIB_H_ */
