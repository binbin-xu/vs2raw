/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2017 by Wenbin Li (wenbin.li@imperial.ac.uk)            *
 *                                                                         *
 *   https://www.doc.ic.ac.uk/~wl208                                       *
 *                                                                         *
 *   This code is licensed under the BSD-like License.                     *
 *                                                                         *
 ***************************************************************************/

#include "file_tools.h"
#include <fstream>

#if defined(_WIN32)
#include "Windows.h"
#include <direct.h>
#else
#include <sys/stat.h>
#endif

namespace minibr
{

bool file_exists(const std::string &path) throw()
{
    std::ifstream file(path);
    return file.good();
}

bool directory_exists(const std::string &path) throw()
{
#if defined(_WIN32)
    DWORD attributes = GetFileAttributes(path.c_str());
    return (attributes != INVALID_FILE_ATTRIBUTES && 
            (attributes & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat sb;
    return stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode);
#endif
}

void make_directory(const std::string &path) throw()
{
#if defined(_WIN32)
    _mkdir(path.c_str());
#else
    mkdir(path.c_str(), 0755);
#endif
}

}
