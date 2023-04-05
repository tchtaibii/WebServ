#pragma once
#include "tools.hpp"
#include "Request.hpp"
namespace ws
{
    bool isZero(const std::string &httpRequest)
    {
        size_t pos = httpRequest.find("\r\n0\r\n\r\n");
        if (pos != std::string::npos)
            return 1;
        return 0;
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
        std::string last_one;
        int i = 0;
        while (1)
        {
            std::string start_seq = "\r\n";
            std::string end_seq = "\r\n";
            std::string::iterator start_pos = std::search(str.begin(), str.end(), start_seq.begin(), start_seq.end());
            if (start_pos == str.end())
                break;
            std::string::iterator end_pos = std::search(start_pos + start_seq.size(), str.end(), end_seq.begin(), end_seq.end());
            if (end_pos == str.end())
                break;
            size_t start = std::distance(str.begin(), start_pos) + start_seq.size();
            size_t end = std::distance(str.begin(), end_pos);
            if (end - start < 7)
            {
                std::string tmp = str.substr(start, end - start);
                if (tmp == "0")
                {
                    remove_zero_chunked(str);
                    if (i == 0)
                        return;
                    else
                        break;
                }
                if (isHexadecimal(tmp) && tmp.length() < 7)
                {
                    str = str.erase(start - 2, tmp.length() + 4);
                    continue;
                }
            }
            i++;
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
        std::string start_seq = "\r\n";
        std::string end_seq = "\r\n";
        std::string::iterator start_pos = std::search(str.begin(), str.end(), start_seq.begin(), start_seq.end());
        if (start_pos == str.end())
        {
            req.body += str;
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
        if (isHexadecimal(tmp) && tmp.length() < 7 && tmp != "0")
        {
            str = str.erase(start - 2, tmp.length() + 4);
            req.body += str.substr(0, start - 2);
            str = str.substr(start - 2);
            return chunked_uncoding(str, req);
        }
        req.body += str;
    }
}