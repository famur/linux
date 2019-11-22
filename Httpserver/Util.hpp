#pragma once

#include<iostream>
#include<algorithm>
#include<string>
#include<sstream>
#include<sstream>
#include<sstream>

class Util{
    public:
        static void StringToUpper(std::string &s)
        {
            std::transform(s.begin(), s.end(), s.begin(), ::toupper);
        }

        static void StringToLower(std::string &s)
        {
            std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        }
        static void TansfromToVector(std::string &s, std::vector<std::string> &v)
        {
            size_t start = 0;
            while(1)
            {
               size_t pos = s.find("\n", start);
               if(std::string::npos == pos)
               {
                   break;
               }
               std::string sub_str = s.substr(start, pos - start);
               v.push_back(sub_str);
               start = pos + 1;
            }
        }
        static void MakeKV(std::string s, std::string &key, std::string &value)
        {
            size_t pos = s.find(": ", 0);
            if(std::string::npos == pos)
            {
                return ;
            }
            key = s.substr(0, pos);
            value = s.substr(pos + 2);
        }
        static int StringToInt(std::string s)
        { 
            std:: stringstream ss(s);
            int result = 0;
            ss >> result;
            return result;
        }
};
