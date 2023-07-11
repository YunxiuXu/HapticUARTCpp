#include "GetSocket.h"
#include "motorControl.h"
#include "signalGenerator.h"
#include <iostream>
#include <WS2tcpip.h>
#include <string>

#pragma comment (lib, "ws2_32.lib")

extern float t_global;

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

            int n = sizeof(buf) / sizeof(buf[0]);

            std::vector<std::vector<unsigned char>> receviedPackageList; // divide all received command package inside
            std::vector<unsigned char> temp;

            for (int i = 0; i < n; ++i) { //divide code
                if (buf[i] == 0xFF && i + 1 < n && buf[i + 1] == 0xFE) {
                    if (!temp.empty()) {
                        receviedPackageList.push_back(temp);
                        temp.clear();
                    }
                    ++i;  // skip the 0xFE
                }
                else {
                    temp.push_back(buf[i]);
                }
            }

            for (const auto& oneCommand : receviedPackageList){
                if (oneCommand[0] == 0x02) {
                    std::lock_guard<std::mutex> lock(mtx); // must add lock, or the program will crash
                    //addFunctionCall([](double t, const std::tuple<float, double, double, double>& args) { return basicCollision(std::get<0>(args), std::get<1>(args), std::get<2>(args), std::get<3>(args), t); }, t_global, 14, 309, 67);
                    functionPoolVector.push_back({0x02, (float)oneCommand[1], t_global, 14, 309, 67 }); // command, motorNum,t0, values for functions
                }
                else if (oneCommand[0] == 0x01) {
                    std::lock_guard<std::mutex> lock(mtx); // must add lock, or the program will crash
                    //addFunctionCall([](double t, const std::tuple<float, double, double, double>& args) { return basicCollision(std::get<0>(args), std::get<1>(args), std::get<2>(args), std::get<3>(args), t); }, t_global, 14, 309, 67);
                    functionPoolVector.push_back({ 0x01, (float)oneCommand[1], (float)oneCommand[2], (float)oneCommand[3] }); // command, motorNum,high bit, low bit
                }
            }
         
        }

        // �ر��׽���
        closesocket(clientSocket);
    }

    // ����Winsock
    WSACleanup();
}
