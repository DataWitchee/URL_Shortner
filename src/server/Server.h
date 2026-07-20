#pragma once 

class Server{
    public:
    explicit Server(int port);
    ~Server(); // added deconstructor
    void Start();
    
    private:
    void CreateSocket();
    void Bind();
    void Listen();
    void Accept();
    int port_;
    int serverSocket_;
};