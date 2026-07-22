#include <iostream>
#include "database/Database.h"
#include <stdexcept>
#include<optional>

// Database class :: constructor name
Database::Database()
{
    // 1. Open the database. This creates the file "url_shortener.db" if it doesn't exist.
    if (sqlite3_open("url_shortner.db", &db_) != SQLITE_OK)
    {
        throw std::runtime_error("Failed to open sqlite database...");
    }
    InitializeDatabase();
}

Database::~Database()
{
    sqlite3_close(db_); // cleanup the connection when server stops
}

void Database::InitializeDatabase()
{
    // sql command to create our table
    // short_code is the PRIMARY KEY
    const char *createTableSQL = "CREATE TABLE IF NOT EXISTS urls ("
                                 "short_code TEXT PRIMARY KEY, "
                                 "original_url TEXT NOT NULL);";

    char *errMsg = nullptr;
    if (sqlite3_exec(db_, createTableSQL, nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::string error = errMsg;
        sqlite3_free(errMsg);
        throw std::runtime_error("Failed to create table: " + error);
    }
}

void Database::SaveUrl(const std::string &shortCode, const std::string &originalUrl)
{
    const char *insertSQL = "INSERT INTO urls (short_code, original_url) VALUES (?, ?);";
    sqlite3_stmt *stmt; // this pointer will point to the preapred sql statement

    // preprare the sql statement
    if (sqlite3_prepare_v2(db_, insertSQL, -1, &stmt, nullptr) == SQLITE_OK)
    {
        // Bind the C++ strings to the '?' placeholders in the SQL query
        // SQLITE_TRANSIENT tells SQLite to make a copy of the string data safely.
        sqlite3_bind_text(stmt, 1, shortCode.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, originalUrl.c_str(), -1, SQLITE_TRANSIENT);
    }
    // Execute the statement
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        std::cerr << "Failed to insert URL.\n";
    }

// Clean up the statement memory
    sqlite3_finalize(stmt);
}

std::optional<std::string> Database::GetUrl(const std::string& shortCode) const{
    const char* selectSQL = "SELECT original_url FROM urls WHERE short_code = ?;";
    sqlite3_stmt* stmt;
    std::optional<std::string> result = std::nullopt;

    if (sqlite3_prepare_v2(db_, selectSQL, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, shortCode.c_str(), -1, SQLITE_TRANSIENT);

        // Execute and check if we got a ROW back
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            // Read the text from the first column (index 0)
            const unsigned char* text = sqlite3_column_text(stmt, 0);
            result = std::string(reinterpret_cast<const char*>(text));
        }
    }
    sqlite3_finalize(stmt);
    return result;
}

bool Database::Exists(const std::string& shortCode) const {
    // If GetUrl returns something (has a value), then it exists
    return GetUrl(shortCode).has_value();
}