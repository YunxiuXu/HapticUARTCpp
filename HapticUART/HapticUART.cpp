#include <windows.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>

#include <vector>
#include <stdexcept>

#define PI 3.14159265


std::vector<unsigned char> intToHexProtocol(int num) {
    // Check if the number is in range -2048 to 2048
    if (num < -2048 || num > 2048) {
        throw std::invalid_argument("The number must be in range -2048 to 2048.");
    }

    std::vector<unsigned char> result(2);

    if (num >= 0) {
        // If the number is non-negative, it is represented directly.
        result[0] = num & 0xFF;  // Low byte
        result[1] = (num >> 8) & 0xFF;  // High byte
    }
    else {
        // If the number is negative, it is represented in two's complement form.
        num = -num;  // Take the absolute value
        result[0] = num & 0xFF;  // Low byte
        result[1] = (num >> 8) & 0xFF;  // High byte
        result[1] = ~result[1];  // Bitwise NOT operation
        result[0] = ~result[0];  // Bitwise NOT operation
        if (result[0] == 255) {
            result[0] = 0;
            result[1] += 1;
        }
        else {
            result[0] += 1;
        }
    }

    return result;
}


int main()
{
    HANDLE hSerial;

    // �򿪴���
    hSerial = CreateFile(L"\\\\.\\COM3", // COM�˿ں�
        GENERIC_READ | GENERIC_WRITE, // ��дģʽ
        0, // ���ܱ�����
        NULL, // ��ȫ����
        OPEN_EXISTING, // �����еĴ����ļ�
        0, // ������ģʽ
        NULL); // û��ģ���ļ�

    if (hSerial == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            std::cerr << "ERROR: Serial port doesn't exist.\n";
        }
        else {
            std::cerr << "ERROR: Unable to open serial port.\n";
        }
        return 1;
    }

    // ���ô���
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "ERROR: Unable to get current serial port state.\n";
        return 1;
    }

    dcbSerialParams.BaudRate = 2000000; // ���ò�����
    dcbSerialParams.ByteSize = 8; // ����λΪ8λ
    dcbSerialParams.StopBits = ONESTOPBIT; // һ��ֹͣλ
    dcbSerialParams.Parity = NOPARITY; // ��У��λ

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "ERROR: Unable to configure serial port.\n";
        return 1;
    }

    // ׼�����͵�����
    double frequency = 1; // �������ֵ�Ըı�Ƶ��
    auto start_time = std::chrono::steady_clock::now();
    auto next_time = start_time;
    unsigned char data_to_sends[] = { 0x31,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x21,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 }; // ��Ҫ���͵�����


    

    while (true) {
        next_time += std::chrono::milliseconds(1);
        double elapsed_time = std::chrono::duration<double>(next_time - start_time).count();
        int data_to_send = ((127 * (std::sin(2 * PI * frequency * elapsed_time) + 1.0))) - 40 ; // �������Ҳ���ֵ
        data_to_send = (float)data_to_send / 255 * 400;
        //char numberStr[20];
        //sprintf_s(numberStr, "%d\n", data_to_send);
        //// ���ַ�����������Դ���
        //OutputDebugStringA(numberStr);

        auto result = intToHexProtocol((int)data_to_send);
        data_to_sends[1] = result[0];
        data_to_sends[2] = result[1];
        //data_to_sends[1]
        DWORD bytes_to_send = sizeof(data_to_sends); // Ҫ���͵��ֽ���
        DWORD bytes_written = 0;

        if (!WriteFile(hSerial, &data_to_sends, bytes_to_send, &bytes_written, NULL)) {
            std::cerr << "ERROR: Unable to write to serial port.\n";
            return 1;
        }

        if (bytes_written != bytes_to_send) {
            std::cerr << "WARNING: Not all bytes were written to serial port.\n";
        }

        std::this_thread::sleep_until(next_time); // ����ֱ��ָ��ʱ��
    }

    // �رմ���
    CloseHandle(hSerial);

    return 0;
}




//#include <windows.h>
//#include <iostream>
//#include <chrono>
//#include <thread>
//
//int main()
//{
//    HANDLE hSerial;
//
//    // �򿪴���
//    hSerial = CreateFile(L"COM6", // COM�˿ں�
//        GENERIC_READ | GENERIC_WRITE, // ��дģʽ
//        0, // ���ܱ�����
//        NULL, // ��ȫ����
//        OPEN_EXISTING, // �����еĴ����ļ�
//        0, // ������ģʽ
//        NULL); // û��ģ���ļ�
//
//    if (hSerial == INVALID_HANDLE_VALUE) {
//        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
//            std::cerr << "ERROR: Serial port doesn't exist.\n";
//        }
//        else {
//            std::cerr << "ERROR: Unable to open serial port.\n";
//        }
//        return 1;
//    }
//
//    // ���ô���
//    DCB dcbSerialParams = { 0 };
//    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
//    if (!GetCommState(hSerial, &dcbSerialParams)) {
//        std::cerr << "ERROR: Unable to get current serial port state.\n";
//        return 1;
//    }
//
//    dcbSerialParams.BaudRate = 2000000; // ���ò�����
//    dcbSerialParams.ByteSize = 8; // ����λΪ8λ
//    dcbSerialParams.StopBits = ONESTOPBIT; // һ��ֹͣλ
//    dcbSerialParams.Parity = NOPARITY; // ��У��λ
//
//    if (!SetCommState(hSerial, &dcbSerialParams)) {
//        std::cerr << "ERROR: Unable to configure serial port.\n";
//        return 1;
//    }
//
//    // ׼�����͵�����
//    char data_to_send[] = { 0x31,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x21,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 }; // ��Ҫ���͵�����
//    DWORD bytes_to_send = sizeof(data_to_send); // Ҫ���͵��ֽ���
//
//    auto next_time = std::chrono::steady_clock::now();
//    while (true) {
//        next_time += std::chrono::milliseconds(1);
//        DWORD bytes_written = 0;
//        if (!WriteFile(hSerial, data_to_send, bytes_to_send, &bytes_written, NULL)) {
//            std::cerr << "ERROR: Unable to write to serial port.\n";
//            return 1;
//        }
//
//        if (bytes_written != bytes_to_send) {
//            std::cerr << "WARNING: Not all bytes were written to serial port.\n";
//        }
//
//        std::this_thread::sleep_until(next_time); // ����ֱ��ָ��ʱ��
//    }
//
//    // �رմ���
//    CloseHandle(hSerial);
//
//    return 0;
//}

