cmake_minimum_required(VERSION 3.8)

project(BallKnowledge)

find_package(CURL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(BallKnowledge
    src/main.cpp
    src/apiClient.cpp
    src/matchMonitor.cpp
    src/Notification.cpp
)

target_link_libraries(BallKnowledge
    PRIVATE
        CURL::libcurl
        nlohmann_json::nlohmann_json
)

set_property(TARGET BallKnowledge PROPERTY CXX_STANDARD 20)