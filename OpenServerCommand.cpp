//
// Created by avichai on 01/01/2020.
//

#include "OpenServerCommand.h"
#include "Flag.h"
#include "symTable.h"
#include "Substring.h"
#include "Exp.h"
#include "Mutex.h"
#include <sys/socket.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <vector>
#include <thread>

/**Connects to the simulator as a server, and updates variables values
 * according to what we get from the simulator
 *
 */

using namespace std;

//listens to the simulator as long as the file isn't fully parsed, and updates
//the values of the variables in the map accordingly
void receiveFromSim(int client_socket) {
    //create vector of variables' paths
    vector<string> loc = OpenServerCommand::createLoc();
    //listen to the simulator as long as the file isn't fully parsed, and update
    // the values of the variables in the map accordingly
    while (Flag::getInstance()->threadFlag) {
        //reading from client
        char buffer[1024] = {0};
        int valread = read(client_socket, buffer, 1024);
        if (valread < 1) {
            cerr << "Error while reading from simulator" << endl;
        }
        string buf(buffer);
        int i = 0, count = 0;
        //traverse the buffer until you hit a '\n', split the values into strings,
        //and update the variables in the map accordingly
        while (i < buf.size() && buf[i-1] != '\n') {
            string str;
            //split the values by ',' or until you meet '\n'
            while (buf[i] != ',' && buf[i] != '\n') {
                str += buf[i];
                i++;
            }
            //update the map, in which the key is the variable's path
            char *end;
            float val = strtod(str.c_str(), &end);
            if (symTable::getInstance()->getSiVar(loc.at(count)) != nullptr) {
                symTable::getInstance()->siMap.at(loc.at(count))->setVal(val);
            }
            count++;
            i++;
        }
    }
    close(client_socket);
}

//open a socket to connect to the simulator
int OpenServerCommand::execute(vector<string> lexer, int ind) {

    //create socket
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == -1) {
        //error
        cerr << "Could not create a socket" << endl;
        return 0;
    }
    float port = Exp::inter(lexer.at(ind + 1));
    //bind socket to IP address
    // we first need to create the sockaddr obj.
    sockaddr_in address; //in means IP4
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; //give me any IP allocated for my machine
    address.sin_port = htons(port); //we need to convert our number to a number that the network understands

    //the actual bind command
    if (bind(socketfd, (struct sockaddr *) &address, sizeof(address)) == -1) {
        cerr << "Could not bind the socket to an IP" << endl;
        return 0;
    }

    //making socket listen to the port
    if (listen(socketfd, 5) == -1) { //can also set to SOMAXCON (max connections)
        cerr << "Error during listening command" << endl;
        return 0;
    } else {
        cout << "Server is now listening ..." << endl;
    }

    // accepting a client
    int client_socket = accept(socketfd, (struct sockaddr *) &address, (socklen_t *) &address);

    if (client_socket == -1) {
        cerr << "Error accepting client" << endl;
        return 0;
    } else {
        cout << "Connected" << endl;
    }

    close(socketfd); //closing the listening socket
    //call the thread that listens to the simulator and updates values
    thread thread1(receiveFromSim, client_socket);
    thread1.detach();


    return 2;
}

//create a vector that holds all of the variables' paths
vector<string> OpenServerCommand::createLoc() {
    string loc[36];
    loc[0] = "/instrumentation/airspeed-indicator/indicated-speed-kt";
    loc[1] = "/sim/time/warp";
    loc[2] = "/controls/switches/magnetos";
    loc[3] = "/instrumentation/heading-indicator/offset-deg";
    loc[4] = "/instrumentation/altimeter/indicated-altitude-ft";
    loc[5] = "/instrumentation/altimeter/pressure-alt-ft";
    loc[6] = "/instrumentation/attitude-indicator/indicated-pitch-deg";
    loc[7] = "/instrumentation/attitude-indicator/indicated-roll-deg";
    loc[8] = "/instrumentation/attitude-indicator/internal-pitch-deg";
    loc[9] = "/instrumentation/attitude-indicator/internal-roll-deg";
    loc[10] = "/instrumentation/encoder/indicated-altitude-ft";
    loc[11] = "/instrumentation/encoder/pressure-alt-ft";
    loc[12] = "/instrumentation/gps/indicated-altitude-ft";
    loc[13] = "/instrumentation/gps/indicated-ground-speed-kt";
    loc[14] = "/instrumentation/gps/indicated-vertical-speed";
    loc[15] = "/instrumentation/heading-indicator/indicated-heading-deg";
    loc[16] = "/instrumentation/magnetic-compass/indicated-heading-deg";
    loc[17] = "/instrumentation/slip-skid-ball/indicated-slip-skid";
    loc[18] = "/instrumentation/turn-indicator/indicated-turn-rate";
    loc[19] = "/instrumentation/vertical-speed-indicator/indicated-speed-fpm";
    loc[20] = "/controls/flight/aileron";
    loc[21] = "/controls/flight/elevator";
    loc[22] = "/controls/flight/rudder";
    loc[23] = "/controls/flight/flaps";
    loc[24] = "/controls/engines/engine/throttle";
    loc[25] = "/controls/engines/current-engine/throttle";
    loc[26] = "/controls/switches/master-avionics";
    loc[27] = "/controls/switches/starter";
    loc[28] = "/engines/active-engine/auto-start";
    loc[29] = "/controls/flight/speedbrake";
    loc[30] = "/sim/model/c172p/brake-parking";
    loc[31] = "/controls/engines/engine/primer";
    loc[32] = "/controls/engines/current-engine/mixture";
    loc[33] = "/controls/switches/master-bat";
    loc[34] = "/controls/switches/master-alt";
    loc[35] = "/engines/engine/rpm";

    //create a vector in which the i'th index == the above array's i'th index
    vector<string> location;
    location.reserve(36);
    for (const auto &i : loc) {
        location.push_back(i);
    }
    return location;
}

OpenServerCommand::OpenServerCommand() = default;