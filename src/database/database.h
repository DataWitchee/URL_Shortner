#pragma once
#include <iostream>
#include <string>
#include <optional>
#include <sqlite3.h> // provides sqlite functions

class Database
{
public:
    Database();
    ~Database(); // destructor to close the database connection

    // saves mapping of shortcode to original URL in the database
    void SaveUrl(const std::string &shortcode, const std::string &originalURL);

    // Retrieves the original URL. Returns std::nullopt if not found.
    std::optional<std::string> GetUrl(const std::string &shortCode) const;

    // checks if a short code already exists in the database(to prevent overwriting)
    bool Exists(const std::string &shortcode) const;

private:
    sqlite3 *db_;              // Pointer to sqlite database instance
    void InitializeDatabase(); // helper to create database tables
};
