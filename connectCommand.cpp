
//
// Created by ofirn93 on 23/12/2019.
//

#include "connectCommand.h"
#include "Flag.h"
#include "Exp.h"
#include <sys/socket.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <vector>

/**opens socket for the client and sends its identifier to the symbol table
*/

using namespace std;

connectCommand::connectCommand() = default;

int connectCommand::execute(vector<string> lexer, int ind) {
    //create socket
    int client_socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socketfd == -1) {
        //error
        std::cerr << "Could not create a socket"<<std::endl;
        return -1;
    }

    float port = Exp::inter(lexer.at(ind + 2));
    string ip2 = lexer.at(ind + 1);
    const char* ip = ip2.substr(1,ip2.length()-2).c_str();
    //bind socket to IP address
    // we first need to create the sockaddr obj.
    sockaddr_in address; //in means IP4
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip); //give me any IP allocated for my machine
    address.sin_port = htons(port);
    //we need to convert our number to a number that the network understands.

    int is_connect = connect(client_socketfd, (struct sockaddr *)&address, sizeof(address));
    if (is_connect == -1) {
        std::cerr << "Could not connect to host server" << std::endl;
        return -2;
    } else {
        std:: cout << "Client is now connected to server"<< std::endl;
    }

    //send client socket number to symbol table
    symTable::getInstance()->clientSocketFD = client_socketfd;

    return 3;
}