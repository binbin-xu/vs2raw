/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2017 by Wenbin Li (wenbin.li@imperial.ac.uk)            *
 *                                                                         *
 *   https://www.doc.ic.ac.uk/~wl208                                       *
 *                                                                         *
 *   This code is licensed under the BSD-like License.                     *
 *                                                                         *
 ***************************************************************************/

#ifndef STRING_TOOLS_H
#define STRING_TOOLS_H

#include <string>
#include <vector>

namespace minibr
{

std::string get_extension(const std::string &path);
std::string strip_extension(const std::string &path);

std::string get_filename(const std::string &full_path);
std::string strip_filename(const std::string &full_path);

std::string append_path(const std::string &root_path, const std::string &leaf_path);

std::vector<std::string> split_string(const std::string &s);
std::vector<std::string> split_path(const std::string& str);

std::string to_lower(const std::string &str);

bool find_any(const std::vector<std::string> &needles, const std::string &haystack);

}


#endif // STRING_TOOLS_H
