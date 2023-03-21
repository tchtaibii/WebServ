#include "../Request/tools.hpp"
namespace ws
{
    std::string generateSession()
{
    std::string id_sesion = ""; 
    bool id_found = true;
    while (id_found) {
        id_sesion = std::to_string(time(NULL)) + "-" + std::to_string(rand()); // generate unique session ID
        std::fstream file("./Session_management/sessionIds", std::ios::in);
        std::string line;
        while (std::getline(file, line))
        {
            size_t pos = line.find(id_sesion);
            if (pos != std::string::npos) {
                id_found = true;
                break;
            }
            else {
                id_found = false;
            }
        }
        file.close(); // close file after reading from it
    }
    std::fstream file("./Session_management/sessionIds", std::ios::out | std::ios::app);
    file << "session_id=" << id_sesion << std::endl; // write session ID to file
    file.close(); // close file after writing to it
    return id_sesion;
}
}