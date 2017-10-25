/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2017 by Wenbin Li (wenbin.li@imperial.ac.uk)            *
 *                                                                         *
 *   https://www.doc.ic.ac.uk/~wl208                                       *
 *                                                                         *
 *   This code is licensed under the BSD-like License.                     *
 *                                                                         *
 ***************************************************************************/

#include "string_tools.h"

#include <sstream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <iterator>
#include <set>

#include <cctype>

namespace minibr
{

std::string get_extension(const std::string &path)
{
    return path.substr(path.find_last_of(".") + 1);
}

std::string strip_extension(const std::string &path)
{
    return path.substr(0, path.find_last_of("."));
}

std::string get_filename(const std::string &full_path)
{
    size_t position = full_path.find_last_of("/\\");
    return full_path.substr(position + 1);
}

std::string strip_filename(const std::string &full_path)
{
    size_t position = full_path.find_last_of("/\\");
    if (position == std::string::npos)
        return "";

    return full_path.substr(0, position);
}

std::string append_path(const std::string &root_path, const std::string &leaf_path)
{
    std::stringstream pathstream;
    pathstream << root_path;
    if (!root_path.empty())
        pathstream << "/";
    pathstream << leaf_path;

    std::string merged_path = pathstream.str();
    std::replace(merged_path.begin(), merged_path.end(), '\\', '/');
    return merged_path;
}

std::vector<std::string> split_string(const std::string &s)
{
    std::stringstream ss(s);
    std::vector<std::string> tokens;
    while (ss.good())
    {
        std::string token;
        ss >> token;
        tokens.push_back(token);
    }
    return tokens;
}

std::vector<std::string> split_path(const std::string& str)
{
    std::set<char> delimiters{'/'};
  std::vector<std::string> result;

  char const* pch = str.c_str();
  char const* start = pch;
  for(; *pch; ++pch)
  {
    if (delimiters.find(*pch) != delimiters.end())
    {
      if (start != pch)
      {
        std::string str(start, pch);
        result.push_back(str);
      }
      else
      {
        result.push_back("");
      }
      start = pch + 1;
    }
  }
  result.push_back(start);

  return result;
}

std::string to_lower(const std::string &str)
{
    std::string out;
    out.reserve(str.length());

    for (size_t i = 0; i < str.length(); ++i)
        out.push_back(tolower(str[i]));

    return out;
}

bool find_any(const std::vector<std::string> &needles, const std::string &haystack)
{
    for (std::string needle : needles)
    {
        if (haystack.find(needle) != std::string::npos)
            return true;
    }
    
    return false;
}

}
