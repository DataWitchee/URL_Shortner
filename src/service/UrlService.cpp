#include "service/UrlService.h"
#include "utils/Random.h"

UrlService :: UrlService(Database& db) : db_(db) {}

std::string UrlService::ShortenUrl(const std::string& originalUrl){
    std::string shortCode;

    do{
        shortCode = utils::GenerateShortCode();
    }
    while(db_.Exists(shortCode));

    db_.SaveUrl(shortCode, originalUrl);
    return shortCode;
}

std::optional<std::string> UrlService::GetOriginalUrl(const std::string& shortCode) const {
    // Ask the database for the URL
    return db_.GetUrl(shortCode);
}