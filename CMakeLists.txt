cmake_minimum_required(VERSION 3.16)

project(URL_SHORTNER)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(URL_SHORTNER
    src/main.cpp
    src/server/Server.cpp
    src/service/UrlService.cpp
    src/database/database.cpp
    src/utils/Random.cpp
)
target_include_directories(URL_SHORTNER PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)