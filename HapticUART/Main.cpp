#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include "serial.h"
#include "GetSocket.h"
#include "motorControl.h"
#include "signalGenerator.h"

#define M_PI 3.14159265

int aaa = 0;
// 打印函数
void socketThread() {
    runServer();
}

float t = 0;

int main()
{
    double frequency = 3; // 调整这个值以改变频率

    SerialPort serial("COM3", 2000000); // 创建串口对象

    if (!serial.isOpen()) {
        std::cerr << "ERROR: Unable to open serial port.\n";
        return 1;
    }

    auto start_time = std::chrono::steady_clock::now();
    auto next_time = start_time;

    // 创建新线程并运行printHelloWorld函数
    std::thread printThread(socketThread);

    while (true) {

        /////////////////////////Code for socket
        // std::cout << data << std::endl;
        /////////////////////////Code for serial
        next_time += std::chrono::milliseconds(1);
        double elapsed_time = std::chrono::duration<double>(next_time - start_time).count();

        //int data_to_send = ((127 * (std::sin(2 * M_PI * frequency * elapsed_time) + 1.0)));
        //data_to_send = static_cast<float>(data_to_send) / 255 * 300;

        //auto result = intToHexProtocol(data_to_send);
        pushValue2Current();
        auto result = intToHexProtocol(motorCurrentValue[0]);
        //std::cout << motorCurrentValue[0] << std::endl;
        
        for (const HapticFunctionCall& fc : functionCalls) {
            double result = fc.function(t, fc.args);

            // 输出结果
            std::cout << result << std::endl;
        }


        unsigned char data_to_sends[] = { 0x31, result[0], result[1], result[0], result[1], 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
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
        t += 0.001;
        if (functionCalls.empty()) {
            t = 0;
        }
    }

    // 等待printThread线程结束
    printThread.join();

    return 0;
}
