/*
 * lib.h
 *
 *  Created on: Sep 8, 2012
 *      Author: Lars Mortensen
 */

#ifndef LIB_H_
#define LIB_H_

#include <iostream>
 extern "C"{
#include <openssl/sha.h>
}

void error(const std::string msg);
const std::string extractFileName(const std::string fileName);
const long check_File_Exists(std::string fileName);
const std::string getFile_sha_sum(std::string fileName);

#endif /* LIB_H_ */