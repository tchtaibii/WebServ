#include "../Request/tools.hpp"
namespace ws
{
    std::string generateSession()
    {
        std::string id_sesion = randomString(128);
        std::cout << id_sesion << std::endl;
        std::fstream file("./Session_management/sessionIds", std::ios::in | std::ios::out | std::ios::app);
        std::string line;
        while (std::getline(file, line))
        {
            std::cout << line << std::endl;
        }
        size_t pos = line.find(id_sesion);
        if (pos == std::string::npos)
            file << "sessio_id=" << id_sesion << std::endl;
        return id_sesion;
    }
}