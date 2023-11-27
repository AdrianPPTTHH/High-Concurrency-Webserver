cmake_minimum_required(VERSION 3.26.0)

project(webserver)

add_executable(run.exe
    ./source-code/main.cpp
    ./source-code/buffer/buffer.cpp
    ./source-code/epoller/epoller.cpp
    ./source-code/http/httprequest.cpp
    ./source-code/http/httpresponse.cpp
    ./source-code/http/httpconn.cpp
    ./source-code/timer/heaptimer.cpp
    ./source-code/log/log.cpp
    ./source-code/threadpool/threadpool.cpp
    ./source-code/sqlconnpool/sqlconnpool.cpp
    ./source-code/server/server.cpp
)

#C++版本
target_compile_features(run.exe PRIVATE cxx_std_17)

#头文件搜索路劲
target_include_directories(run.exe PRIVATE ./source-code)


target_link_libraries(run.exe mysqlclient)