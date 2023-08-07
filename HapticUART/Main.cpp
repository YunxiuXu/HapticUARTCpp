#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <mutex>
#include "serial.h"
#include "GetSocket.h"
#include "motorControl.h"
#include "signalGenerator.h"
#include <string>


#define M_PI 3.14159265

int aaa = 0;
// 打印函数
void socketThread() {
    runServer();
}

float t_global = 0, t_global_continus = 0; //continus means not reset to 0
int motorNum = 8;//单次控制的电机数量
//std::mutex mtx;  // 用于保护 functionCalls 的互斥量


int main()
{
    //double frequency = 3; // 调整这个值以改变频率
    
    char userInput;
    std::string ComNum;
    std::cout << "Please input 0 for right hand 1 for left hand:" << std::endl;
    std::cin >> userInput;
    if (userInput == '0') {
        std::cout << "right hand" << std::endl;
        ComNum = "\\\\.\\COM6";
        port = 1233;
    }
    else{
        std::cout << "left hand" << std::endl;
        ComNum = "\\\\.\\COM5";
        port = 1234;
    }


    SerialPort serial(ComNum, 2000000); // 创建串口对象

    if (!serial.isOpen()) {
        std::cerr << "ERROR: Unable to open serial port.\n";
        return 1;
    }

    auto start_time = std::chrono::steady_clock::now();
    auto next_time = start_time;

    // 创建新线程并运行printHelloWorld函数
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
                    //motorBaseCurrentValue[(int)v[1]] = 0;
                    last_motorBaseCurrentValue[(int)v[1]] = motorBaseCurrentValue[(int)v[1]];
                    motorBaseCurrentValue[(int)v[1]] = receivedCurrentValue; // ! Not+=, because Unity may send multiple packages, so 0x01 must write at front
                    
                    tilt_motorBaseCurrentValue[(int)v[1]] = ((float)motorBaseCurrentValue[(int)v[1]] - (float)last_motorBaseCurrentValue[(int)v[1]]) / (1000 * 0.02);;

                    //if ((int)v[1] == 5) {
                    //    std::cout << receivedCurrentValue << std::endl;
                    //}
                    v[0] = 0xFF; //life over flag
                    //std::cout << motorBaseCurrentValue[(int)v[1]] << std::endl;
                }
                else if (v[0] == 0x02) {
                    auto receivedCurrentValue = ((int)v[4] << 8) + (int)v[5];
                    auto result = basicCollision(v[2], v[3], 144, v[6], t_global);

                    if (motorCurrentValue[(int)v[1]] > 256)
                        motorCurrentValue[(int)v[1]] = 256;

                    motorCurrentValue[(int)v[1]] += 0.5 * receivedCurrentValue * result[0]; //motor No. , must float to int

                    if (motorCurrentValue[(int)v[1]] < 0)
                        motorCurrentValue[(int)v[1]] = -motorCurrentValue[(int)v[1]];
                    

                    //if(motorCurrentValue[(int)v[1]] != 0)
                        //std::cout << motorCurrentValue[(int)v[1]] << std::endl;
                    if (result[1] == 0) // if life over
                        v[0] = 0xFF; //life over flag
                }
                else if (v[0] == 0x03) {
                    auto receivedCurrentValue = ((int)v[2] << 8) + (int)v[3];
                    auto result = 0;
                    if(receivedCurrentValue < 30)
                        result = calculateSin(receivedCurrentValue, 40, 0, t_global_continus)*7;
                    else if (receivedCurrentValue < 60)
                        result = calculateSin(receivedCurrentValue, 40, 0, t_global_continus) * 1;
                    else 
                        result = calculateSin(receivedCurrentValue, 40, 0, t_global_continus) * 0.2;

                    if (result > 50)
                    {
                        if (motorBaseCurrentValue[(int)v[1]] > 128)
                            motorBaseCurrentValue[(int)v[1]] = 128;
                    }
                    motorBaseCurrentValue[(int)v[1]] += result; // ! Not+=, because Unity may send multiple packages, so 0x01 must write at front

                    //if ((int)v[1] == 5) {
                        //std::cout << result << std::endl;
                    //}
                    v[0] = 0xFF; //life over flag
                }

            }
            //auto result = calculateSin(20, 30, 0, t_global1) - 40;
            //std::cout << result << "ggg"<< std::endl;
            //motorCurrentValue[5] += result;
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
        for (int num = 0; num < motorNum; num++) { //最终电流转换为电机控制参数
            last_motorBaseCurrentValue[num] += tilt_motorBaseCurrentValue[num];

            auto outputCurrent = motorCurrentValue[num] + last_motorBaseCurrentValue[num];

            if (isCooling[num] == 0 && motorQ[num] > MaxQ) //overHeat and not cooling
            {
                isCooling[num] = 1;
            }

            if (isCooling[num] == 1)
            {
                outputCurrent /= 3;
                if (motorQ[num] <= 0)
                    isCooling[num] = 0;
            }
                


            auto result = intToHexProtocol(outputCurrent);

            motorQ[num] += outputCurrent * outputCurrent / 1000 - DiffuseQ; // I2RDeltaT - DeltaT, omit somevalues
            if (motorQ[num] < 0)
                motorQ[num] = 0;
            //if (num == 5)
            //{
            //    if (outputCurrent > 0)
			//		std::cout << outputCurrent << ",";
				
            //}
               
            val[num * 2] = result[0];
            val[num * 2 + 1] = result[1];
        }

        
        
        //unsigned char data_to_sends[] = { 0x31, 0, 0, 0, 0, 0, 0, 0,0,0, 0, val[10], val[11], 0, 0, val[14], val[15]};
        unsigned char data_to_sends[] = { 0x31, val[0], val[1], val[2], val[3], val[4], val[5], val[6], val[7], val[8], val[9], val[10], val[11], val[12], val[13], val[14], val[15]};
        //for (int i = 0; i < sizeof(data_to_sends); i++) {
        //    if (i == 0)
        //        data_to_sends[0] = 0x31;
        //    data_to_sends[i] =  val[i];
        //}
        
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
        t_global_continus += 0.001;
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (functionPoolVector.empty()) {
                t_global = 0;
            }
        }
        
    }

    // 等待printThread线程结束
    printThread.join();

    return 0;
}





