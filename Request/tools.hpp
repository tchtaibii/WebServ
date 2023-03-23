
#pragma once
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <sys/stat.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <stdlib.h>
#include <iomanip>
#include <sys/time.h>
namespace ws
{
    std::string get_PWD(char **env)
    {
        std::string tmp;
        int i = -1;
        while (env[++i])
        {
            if (env[i][0] == 'P')
            {
                tmp = env[i];
                size_t pos = tmp.find("PWD=", 0, 3);
                if (pos != std::string::npos)
                    break;
            }
        }
        return tmp.substr(4);
    }
    std::string readFileToString(const char *filename)
    {
        std::ifstream infile(filename);
        std::stringstream buffer;
        buffer << infile.rdbuf();
        return buffer.str();
    }
    std::string randomString(int length)
    {
        srand(time(NULL));                          // seed the random number generator with the current time
        std::string characters = "ABCDEF123456789"; // the characters to choose from
        std::string result;
        for (int i = 0; i < length; i++)
        {
            int randomIndex = rand() % characters.length(); // generate a random index
            result += characters[randomIndex];              // add the random character to the result string
        }
        return result;
    }
    std::string getCurrentDateTime()
    {
        time_t now = time(0);
        struct tm tstruct;
        char buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d-%H-%M-%S", &tstruct);
        struct timeval tv;
        gettimeofday(&tv, NULL);
        std::ostringstream oss;
        oss << buf << "." << std::setfill('0') << std::setw(3) << tv.tv_usec / 1000;
        return oss.str();
    }
    size_t countSubstring(const std::string &str, const std::string &sub)
    {
        size_t count = 0;
        size_t pos = str.find(sub);
        while (pos != std::string::npos)
        {
            count++;
            pos = str.find(sub, pos + 52);
        }
        return count;
    }
    bool is_hxa_low(char c)
    {
        std::string hexa = "0123456789abcdef";
        for (size_t i = 0; hexa[i]; i++)
        {
            if (hexa[i] == c)
                return 1;
        }
        return 0;
    }
    bool isHexadecimal(std::string str)
    {
        if (str.length() == 0)
            return 0;
        for (size_t i = 0; i < str.length(); i++)
        {
            if (is_hxa_low(str[i]))
                continue;
            else
                return false;
        }
        return true;
    }
    std::string pathjoin(std::string root, std::string &path)
    {
        std::string r;
        r = root + path.substr(1);
        return r;
    }

    bool fileExists(const std::string &filename)
    {
        struct stat buffer;
        return (stat(filename.c_str(), &buffer) == 0);
    }

    bool is_directory(const std::string &path)
    {
        struct stat status;
        if (stat(path.c_str(), &status) == 0)
            return (status.st_mode & S_IFDIR) != 0;
        return false;
    }
}
