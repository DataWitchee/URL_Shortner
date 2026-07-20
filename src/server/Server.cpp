#include "server/Server.h"
#include<iostream>

#include <stdexcept>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>

Server::Server(int port) : port_(port), serverSocket_(-1){}

Server::~Server(){
    if(serverSocket_ != -1){
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
    sockaddr_in serverAddress{};           // handles IPv4 addresses
    serverAddress.sin_family = AF_INET;    // this set address family to AF_INET(IPV4)
    serverAddress.sin_port = htons(port_); // this will set the port number host will listen to
    // htons host to network  // AF means address family
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    // htonl -> host to network long
    // INADDR_ANY will tell the server to listen to any network address

    // Binding the socket
    if (::bind(serverSocket_,
               reinterpret_cast<sockaddr *>(&serverAddress),
               sizeof(serverAddress)) == -1)
    {
        throw std::runtime_error("Failed to bind socket.");
        // :: is used to call global C function
    }
}

void Server::Listen()
{
    constexpr int backlog = 128;
    if (::listen(serverSocket_, backlog) == -1)
    {
        throw std::runtime_error("Failed to listen on socket");
    }
    // process enters in kernel with listen system call
    //  it changes kernels state from closed to listen
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
            throw std::runtime_error("Failed to acccept client..");
            continue;
        }
        std::cout<<"Client Connected..";
        ::close(clientSocket);
    }
}
