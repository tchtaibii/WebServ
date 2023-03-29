#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <chrono>
#include <thread>

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
    int cgi_pid;
    int pid_status;
    char **env;
    char **args;
    int in_fd;
    //int out_fd;
    int tmp_fd;
    std::string out_path;
    std::string php;
    std::string py;
    std::string outname;
    std::string content_type;
    int ext;

public:
    cgi(std::string p);
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

cgi::cgi(std::string p)
{
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
    std::string s = "PATH_INFO=";
    s.append(path);
    env = new char *[2];
    env[0] = new char[11 + path.size()];
    strcpy(env[0], s.c_str());
    env[1] = NULL;
}

void cgi::exec_cgi(char **args, char **env, int fd)
{
    cgi_pid = fork();
    if (cgi_pid == -1)
    {
        throw(fork_error());
    }
    if (cgi_pid == 0)
    {
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

    wait_for_tempfile_file();
    
    infile.open("cgi/tempfile", std::ios::in);
    while (getline(infile, s))
    {
        str += s;
        if (infile.eof())
            break;
        str += '\n';
    }
    if (ext == 1)
    {
        int i = 0;
        int n = 0;
        parse_content_type(str);
        while (str[i])
        {
            if (str[i] == '\n')
                n++;
            if (n == 3)
                break;
            i++;
        }
        f = str.substr(i + 1);
    }
    else if (ext == 2)
        f = str;
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
    in_fd = open(path.c_str(), O_RDONLY);
    tmp_fd = open("cgi/tempfile", O_CREAT | O_WRONLY, 0666);
    fill_env();
    exec_cgi(args, env, in_fd);
    //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    remove_header();
    remove("cgi/tempfile");
}

#endif