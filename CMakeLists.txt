cmake_minimum_required(VERSION 3.13)

project(inotify-asio)

find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

add_executable(inotify-demo main.cpp)
target_include_directories(inotify-demo PRIVATE .)
target_link_libraries(inotify-demo PRIVATE Boost::headers Threads::Threads)
target_compile_features(inotify-demo PRIVATE cxx_std_17)