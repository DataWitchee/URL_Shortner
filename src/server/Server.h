#pragma once
#include "service/UrlService.h"

class Server
{
public:
    explicit Server(int port, UrlService &urlService);
    ~Server(); // added deconstructor
    void Start();

private:
    void CreateSocket();
    void Bind();
    void Listen();
    void Accept();

    // method to handle web browsers request
    void HandleClient(int clientSocket);
    std::string ExtractPath(const std::string &request);
    int port_;
    int serverSocket_;
    UrlService &urlService_;
};