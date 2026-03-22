#include <iostream>
#include "server.h"
using namespace std;

int main(){
    ProxyServer server(8080);
    server.start();

    cout << endl;
    return 0;
}