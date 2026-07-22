cmake_minimum_required(VERSION 3.10)
project(URL_SHORTNER)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(src)

set(SOURCES
    src/main.cpp
    src/server/Server.cpp
    src/database/Database.cpp
    src/service/UrlService.cpp
    src/utils/Random.cpp
)

add_executable(URL_SHORTNER ${SOURCES})

# Link the standard SQLite3 library
target_link_libraries(URL_SHORTNER sqlite3)