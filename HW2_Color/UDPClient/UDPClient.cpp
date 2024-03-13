#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>

#include <iostream>
using namespace std;

int main()
{
    WSADATA wsadata;

    int res = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (res != NO_ERROR)
    {
        cout << "WSAStartup failked with error " << res << endl;
        return 1;
    }

    SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket == INVALID_SOCKET)
    {
        cout << "socket failed with error " << WSAGetLastError() << endl;
        return 2;
    }  

    sockaddr_in addrTo;
    addrTo.sin_family = AF_INET;
    addrTo.sin_port = htons(23000);
    inet_pton(AF_INET, "127.0.0.1", &addrTo.sin_addr.s_addr);

    const size_t sendBufSize = 1024;
    const size_t numBuffers = 3;
    char sendBuf[numBuffers][sendBufSize];

    string bufData[] = { "color code", "nickname", "message" };

    for (size_t i = 0; i < numBuffers; ++i)
    {
        cout << "Enter " << bufData[i] << ": ";
        cin >> sendBuf[i];

        int sendResult = sendto(udpSocket, sendBuf[i], strlen(sendBuf[i]), 0, (SOCKADDR*)&addrTo, sizeof(addrTo));
        if (sendResult == SOCKET_ERROR)
        {
            cout << "sendto failed with error " << WSAGetLastError() << endl;
            WSACleanup();
            return 4;
        }
        cout << "\033[90mSending " << bufData[i] << "....\033[0m" << endl;
    }

    closesocket(udpSocket);
    WSACleanup();
}

