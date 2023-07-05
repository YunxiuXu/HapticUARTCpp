#include "GetSocket.h"
#include "motorControl.h"
#include "signalGenerator.h"
#include <iostream>
#include <WS2tcpip.h>
#include <string>

#pragma comment (lib, "ws2_32.lib")

int data = 0;

void runServer()
{
    
    std::string ipAddress = "127.0.0.1";
    int port = 1233;

    // ��ʼ��Winsock
    WSAData wsData;
    WORD ver = MAKEWORD(2, 2);
    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0)
    {
        std::cerr << "Can't Initialize winsock! Quitting" << std::endl;
        return;
    }

    while (true)
    {
        // �����׽���
        SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
        if (listening == INVALID_SOCKET)
        {
            std::cerr << "Can't create a socket! Quitting" << std::endl;
            WSACleanup();
            return;
        }

        // ��IP��ַ�Ͷ˿ڵ��׽���
        sockaddr_in hint;
        hint.sin_family = AF_INET;
        hint.sin_port = htons(port);
        inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

        if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR)
        {
            std::cerr << "Can't bind socket! Quitting" << std::endl;
            closesocket(listening);
            WSACleanup();
            return;
        }

        // �����׽���Ϊ����״̬
        if (listen(listening, SOMAXCONN) == SOCKET_ERROR)
        {
            std::cerr << "Can't listen! Quitting" << std::endl;
            closesocket(listening);
            WSACleanup();
            return;
        }

        // ��������
        sockaddr_in client;
        int clientSize = sizeof(client);
        SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Problem with client connecting! Quitting" << std::endl;
            closesocket(listening);
            WSACleanup();
            return;
        }

        char host[NI_MAXHOST];
        char svc[NI_MAXSERV];

        ZeroMemory(host, NI_MAXHOST);
        ZeroMemory(svc, NI_MAXSERV);

        if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0) == 0)
        {
            std::cout << host << " connected on port " << svc << std::endl;
        }

        // �رռ����׽���
        closesocket(listening);

        // ѭ����������
        unsigned char buf[4096];
        
        while (true)
        {
            ZeroMemory(buf, 4096);
            int bytesReceived = recv(clientSocket, (char*)buf, 4096, 0);
            if (bytesReceived == SOCKET_ERROR)
            {
                std::cerr << "Error in recv(). Quitting" << std::endl;
                break;
            }

            if (bytesReceived == 0)
            {
                std::cout << "Client disconnected " << std::endl;
                break;
            }
            unsigned char num = buf[4];
            //motorPushByteValue[1] = 0;
            //std::cout << "command 01: " << std::string((char*)buf, 0, bytesReceived) << std::endl;
            //motorPushByteValue[1] = num;
            if(buf[2] == 0x02)
                addFunctionCall([](double t, const std::tuple<double, double, double>& args) { return basicCollision(std::get<0>(args), std::get<1>(args), std::get<2>(args), t); }, 14, 309, 67);
            data += 1;
        }

        // �ر��׽���
        closesocket(clientSocket);
    }

    // ����Winsock
    WSACleanup();
}
