#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <mutex>
#include "serial.h"
#include "GetSocket.h"
#include "motorControl.h"
#include "signalGenerator.h"

#define M_PI 3.14159265

int aaa = 0;
// ��ӡ����
void socketThread() {
    runServer();
}

float t_global = 0;
int motorNum = 4;//���ο��Ƶĵ������
//std::mutex mtx;  // ���ڱ��� functionCalls �Ļ�����


int main()
{
    double frequency = 3; // �������ֵ�Ըı�Ƶ��

    SerialPort serial("COM3", 2000000); // �������ڶ���

    if (!serial.isOpen()) {
        std::cerr << "ERROR: Unable to open serial port.\n";
        return 1;
    }

    auto start_time = std::chrono::steady_clock::now();
    auto next_time = start_time;

    // �������̲߳�����printHelloWorld����
    std::thread printThread(socketThread);


    // Initialize the Kalman filter


    while (true) {

        /////////////////////////Code for socket
        // std::cout << data << std::endl;
        /////////////////////////Code for serial
        next_time += std::chrono::milliseconds(1);
        double elapsed_time = std::chrono::duration<double>(next_time - start_time).count();

        clearMotorCurrentValue();
            
        {
            std::lock_guard<std::mutex> lock(mtx);
            for (std::vector<float>& v : functionPoolVector) {
                
                if (v[0] == 0x01) {
                    auto receivedCurrentValue = ((int)v[2] << 8) + (int)v[3];
                    motorBaseCurrentValue[(int)v[1]] = receivedCurrentValue; // ! Not+=, because Unity may send multiple packages, so 0x01 must write at front
                    
                    //if ((int)v[1] == 2) {
                    //    std::cout << motorBaseCurrentValue[2] << std::endl;
                    //}
                        
                    v[0] = 0xFF; //life over flag
                    std::cout << motorBaseCurrentValue[(int)v[1]] << std::endl;
                }
                else if (v[0] == 0x02) {

                    auto result = basicCollision(v[2], v[3], v[4], v[5], t_global);

                    motorCurrentValue[(int)v[1]] += 100 * result[0] + 50; //motor No. , must float to int

                    if (result[1] == 0) // if life over
                        v[0] = 0xFF; //life over flag
                }
            }
        }
        {
            std::lock_guard<std::mutex> lock(mtx);
            for (auto it = functionPoolVector.begin(); it != functionPoolVector.end(); ) { // clear all function package start with 0xFF
                if (!it->empty() && (*it)[0] == 0xFF) {

                    it = functionPoolVector.erase(it);
                }
                else {
                    ++it;
                }
            }
        }
        for (int num = 0; num < motorNum; num++) { //���յ���ת��Ϊ������Ʋ���
            auto result = intToHexProtocol(motorCurrentValue[num] + motorBaseCurrentValue[num]);
            val[num * 2] = result[0];
            val[num * 2 + 1] = result[1];
        }

        
        //if(abs(motorCurrentValue[1]) > 0.001)
        //    std::cout << motorCurrentValue[1] << std::endl;
       

        unsigned char data_to_sends[] = { 0x31, val[0], val[1], val[2], val[3], val[4], val[5], val[6], 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        DWORD bytes_to_send = sizeof(data_to_sends);
        DWORD bytes_written = 0;

        if (!serial.writeData(data_to_sends, bytes_to_send)) {
            std::cerr << "ERROR: Unable to write to serial port.\n";
            return 1;
        }

        //if (bytes_written != bytes_to_send) {
        //    std::cerr << "WARNING: Not all bytes were written to serial port.\n";
        //}

        std::this_thread::sleep_until(next_time);
        t_global += 0.001;
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (functionPoolVector.empty()) {
                t_global = 0;
            }
        }
        
    }

    // �ȴ�printThread�߳̽���
    printThread.join();

    return 0;
}





