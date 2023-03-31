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
#include <dirent.h>
namespace ws
{
    std::map<std::string, std::string> parseKeyValuePairs(std::string input, char delimiter = ';')
    {
        input.insert(input.length(), 1, ';');
        std::map<std::string, std::string> output;
        size_t i = 0;
        size_t delimiterPos = input.find(delimiter);
        while (delimiterPos != std::string::npos)
        {
            std::string keyValueString = input.substr(0, delimiterPos - 0);
            size_t equalsPos = keyValueString.find('=');
            if (equalsPos != std::string::npos)
            {
                std::string key = keyValueString.substr(0, equalsPos);
                std::string value = keyValueString.substr(equalsPos + 1);
                if (i != 0)
                    key = key.substr(1);
                output[key] = value;
            }
            input = input.substr(delimiterPos + 1);
            delimiterPos = input.find(delimiter);
            i++;
        }
        return output;
    }
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
    void writeToFile(const std::string &filename, const std::string &text, bool createIfNotExist = true)
    {
        std::ofstream ofs(filename.c_str(), std::ios::out | (createIfNotExist ? std::ios::trunc : std::ios::app));
        if (!ofs.is_open())
            return;
        ofs << text;
        ofs.close();
    }
    std::string readFileToString(const char *filename)
    {
        std::ifstream infile(filename);
        std::stringstream buffer;
        buffer << infile.rdbuf();
        return buffer.str();
    }
    void setCookies_onfile(std::map<std::string, std::string> &cook, std::string token)
    {
        std::string file = readFileToString("./Session_management/sessionIds");
        size_t poss;
        if ((poss = file.find(token)) != std::string::npos)
        {
            size_t pos = poss;
            std::string cookies_Add;
            for (std::map<std::string, std::string>::iterator it = cook.begin(); it != cook.end(); it++)
            {
                if (it->first != "session_id")
                {
                    if ((pos = file.find(" " + it->first + "=", pos)) != std::string::npos)
                    {
                        pos += 2 + it->first.length();
                        size_t value_pos = file.find(";", pos);
                        file = file.erase(pos, value_pos - pos);
                        for (size_t i = 0; it->second[i]; i++)
                            file.insert(pos + i, 1, it->second[i]);
                    }
                    else
                        cookies_Add += it->first + "=" + it->second + "; ";
                }
            }
            size_t len = token.length();
            if (!cookies_Add.empty())
                for (size_t i = 0; cookies_Add[i]; i++)
                    file.insert(file.find(token) + len + 2 + i, 1, cookies_Add[i]);
            writeToFile("./Session_management/sessionIds", file);
        }
    }
    std::string CokiesResponse(std::string token)
    {
        std::ifstream file("./Session_management/sessionIds");
        std::string line;
        if (file.is_open())
        {
            while (getline(file, line))
            {
                if (line.find(token) != std::string::npos)
                    return line.substr(0, line.length() - 2);
            }
            file.close();
        }
        return "session_id=" + token;
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
    std::string pathjoin(std::string root, std::string &path, std::string Location)
    {
        std::string r;
        if (root.back() != '/')
            root += '/';
        r = root + path.substr(Location.length());
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
    bool remove_directory(std::string path)
    {
        // Open the directory
        DIR *dir = opendir(path.c_str());
        if (!dir)
        {
            // Directory does not exist or cannot be opened
            return false;
        }

        // Loop over the contents of the directory
        struct dirent *entry;
        while ((entry = readdir(dir)))
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }
            std::string entry_path = std::string(path) + std::string(entry->d_name);
            if (is_directory(entry_path))
            {
                entry_path = entry_path + "/";
                if (!remove_directory(entry_path.c_str()))
                {
                    closedir(dir);
                    return false;
                }
            }
            else
            {
                // Remove files
                if (remove(entry_path.c_str()) != 0)
                {
                    closedir(dir);
                    return false;
                }
            }
        }

        // Close the directory and remove it
        closedir(dir);
        if (rmdir(path.c_str()) != 0)
        {
            return false;
        }

        return true;
    }
    std::string check_file(std::string path, int i = 1)
    {
        (void) i;
        if (fileExists(path + "index.html") && i == 0)
            return path + "index.html";
        else if (fileExists(path + "index.py"))
            return path + "index.py";
        else if (fileExists(path + "index.php"))
            return path + "index.php";
        return std::string();
    }
}
