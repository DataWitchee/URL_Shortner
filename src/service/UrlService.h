#pragma once
#include <iostream>
#include <string>
#include <optional>
#include "database/Database.h"

class UrlService
{
public:
    explicit UrlService(Database &db_); // internal conversion ko avoid krne ke liye

    // take a long url , convert it into short code , save it and return krdoo
    std::string ShortenUrl(const std::string &originalUrl);

    // generate url
    std::optional<std::string> GetOriginalUrl(const std::string &shortCode) const;

private:
    Database &db_;
};