#include "server/Server.h"
#include "database/Database.h"
#include "utils/Random.h"
#include "service/UrlService.h"
#include <iostream>

int main() {
    try {
        std::cout << "--- Starting URL Shortener ---\n";

        // 1. Initialize Database (This creates url_shortener.db and the urls table)
        std::cout << "Initializing Database (SQLite)...\n";
        Database db;

        // 2. Initialize the Service with the database reference
        UrlService service(db);

        // --- QUICK SANITY CHECK ---
        // Let's test the Utils and Database locally before starting the web server
        std::string originalUrl = "https://www.google.com/search?q=c++";
        std::string shortCode = service.ShortenUrl(originalUrl);
        
        std::cout << "Test: Shortened '" << originalUrl << "' to code: " << shortCode << "\n";

        auto retrievedUrl = service.GetOriginalUrl(shortCode);
        if (retrievedUrl.has_value()) {
            std::cout << "Test: Successfully retrieved from DB: " << retrievedUrl.value() << "\n\n";
        } else {
            std::cerr << "Test: Failed to retrieve URL from DB!\n\n";
        }
        // --------------------------

        // 3. Start the Web Server
        int port = 8080;
        Server server(port, service);
        // Note: Start() has an infinite loop inside Accept(), so the program will stay running here.
        server.Start(); 

    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}