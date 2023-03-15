// #pragma once
// #include <iostream>
// #include "../Socket/socket.hpp"
// #include "../requestParsing/checkRequest.hpp"
// #include <ctime>
// #include <string>

// namespace ws
// {


//     std::string getCurrentTime()
//     {
//         time_t now = time(0);
//         tm *gmt_time = gmtime(&now);
//         char buffer[30];
//         strftime(buffer, 30, "%a, %d %b %Y %H:%M:%S GMT", gmt_time);
//         return std::string(buffer);
//     }
//     std::string statusStr(int cs)
//     {
//     }
//     std::string responseStr(server server, HttpRequest &req, int codeStatus)
//     {
//         std::string versionHttp = req.version;
//         std::stringstream ss;
//         ss << codeStatus;
//         std::string CS = ss.str();
//         std::string Status = statusStr(codeStatus);
//         std::string Date = getCurrentTime();

//         std::string httpResponse = versionHttp + " " +  CS + " " +  Status "\r\n"
//                                        "Date: " + Date + "\r\n"
//                                        "Server: Apache/2.2.14 (Win32)\r\n"
//                                        "Content-Length: 230\r\n"
//                                        "Connection: Closed\r\n"
//                                        "Content-Type: text/html; charset=iso-8859-1\r\n\r\n"
//                                        "<!DOCTYPE HTML PUBLIC " - // IETF//DTD HTML 2.0//EN">\r\n"
//                                        "<html>\r\n"
//                                        "<head>\r\n"
//                                        "<title>404 Not Found</title>\r\n"
//                                        "</head>\r\n"
//                                        "<body>\r\n"
//                                        "<h1>404 Not Found</h1>\r\n"
//                                        "<p>The requested URL /t.html was not found on this server.</p>\r\n"
//                                        "</body>\r\n"
//                                        "</html>\r\n";
//     }
// }