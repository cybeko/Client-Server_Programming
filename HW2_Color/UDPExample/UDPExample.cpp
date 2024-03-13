#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <iostream>
using namespace std;

int main() 
{
    WSADATA wsadata;

    int res = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (res != NO_ERROR)
    {
        cout << "WSAStartup failed with error " << res << endl;
        return 1;
    }

    SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket == INVALID_SOCKET)
    {
        cout << "socket failed with error " << WSAGetLastError() << endl;
        return 2;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(23000);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);

    if (bind(udpSocket, (SOCKADDR*)&addr, sizeof(addr)) != NO_ERROR)
    {
        cout << "bind failed with error " << WSAGetLastError() << endl;
        return 3;
    }
    const size_t receiveBufSize = 1024;
    char receiveBuf[receiveBufSize];
    sockaddr_in senderAddr;
    int senderAddrSize = sizeof(senderAddr);
    string bufferNames[] = { "Color", "Nickname", "Message" };
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    for (size_t i = 0; i < sizeof(bufferNames) / sizeof(bufferNames[0]); ++i)
    {
        cout << "Receiving " << bufferNames[i] << "...." << endl;

        int bytesReceived = recvfrom(udpSocket, receiveBuf, receiveBufSize, 0, (SOCKADDR*)&senderAddr, &senderAddrSize);

        if (bytesReceived == SOCKET_ERROR)
        {
            cout << "recvfrom failed with error " << WSAGetLastError() << endl;
            return 4;
        }
        receiveBuf[bytesReceived] = '\0';
        cout << bufferNames[i] << ": " << receiveBuf << endl;
        bufferNames[i] = receiveBuf;
    }
    cout << "\n[Message received]\n" << endl;
    SetConsoleTextAttribute(hConsole, atoi(bufferNames[0].c_str()));
    cout << bufferNames[1] << ": " << bufferNames[2] << endl;


    closesocket(udpSocket);
    WSACleanup();

    return 0;
}

