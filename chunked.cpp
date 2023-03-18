
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
bool isHexadecimal(std::string str)
{
    for (size_t i = 0; i < str.length(); i++)
    {
        if (isxdigit(str[i]))
            continue;
        else
            return false;
    }
    return true;
}
void chunked_uncoding(std::string &str, std::string &reqbody)
{
    std::string start_seq = "*/";
    std::string end_seq = "*/";
    std::string::iterator start_pos = std::search(str.begin(), str.end(), start_seq.begin(), start_seq.end());
    if (start_pos == str.end())
    {
        std::cout << "start_pos" << std::endl;
        return;
    }
    std::string::iterator end_pos = std::search(start_pos + start_seq.size(), str.end(), end_seq.begin(), end_seq.end());
    if (end_pos == str.end())
    {
        std::cout << "end_pos" << std::endl;
        return;
    }
    size_t start = std::distance(str.begin(), start_pos) + start_seq.size();
    size_t end = std::distance(str.begin(), end_pos);
    std::cout << "Subsequence found from position " << start << " to " << end << std::endl;
    std::string tmp = str.substr(start, end - start);
    std::cout << "|" << tmp << "|" << std::endl;
    if (tmp.length() > 6)
    {
        std::cout << tmp.length() << " : lenhght" << std::endl;
        reqbody += str.substr(0, end + 2);
        str = str.substr(end + 2);
        return chunked_uncoding(str, reqbody);
    }
    bool boolean = isHexadecimal(tmp);
    if (boolean)
    {
        std::cout << "delete '" << str.substr(start - 2, tmp.length() + 4) << "'";
        str = str.erase(start - 2, tmp.length() + 4);
        std::cout << "boolean" << std::endl;
        return chunked_uncoding(str, reqbody);
    }
    reqbody += str.substr(end + 2);
}
int main()
{

    std::string str = "";
    std::string taha;
    std::ifstream file("tex");
    if (!file.is_open())
        return 1;
    std::ostringstream ss;
    ss << file.rdbuf();
    str = ss.str();
    file.close();
    chunked_uncoding(str, taha);
    std::ofstream export_("skkkkkkr.cpp");
    if (export_.is_open())
    {
        export_ << taha;
        export_.close();
    }
    return 0;
}