/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2017 by Wenbin Li (wenbin.li@imperial.ac.uk)            *
 *                                                                         *
 *   https://www.doc.ic.ac.uk/~wl208                                       *
 *                                                                         *
 *   This code is licensed under the BSD-like License.                     *
 *                                                                         *
 ***************************************************************************/

#ifndef FILE_TOOLS_H
#define FILE_TOOLS_H

#include <string>

namespace minibr
{

bool file_exists(const std::string &path) throw();
bool directory_exists(const std::string &path) throw();
void make_directory(const std::string &path) throw();

}

#endif
