//
//  TrimString.h
//  snowcrash
//
//  Created by Zdenek Nemec on 5/11/13.
//  Copyright (c) 2013 Apiary Inc. All rights reserved.
//
//  Credits:
//  http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

#ifndef SNOWCRAH_TRIMSTRING_H
#define SNOWCRAH_TRIMSTRING_H

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <string>
#include <sstream>
#include <vector>

namespace snowcrash {

    // Trim string from start
    inline std::string& TrimStringStart(std::string &s) {
#ifdef _WIN32
        s.erase(0, s.find_first_not_of(' '));
#else
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
#endif
        return s;
    }
    
    // Trim string from end
    inline std::string& TrimStringEnd(std::string &s) {
#ifdef _WIN32
        s.erase(s.find_last_not_of(' ')+1);
#else
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
#endif
        return s;
    }
    
    // Trim both ends of string
    inline std::string& TrimString(std::string &s) {
        return TrimStringStart(TrimStringEnd(s));
    }
    
    // Retrieve first line of given string
    inline std::string GetFirstLine(const std::string& s) {
        std::string::size_type pos = s.find("\n");
        if (pos == std::string::npos)
            return s;
        else
            return s.substr(0, pos);
    }
    
    // Split string by delim
    inline std::vector<std::string>& Split(const std::string& s, char delim, std::vector<std::string>& elems) {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }
    
    // Split string by delim    
    inline std::vector<std::string> Split(const std::string& s, char delim) {
        std::vector<std::string> elems;
        Split(s, delim, elems);
        return elems;
    }
    
    // Split string on the first occurrence of delim
    inline std::vector<std::string> SplitOnFirst(const std::string& s, char delim) {
        std::string::size_type pos = s.find(delim);
        std::vector<std::string> elems;
        if (pos == std::string::npos) {
            elems.push_back(s);
        }
        else {
            elems.push_back(s.substr(0, pos));
            elems.push_back(s.substr(pos + 1, std::string::npos));
        }
        return elems;
    }
    
    
    /**
     *  \brief  Replace all occurrences of a string.
     *  \param  s       A string to search in.
     *  \param  find    A string to look for.
     *  \param  replace A string to replace with.
     *  \return A copy of %s with all occurrences of %find replaced by %replace.
     */
    inline std::string ReplaceString(const std::string& s,
                                     const std::string& find,
                                     const std::string& replace) {
        size_t pos = 0;
        std::string target(s);
        while ((pos = target.find(find, pos)) != std::string::npos) {
            target.replace(pos, find.length(), replace);
            pos += replace.length();
        }
        return target;
    }
}

#endif
