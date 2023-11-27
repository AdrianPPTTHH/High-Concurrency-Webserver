#include "./server/server.h"
#include <iostream>

using namespace std;

int main(){

    WebServer webserver(
        8876,        //port
        3,           //trigMode
        60000,       //timeoutMS
        false,        //OptLinger
        "localhost", //sqlHost
        3306,        //sqlPort
        "web_admin", //sqlUsername
        "asd123456", //sqlPassword
        "webserver", //dbname
        30,          //sqlconnPoolNum
        50,          //threadNum,
        true,        //openLog
        1,           //logLevel
        2048         //logSize
    );

    webserver.Start();
    
    return 0;
}