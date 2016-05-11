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

#ifdef __cplusplus
extern "C" {
#endif

 const std::string getFile_sha_sum(std::string fileName);

#ifdef __cplusplus
}
#endif

void error(const std::string msg);
const std::string extractFileName(const std::string fileName);
const long check_File_Exists(std::string fileName);

#endif /* LIB_H_ */