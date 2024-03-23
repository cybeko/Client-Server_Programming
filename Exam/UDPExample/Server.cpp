#include <winsock2.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include <map>
#include <algorithm>
using namespace std;

#define MAX_CLIENTS 30
#define DEFAULT_BUFLEN 4096

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

SOCKET server_socket;
vector<string> history;
WSADATA wsa;

struct MyMenu
{
    string name;
    int cost;
    int time;
};

struct Order
{
    int id;
    vector<string> dishes;
    int totalCost;
    int totalTime;
};

vector<Order> AllOrders;

vector<MyMenu> Menu = { {"Fries", 5, 3}, {"Hamburger", 7, 7}, {"Nuggets", 4, 5}, {"Cola", 4, 4} };
int timeToWait;

string MenuString()
{
    string menuStr;
    menuStr += "Menu:\n";
    for (int i = 0; i < Menu.size(); ++i)
    {
        menuStr += Menu[i].name + ": $" + to_string(Menu[i].cost) + " [time: " + to_string(Menu[i].time) + "]\n";
    }
    return menuStr;
}
string StringLowercase(const std::string& str) 
{
    string result = str;
    for (int i = 0; i < result.size(); i++) {
        result[i] = tolower(result[i]);
    }
    return result;
}

void printOrder(const Order& order) {
    cout << "\n[New order]" << endl;
    cout << "Order ID: " << order.id << endl;
    cout << "Dishes:" << endl;
    for (int i = 0; i < order.dishes.size(); ++i) {
        cout<< order.dishes[i] << endl;
    }
    cout << "Total Cost: $" << order.totalCost << endl;
    cout << "Total Time: " << order.totalTime << " minutes\n" << endl;
}
string CalcAndReturnOrderInfo(string str) {
    string orderInfoStr;
    string word;
    size_t pos = 0;
    size_t space;
    int totalCost = 0;
    int orderId = AllOrders.size() + 1;
    Order newOrder;
    newOrder.id = orderId;

    orderInfoStr += "Your order: \n";
    transform(str.begin(), str.end(), str.begin(), ::tolower);

    while ((space = str.find(' ', pos)) != string::npos) {
        word = str.substr(pos, space - pos);
        for (int i = 0; i < Menu.size(); ++i) {
            if (word == StringLowercase(Menu[i].name)) {
                newOrder.dishes.push_back(Menu[i].name);
                orderInfoStr += Menu[i].name + ": " + to_string(Menu[i].cost) + "$\n";
                timeToWait += Menu[i].time;
                totalCost += Menu[i].cost;
                break;
            }
        }
        pos = space + 1;
    }
    if (pos < str.length()) {
        word = str.substr(pos);
        for (int i = 0; i < Menu.size(); ++i) {
            if (word == StringLowercase(Menu[i].name)) {
                newOrder.dishes.push_back(Menu[i].name);
                orderInfoStr += Menu[i].name + ": " + to_string(Menu[i].cost) + "$\n";
                timeToWait += Menu[i].time;
                totalCost += Menu[i].cost;
                break;
            }
        }
    }
    newOrder.totalCost = totalCost;
    newOrder.totalTime = timeToWait;
    AllOrders.push_back(newOrder);
    printOrder(newOrder);
    orderInfoStr += "\nTotal cost: " + to_string(totalCost) + " $\n";
    orderInfoStr += "Total wait time: " + to_string(timeToWait) + " minutes\n";
    if (timeToWait > 0) {
        return orderInfoStr;
    }
    else {
        return "0";
    }
}

int CreateServer()
{
    puts("Server started");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code: %d", WSAGetLastError());
        return 1;
    }

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket: %d", WSAGetLastError());
        return 2;
    }

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    if (bind(server_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code: %d", WSAGetLastError());
        return 3;
    }

    listen(server_socket, MAX_CLIENTS);
    puts("Server is waiting for incoming connections...\nPlease, start one or more client-side app.");

    return 0;
}

DWORD WINAPI HandleClient(LPVOID lpParam)
{
    SOCKET client_socket = (SOCKET)lpParam;

    char client_message[DEFAULT_BUFLEN];

    int addrlen = sizeof(sockaddr_in);
    sockaddr_in address;
    getpeername(client_socket, (sockaddr*)&address, (int*)&addrlen);

    int client_message_length;
    while ((client_message_length = recv(client_socket, client_message, DEFAULT_BUFLEN, 0)) > 0)
    {
        client_message[client_message_length] = '\0';

        string str_client_message = client_message;
        if (timeToWait > 0)
        {
            string busyStr = "Your order will be processed when previous clinet's order is done.\n";
            send(client_socket, busyStr.c_str(), busyStr.size(), 0);
        }
        string sendTimeStr = CalcAndReturnOrderInfo(str_client_message);
        if(sendTimeStr!="0")
        {
            send(client_socket, sendTimeStr.c_str(), sendTimeStr.size(), 0);
            Sleep(timeToWait * 1000);
            string orderDone = "Your order is done!\n";
            timeToWait = 0;
            send(client_socket, orderDone.c_str(), orderDone.size(), 0);
        }
        else
        {
            string wrongOrder = "No such item on the menu.\n";
            send(client_socket, wrongOrder.c_str(), wrongOrder.size(), 0);
        }

        if (str_client_message == "end")
        {
            cout << "Client disconnected" << endl;
            break;
        }

        string temp = client_message;
        history.push_back(temp);
    }

    closesocket(client_socket);
    return 0;
}

int main()
{
    system("title Server");
    CreateServer();

    fd_set readfds; // https://docs.microsoft.com/en-us/windows/win32/api/winsock/ns-winsock-fd_set
    SOCKET client_socket[MAX_CLIENTS] = {};

    while (true) {
        FD_ZERO(&readfds);

        FD_SET(server_socket, &readfds);

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            SOCKET s = client_socket[i];
            if (s > 0) {
                FD_SET(s, &readfds);
            }
        }

        if (select(0, &readfds, NULL, NULL, NULL) == SOCKET_ERROR) {
            printf("select function call failed with error code : %d", WSAGetLastError());
            return 4;
        }

        SOCKET new_socket;
        sockaddr_in address;
        int addrlen = sizeof(sockaddr_in);

        if (FD_ISSET(server_socket, &readfds)) {
            if ((new_socket = accept(server_socket, (sockaddr*)&address, &addrlen)) < 0) {
                perror("accept function error");
                return 5;
            }

            string menuStr = MenuString();
            send(new_socket, menuStr.c_str(), menuStr.size(), 0);

            printf("New client, socket is %d\n", new_socket);

            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_socket[i] == 0) 
                {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets at index %d\n", i);
                    DWORD threadId;
                    CreateThread(NULL, 0, HandleClient, (LPVOID)new_socket, 0, &threadId);
                    break;
                }
            }
        }
    }

    WSACleanup();
    return 0;
}
