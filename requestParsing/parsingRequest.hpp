#pragma once
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <stdlib.h>
#include <iomanip>
#include <sys/time.h>

namespace ws
{
    // struct of http request
    struct HttpRequest
    {
        std::string method;
        std::string path;
        std::string version;
        std::map<std::string, std::string> headers;
        std::string body;
        // int read;
        bool chunked;
        bool deja;
        bool con;
        bool end_;
        bool Boundary;
        std::string Boundary_token;
        bool headers_complet;
        bool length_chunk;
        int chunked_need;
        std::string chunked_tmp;
        HttpRequest() : chunked(false), deja(false), con(false), end_(false), Boundary(false), headers_complet(false), length_chunk(false), chunked_need(0), chunked_tmp("") {}
    };
    std::string randomString(int length)
    {
        srand(time(NULL));                                                                               // seed the random number generator with the current time
        std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.$@*"; // the characters to choose from
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
        // Get the current time
        time_t now = time(0);
        struct tm tstruct;
        char buf[80];

        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tstruct);

        // Get the current milliseconds
        struct timeval tv;
        gettimeofday(&tv, NULL);

        // Convert the time to a string
        std::ostringstream oss;
        oss << buf << "." << std::setfill('0') << std::setw(3) << tv.tv_usec / 1000;
        return oss.str();
    }
    bool isZero(const std::string &httpRequest)
    {
        size_t pos = httpRequest.find("\r\n0\r\n\r\n");
        if (pos != std::string::npos)
            return 1;
        return 0;
    }
    size_t countSubstring(const std::string &str, const std::string &sub)
    {
        size_t count = 0;
        size_t pos = str.find(sub);
        while (pos != std::string::npos)
        {
            count++;
            pos = str.find(sub, pos + sub.length());
        }
        return count;
    }
    std::map<std::string, std::string> boundaryParsing(std::string body, HttpRequest &req)
    {
        std::map<std::string, std::string> boundary_files;
        size_t count = countSubstring(body, "----------------------------" + req.Boundary_token);
        for (size_t i = 0; i < count - 1; i++)
        {
            size_t pos = body.find("filename=");
            std::string fileName;
            if (pos != std::string::npos)
                fileName = body.substr(pos + 10);
            else
                fileName = req.Boundary_token + randomString(1);
            fileName = fileName.substr(0, fileName.find("\"\r\n"));
            body = body.substr(body.find("\r\n\r\n") + 2);
            std::string tmp = body.substr(2, body.find("----------------------------" + req.Boundary_token) - 4);
            body = body.substr(body.find(tmp) + tmp.length());
            boundary_files.insert(std::make_pair(fileName, tmp));
        }
        return boundary_files;
    }
    void httpRequestInit(HttpRequest &req, bool all)
    {
        if (all)
        {
            req.method.clear();
            req.path.clear();
            req.version.clear();
            req.headers.clear();
        }
        req.body.clear();
        req.Boundary_token.clear();
        req.chunked = 0;
        req.deja = 0;
        req.con = 0;
        req.end_ = 0;
        req.Boundary = 0;
        req.headers_complet = false;
        req.length_chunk = false;
        req.chunked_tmp.clear();
        req.chunked_need = 0;
        req.valread = 0;
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
            std::cout << "the char pass it on hexa fun " << str[i] << std::endl;
            if (is_hxa_low(str[i]))
                continue;
            else
                return false;
        }
        return true;
    }
    void remove_zero_chunked(std::string &chunked_message)
    {
        size_t pos = chunked_message.find("\r\n0\r\n");
        if (pos == std::string::npos)
            return;
        chunked_message = chunked_message.erase(pos, 5);
        return;
    }
    void verifyChunk(std::string &str)
    {
        std::string  last_one;
        while (1)
        {
            std::string start_seq = "\r\n";
            std::string end_seq = "\r\n";
            std::string::iterator start_pos = std::search(str.begin(), str.end(), start_seq.begin(), start_seq.end());
            if (start_pos == str.end())
                break ;
            std::string::iterator end_pos = std::search(start_pos + start_seq.size(), str.end(), end_seq.begin(), end_seq.end());
            if (end_pos == str.end())
                break ;
            size_t start = std::distance(str.begin(), start_pos) + start_seq.size();
            size_t end = std::distance(str.begin(), end_pos);
            if (end - start < 7)
            {
                std::string tmp = str.substr(start, end - start);
                if (tmp == "0")
                {
                    std::cout << "hhya f zeeeero " << std::endl;
                    remove_zero_chunked(str);
                    break ;
                }
                if (isHexadecimal(tmp) && tmp.length() < 7)
                {
                    std::cout << "|" << tmp << "|" << std::endl;
                    str = str.erase(start - 2, tmp.length() + 4);
                    continue ;
                }
            }
            last_one += str.substr(0, end);
            str = str.substr(end);
        }
        str = last_one;
        last_one.clear();
    }

    bool check_chunk(std::string str)
    {
        if (str[str.length() - 1] == '\r' || str[str.length() - 1] == '\n' || is_hxa_low(str[str.length() - 1]))
            return 1;
        return 0;
    }
    void chunked_uncoding(std::string &str, HttpRequest &req)
    {
        // if (check_chunk(str))
        // {
        //     std::cout << "mada mada" << std::endl;
        //     req.chunked_tmp += str;
        //     str.clear();
        //     req.chunked_need = 1;
        //     return;
        // }
        // if (req.chunked_need)
        // {
        //     std::cout << "sarf" << std::endl;
        //     str = req.chunked_tmp + str;
        //     req.chunked_tmp.clear();
        //     req.chunked_need = 0;
        // }
        std::string start_seq = "\r\n";
        std::string end_seq = "\r\n";
        std::string::iterator start_pos = std::search(str.begin(), str.end(), start_seq.begin(), start_seq.end());
        if (start_pos == str.end())
        {
            req.body += str;
            // std::cout << "start_pos" << std::endl;
            return;
        }
        std::string::iterator end_pos = std::search(start_pos + start_seq.size(), str.end(), end_seq.begin(), end_seq.end());
        if (end_pos == str.end())
        {
            req.body += str;
            return;
        }
        size_t start = std::distance(str.begin(), start_pos) + start_seq.size();
        size_t end = std::distance(str.begin(), end_pos);
        std::string tmp = str.substr(start, end - start);
        bool boolean = isHexadecimal(tmp);
        if (tmp == "0")
            std::cout << "tamal 7abk" << std::endl;
        if (boolean && tmp.length() < 7 && tmp != "0")
        {
            std::cout << "|" << tmp << "|" << std::endl;
            str = str.erase(start - 2, tmp.length() + 4);
            req.body += str.substr(0, start - 2);
            str = str.substr(start - 2);
            std::cout << "boolean" << std::endl;
            return chunked_uncoding(str, req);
        }
        req.body += str;
    }
    bool bodyParsing(HttpRequest &req, std::string &body, bool the_end)
    {
        if (req.method == "POST")
        {
            std::string a = req.headers["Content-Length"];
            std::string b = req.headers["Transfer-Encoding"];
            if (!a.empty())
            {
                size_t lenght_body = atoi(a.c_str());
                if ((lenght_body + 2 == body.length()) || (req.Boundary && body.find("----------------------------" + req.Boundary_token + "--") != std::string::npos))
                {
                    req.body = body.substr(2);
                    if (req.Boundary)
                    {
                        req.deja = false;
                        std::map<std::string, std::string> boundary_files = boundaryParsing(req.body, req);
                        for (std::map<std::string, std::string>::iterator it = boundary_files.begin(); it != boundary_files.end(); it++)
                        {
                            std::string tmp = it->first + "tmp";
                            std::ofstream file(tmp);
                            if (file.is_open())
                            {
                                file << it->second;
                                file.close();
                            }
                            httpRequestInit(req, 0);
                        }
                        return true;
                    }
                    else
                    {
                        req.deja = false;
                        std::string extension = req.headers["Content-Type"].substr(req.headers["Content-Type"].find("/") + 1, req.headers["Content-Type"].find("\r"));
                        std::ofstream file(getCurrentDateTime() + "." + extension);
                        if (file.is_open())
                        {
                            file << req.body;
                            file.close();
                        }
                        httpRequestInit(req, 0);
                        return true;
                    }
                }
                else
                    return false;
            }
            else if (req.chunked) // chunked
            {
                if (the_end)
                {
                    // std::cout << "khasha dkhuul" << std::endl;
                    // std::string tmp_b = req.body;
                    // req.body.clear();
                    // chunked_uncoding(tmp_b, req);
                    verifyChunk(req.body);
                    // remove_zero_chunked(req.body);
                    // req.body = req.body.substr(0, body.length());
                    std::string extension = req.headers["Content-Type"].substr(req.headers["Content-Type"].find("/") + 1, req.headers["Content-Type"].find("\r"));
                    std::ofstream file(getCurrentDateTime() + "." + extension);
                    if (file.is_open())
                    {
                        file << req.body;
                        file.close();
                    }
                    httpRequestInit(req, 0);
                    return true;
                }
                return false;
            }
        }
        return false;
    }
    void remove_chunk_coding(std::string &chunked_message, HttpRequest &req)
    {
        if (!req.chunked_tmp.empty())
            chunked_message = req.chunked_tmp.substr(0, req.chunked_tmp.length()) + chunked_message.substr(0, chunked_message.length());
        req.end_ = isZero(chunked_message);
        if (req.chunked_need != 0)
        {
            req.body += chunked_message.substr(0, req.chunked_need);
            chunked_message = chunked_message.substr(req.chunked_need);
            req.chunked_need = 0;
            if (req.end_)
            {
                remove_zero_chunked(chunked_message);
                req.body += chunked_message;
                req.chunked_tmp = "";
                req.con = bodyParsing(req, req.body, 1);
                return;
            }
        }
        size_t pos1 = chunked_message.find("\r\n");
        if (pos1 == std::string::npos)
            return;
        std::string tmp = chunked_message.substr(pos1 + 2);
        size_t pos = tmp.find("\r\n");
        if (pos == std::string::npos)
            return;
        tmp = tmp.substr(0, pos);
        if (isHexadecimal(tmp))
        {
            size_t hex_ = strtol(tmp.c_str(), NULL, 16);
            std::string ok = "\r\n" + tmp + "\r\n";
            req.chunked_tmp = chunked_message.substr(pos1 + ok.length(), hex_);
            size_t len = req.chunked_tmp.length();
            req.length_chunk = (len != (size_t)strtol(tmp.c_str(), NULL, 16)) ? false : true;
            req.chunked_need = (!req.length_chunk) ? hex_ - len : 0;
            req.body += req.chunked_tmp;
            req.chunked_tmp = chunked_message.substr(chunked_message.find(req.chunked_tmp) + len);
            chunked_message.clear();
        }
        if (req.end_)
            req.con = bodyParsing(req, req.body, 1);
        return;
    }
    HttpRequest parse_http_request(std::string tmp, HttpRequest &req, std::string &request_im)
    {
        if (tmp.find("\r\n\r\n") == std::string::npos && !req.headers_complet)
        {
            request_im += tmp;
        }
        else if (!req.deja)
        {
            request_im += tmp.substr(0, tmp.length());
            tmp = request_im.substr(0, request_im.length());
            req.headers_complet = true;
            size_t pos = tmp.find("\r\n\r\n");
            std::string header_tmp = tmp.substr(0, pos);
            std::stringstream ss(header_tmp);
            std::string line;
            getline(ss, line); // First line is the request line
            std::stringstream req_line_ss(line);
            req_line_ss >> req.method >> req.path >> req.version;
            // Parse headers
            while (getline(ss, line))
            {
                if (line.empty())
                    break; // End of headers
                size_t tmp_pos = line.find(":");
                if (tmp_pos == std::string::npos)
                    continue; // Malformed header
                std::string header_name = line.substr(0, tmp_pos);
                std::string header_value = line.substr(tmp_pos + 2);
                req.headers.insert(make_pair(header_name, header_value));
            }
            if (req.method == "POST")
            {
                req.body = tmp.substr(pos + 2);
                try
                {
                    if (req.headers["Content-Type"].find("boundary=--------------------------"))
                    {
                        req.Boundary_token = req.headers["Content-Type"].substr(req.headers["Content-Type"].find("boundary=") + 9 + 26, 24);
                        // std::cout << "|" << req.Boundary_token << "|" << std::endl;
                        req.Boundary = true;
                    }
                }
                catch (...)
                {
                    std::cerr << "" << '\n';
                }
                std::string b = req.headers["Transfer-Encoding"];
                if (!b.empty() && "chunked\r" == b)
                {
                    req.chunked = true;
                    std::string tmp_tmp = req.body;
                    req.body = "";
                    remove_chunk_coding(tmp_tmp, req);
                }
            }
            req.deja = true;
        }
        return req;
    }
}
