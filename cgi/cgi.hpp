#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <chrono>
#include <thread>
#include<string>
#include "../Response/Response.hpp"

class cgi
{
public:
    enum
    {
        NONE = 0,
        DONE = 1,
        ERROR = -1
    };

private:
    std::string path;
    int body_existense;
    int cgi_pid;
    int pid_status;
    char **env;
    char **args;
    int in_fd;
    int tmp_fd;
    std::string out_path;
    std::string php;
    std::string py;
    std::string outname;
    std::string content_type;
    int ext;
    ws::HttpRequest req;
    std::string port;
    std::string query;

public:
    cgi(std::string p, ws::HttpRequest request);
    ~cgi();
    int get_cgi_pid();
    void fill_env();
    void exec_cgi(char **args, char **env, int fd);
    void wait_cgi();
    void exec();
    int check_extension(std::string str);
    void fill_args();
    std::string random_name();
    std::string get_outfile_path();
    void remove_header();
    void wait_for_tempfile_file();
    void parse_content_type(std::string str);
    std::string get_content_type();
    int get_extension();
    void wait_for_body_file();
    class fork_error : public std::exception
    {
        const char *what() const throw()
        {
            return ("fork failed");
        }
    };
    class cgi_open_error : public std::exception
    {
        const char *what() const throw()
        {
            return ("CGI open failed");
        }
    };
    class extension_error : public std::exception
    {
        const char *what() const throw()
        {
            return ("Unknown extension");
        }
    };
};

int check_extension2(std::string name)
{
    if (name.substr(name.find_last_of(".") + 1) == "php" || name.substr(name.find_last_of(".") + 1) == "py")
        return (1);
    else
        return (0);
}

cgi::cgi(std::string p, ws::HttpRequest request)
{
    query = request.query;
    port = request.port;
    req = request;
    path = p;
    cgi_pid = -1;
    pid_status = 0;
    php = "cgi/cgi-bin/php-cgi";
    py = "/usr/local/bin/python3";
}

cgi::~cgi()
{
}

int cgi::get_cgi_pid()
{
    return (this->cgi_pid);
}

std::string cgi::random_name()
{
    char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    int len = 5;
    srand(time(0));
    std::string name;
    name.reserve(len);
    for (int i = 0; i < len; i++)
        name += letters[rand() % (sizeof(letters) - 1)];
    return (name);
}

std::string cgi::get_outfile_path()
{
    char buff[PATH_MAX];
    getcwd(buff, PATH_MAX);
    std::string outfile_path(buff);
    outfile_path.append("/");
    outfile_path.append(outname);
    return (outfile_path);
}

void cgi::fill_env()
{
    if (req.method == "POST")
        env = new char *[13];
    else
        env = new char *[11];
    std::string s = "PATH_INFO=";
    s.append(path);
    if (!query.empty() && query.find('&') != std::string::npos && req.method == "POST")
    {
        query = query.substr(0, query.size() - 1);
        s += '?';
        s.append(query);
    }
    if (query.empty() && req.method == "POST" && req.headers["Content-Type"] == "application/x-www-form-urlencoded\r")
        query = req.body;
    env[0] = new char[s.size() + 1];
    strcpy(env[0], s.c_str());
    s.clear();
    s = "QUERY_STRING=";
    s.append(query);
    env[1] = new char[s.size() + 1];
    strcpy(env[1], s.c_str());

    s.clear();
    s = "SERVER_PORT=";
    s.append(port);
    env[2] = new char[s.size() + 1];
    strcpy(env[2], s.c_str());

    s.clear();
    s = "REQUEST_METHOD=";
    s.append(req.method);
    env[3] = new char[s.size() + 1];
    strcpy(env[3], s.c_str());

    s.clear();
    s = "SCRIPT_FILENAME=";
    s.append(path);
    env[4] = new char[s.size() + 1];
    strcpy(env[4], s.c_str());
    
    s.clear();
    s = "SCRIPT_NAME=";
    s.append(php);
    env[5] = new char[s.size() + 1];
    strcpy(env[5], s.c_str());

    s.clear();
    s = "REDIRECT_STATUS=200";
    env[6] = new char[s.size() + 1];
    strcpy(env[6], s.c_str());

    s.clear();
    s = "REMOTE_ADDR=";
    s.append("127.0.0.1");
    env[7] = new char[s.size() + 1];
    strcpy(env[7], s.c_str());

    s.clear();
    s = "HTTP_COOKIE=";
    s.append(req.headers["Cookie"]);
    env[8] = new char[s.size() + 1];
    strcpy(env[8], s.c_str());

    s.clear();
    s = "HTTP_USER_AGENT=";
    s.append(req.headers["User-Agent"]);
    env[9] = new char[s.size() + 1];
    strcpy(env[9], s.c_str());

    s.clear();
    s = "HTTP_HOST=";
    s.append("localhost");
    env[10] = new char[s.size() + 1];
    strcpy(env[10], s.c_str());

    if (req.method == "POST")
    {
        s.clear();
        s = "CONTENT_TYPE=";
        s.append(req.headers["Content-Type"].substr(0, req.headers["Content-Type"].length() - 1));
        env[11] = new char[s.size() + 1];
        strcpy(env[11], s.c_str());
        s.clear();
        s = "CONTENT_LENGTH=";
        s.append(req.headers["Content-Length"]);
        env[12] = new char[s.size() + 1];
        strcpy(env[12], s.c_str());

        env[13] = NULL;
    }
    else
        env[11] = NULL;
}

void cgi::exec_cgi(char **args, char **env, int fd)
{
    lseek(tmp_fd, 0, SEEK_SET);
    cgi_pid = fork();
    if (cgi_pid == -1)
    {
        throw(fork_error());
    }
    if (cgi_pid == 0)
    {
        if (req.method == "POST")
            dup2(fd, 0);
        dup2(tmp_fd, 1);
        if (execve(args[0], args, env) == -1)
            exit(1);
    }
}

void cgi::wait_cgi()
{
    int s;
    int pid = waitpid(cgi_pid, &s, WNOHANG);
    if (pid == -1)
        pid_status = ERROR;
    else if (pid != 0)
    {
        if (WIFSIGNALED(pid_status))
        {
            pid_status = ERROR;
        }
        else
        {
            pid_status = DONE;
        }
    }
    if (pid_status == DONE || pid_status == ERROR)
    {
        close(tmp_fd);
        close(in_fd);
    }
}

int cgi::check_extension(std::string str)
{
    if (str.substr(str.find_last_of(".") + 1) == "php")
        return (1);
    else if (str.substr(str.find_last_of(".") + 1) == "py")
        return (2);
    else
        return (0);
}

void cgi::fill_args()
{
    ext = check_extension(path);
    args = new char *[3];
    if (ext == 1)
    {
        args[0] = new char[php.size() + 1];
        strcpy(args[0], php.c_str());
    }
    else if (ext == 2)
    {
        args[0] = new char[py.size() + 1];
        strcpy(args[0], py.c_str());
    }
    else
    {
        throw(extension_error());
    }
    args[1] = new char[path.size() + 1];
    strcpy(args[1], path.c_str());
    args[2] = NULL;
}

void cgi::wait_for_tempfile_file()
{
    while (true)
    {
        std::string t;
        std::fstream tempfile;
        tempfile.open("cgi/tempfile");
        if(getline(tempfile, t))
        {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        tempfile.close();
    }
}

void cgi::wait_for_body_file()
{
    while (true)
    {
    
        std::fstream tempfile;
        tempfile.open("cgi/tempbody");
        if(tempfile.good())
        {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        tempfile.close();
    }
}

void cgi::parse_content_type(std::string str)
{
    int i;

    i = str.find("Content-type");
    while (str[i + 14] != ';')
    {
        content_type += str[i + 14];
        i++;
    }
}

std::string cgi::get_content_type()
{
    return(content_type);
}

int cgi::get_extension()
{
    return(ext);
}

void cgi::remove_header()
{
    std::string s;
    std::string str;
    std::string f;
    std::fstream infile;
    std::ofstream outfile;
    std::string c_type;

    wait_for_tempfile_file();
    infile.open("cgi/tempfile", std::ios::in);
    if (ext == 1)
    {
        while (getline(infile, str))
        {
            if (str != "\r")
            {
                c_type += str;
                c_type += "\r\n";
            }
            else
            {
                str.clear();
                break;
            }
            str.clear();
        }
        while (getline(infile, str))
        {
            f += str;
            if (infile.eof())
                break;
            f += '\n';
        }
        content_type = c_type.substr(0, c_type.size() - 1);
    }
    else if (ext == 2)
    {
        while (getline(infile, str))
        {
            f += str;
            if (infile.eof())
                break;
            f += '\n';
        }
        content_type = "Content-Type: text/html";
    }
    outfile.open(outname, std::ios::out);
    outfile << f;
    infile.close();
}

void cgi::exec()
{
    fill_args();
    try
    {
        if (access(args[0], F_OK | X_OK) == -1)
            throw(cgi_open_error());
    }
    catch(...){}
    outname = "cgi/" + random_name();
    if (!req.body.empty())
    {
        req.body = req.body.substr(2);
        body_existense = 1;
        in_fd = open("cgi/tempbody", O_CREAT | O_RDWR | O_TRUNC, 0666);
        wait_for_body_file();
        write(in_fd, req.body.c_str(), req.body.size());
        lseek(in_fd, 0, SEEK_SET);
    }
    else
        body_existense = 0;
    tmp_fd = open("cgi/tempfile", O_CREAT | O_WRONLY | O_TRUNC, 0666);
    fill_env();
    exec_cgi(args, env, in_fd);
    wait_cgi();
    remove_header();
    remove("cgi/tempfile");
    if (body_existense == 1)
        remove("cgi/tempbody");
    
    int i = 0;
	while (env[i]){
		delete env[i];
		i++;
	}
	i = 0;
	while (args[i]){
		delete args[i];
		i++;
	}
	delete[] args;
	delete[] env;
}

#endif