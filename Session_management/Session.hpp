#include "../Request/tools.hpp"
namespace ws
{
    std::string generateSession()
    {
        std::string id_sesion = "";
        bool id_found = false;
        id_sesion = randomString(120); // generate unique session ID
        std::fstream file("./Session_management/sessionIds", std::ios::in);
        std::string line;
        while (std::getline(file, line))
        {
            size_t pos = line.find(id_sesion);
            if (pos != std::string::npos)
            {
                id_found = true;
                break;
            }
            else
                id_found = false;
        }
        file.close();
        if (!id_found)
        {
            std::fstream file("./Session_management/sessionIds", std::ios::out | std::ios::app);
            file << "session_id=" << id_sesion << "\n"; // write session ID to file
            file.close();
        }
        return id_sesion;
    }
}