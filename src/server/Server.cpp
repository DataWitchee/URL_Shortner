#include "server/Server.h"
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <fstream>

Server::Server(int port, UrlService &urlService) : port_(port), serverSocket_(-1), urlService_(urlService) {}

Server::~Server()
{
    if (serverSocket_ != -1)
    {
        ::close(serverSocket_);
    }
}

void Server::CreateSocket()
{
    serverSocket_ = ::socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket_ == -1)
    {
        throw std::runtime_error("Failed to create socket.");
    }
}

void Server::Start()
{
    if (serverSocket_ != -1)
    {
        throw std::logic_error("Server has already been started");
    }

    CreateSocket();
    Bind();
    Listen();
    Accept();
}

void Server::Bind()
{
    sockaddr_in serverAddress{};           
    serverAddress.sin_family = AF_INET;    
    serverAddress.sin_port = htons(port_); 
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    if (::bind(serverSocket_,
               reinterpret_cast<sockaddr *>(&serverAddress),
               sizeof(serverAddress)) == -1)
    {
        throw std::runtime_error("Failed to bind socket.");
    }
}

void Server::Listen()
{
    constexpr int backlog = 128;
    if (::listen(serverSocket_, backlog) == -1)
    {
        throw std::runtime_error("Failed to listen on socket");
    }
}

void Server::Accept()
{
    while (true)
    {
        sockaddr_in clientAddress{};
        socklen_t clientLenght = sizeof(clientAddress);
        int clientSocket = ::accept(
            serverSocket_,
            reinterpret_cast<sockaddr *>(&clientAddress),
            &clientLenght);

        if (clientSocket == -1)
        {
            std::cerr << "Failed to accept client connection.\n";
            continue;
        }
        std::cout << "Client Connected...\n";
        HandleClient(clientSocket);
        ::close(clientSocket);
    }
}

void Server::HandleClient(int clientSocket)
{
    char buffer[4096] = {0};

    // 1. Read the HTTP request from the browser
    ssize_t bytesRead = ::recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0)
        return;

    std::string request(buffer);

    // 2. Parse the Method (GET/POST) and the Path
    std::istringstream stream(request);
    std::string method, path, version;
    stream >> method >> path >> version;

    if (path == "/favicon.ico")
        return;

    std::cout << "Method: " << method << " | Path: " << path << "\n";
    std::string httpResponse;

    // --- ROUTING LOGIC ---

    // ROUTE 1: The Frontend Website (GET /)
    if (method == "GET" && (path == "/" || path.empty()))
    {
        std::ifstream file("../frontend/index.html");

        if (file.is_open())
        {
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string html = buffer.str();

            httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " 
                         + std::to_string(html.length()) + "\r\n\r\n" + html;
        }
        else
        {
            std::string errorMsg = "Frontend file not found!";
            httpResponse = "HTTP/1.1 404 Not Found\r\nContent-Length: " 
                         + std::to_string(errorMsg.length()) + "\r\n\r\n" + errorMsg;
        }
    }

    // ROUTE 2: API to generate the code (POST /api/shorten)
    else if (method == "POST" && path == "/api/shorten")
    {
        std::string::size_type bodyPos = request.find("\r\n\r\n");
        if (bodyPos != std::string::npos)
        {
            std::string longUrl = request.substr(bodyPos + 4);

            // FIX 1: Handle TCP Packet Fragmentation!
            // If Safari sent the headers and body separately, longUrl is empty right now.
            // We call recv() one more time to catch the second packet containing the URL.
            if (longUrl.empty())
            {
                char bodyBuffer[4096] = {0};
                ssize_t bodyBytes = ::recv(clientSocket, bodyBuffer, sizeof(bodyBuffer) - 1, 0);
                if (bodyBytes > 0)
                {
                    longUrl = std::string(bodyBuffer);
                }
            }

            // Only save it to SQLite if we actually successfully grabbed a URL
            if (!longUrl.empty()) 
            {
                std::string shortCode = urlService_.ShortenUrl(longUrl);
                httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " 
                             + std::to_string(shortCode.length()) + "\r\n\r\n" + shortCode;
            }
        }
    }

    // ROUTE 3: Redirecting an existing code (GET /<shortCode>)
    else if (method == "GET")
    {
        std::string shortCode = path.substr(1);
        auto originalUrl = urlService_.GetOriginalUrl(shortCode);

        // FIX 2: Ensure the string actually exists in the database and is not empty
        if (originalUrl.has_value() && !originalUrl.value().empty())
        {
            std::string targetUrl = originalUrl.value();
            
            // FIX 3: The Foolproof Check
            // If the browser sent a link without https://, we force it here!
            if (targetUrl.find("http://") != 0 && targetUrl.find("https://") != 0) {
                targetUrl = "https://" + targetUrl;
            }

            std::cout << "SUCCESS! Redirecting to -> " << targetUrl << "\n";

            httpResponse = "HTTP/1.1 302 Found\r\n"
                           "Location: " + targetUrl + "\r\n"
                           "Connection: close\r\n"
                           "Content-Length: 0\r\n\r\n";
        }
        else
        {
            std::string html = "<h1>404 Not Found</h1><p>This URL doesn't exist.</p>";
            httpResponse = "HTTP/1.1 404 Not Found\r\n"
                           "Content-Type: text/html\r\n"
                           "Connection: close\r\n"
                           "Content-Length: " + std::to_string(html.length()) + "\r\n\r\n" + html;
        }
    }
    ::send(clientSocket, httpResponse.c_str(), httpResponse.length(), 0);
   
}

// Helper function to extract the path from the HTTP request
std::string Server::ExtractPath(const std::string &request)
{
    std::istringstream stream(request);
    std::string method, path, version;
    stream >> method >> path >> version;
    return path;
}