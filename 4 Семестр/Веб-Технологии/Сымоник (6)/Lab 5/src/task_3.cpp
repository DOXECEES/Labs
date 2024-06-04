#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <string>
#include <winsock2.h>
#include <chrono>
#include <limits>

#pragma comment(lib, "Ws2_32.lib")


int main() {
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    if (WSAStartup(ver, &wsData) != 0) {
        std::cerr << "Can't initialize Winsock! Quitting" << std::endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Can't create socket! Quitting" << std::endl;
        WSACleanup();
        return 1;
    }

    while (true)
    {
        std::string serverAddres = "";
        std::string port = "";
        std::getline(std::cin, serverAddres);
        std::getline(std::cin, port);
        int iPort = 0;

        std::istringstream ss(port);
        ss >> iPort;


        sockaddr_in serverAddr = { };
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(iPort);
        serverAddr.sin_addr.S_un.S_addr = inet_addr(serverAddres.c_str()); 

        if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Неудалось подключится к серверу!" <<std::endl;
            if (WSAGetLastError() == WSAECONNREFUSED)
            {
                std::cout << "неверный IP или порт" << std::endl;
                continue;
            }
            else
            {
                std::cout << "Неизвестная ошибка: " << WSAGetLastError() << std::endl;
                return -1;
            }
        }

        std::cout << "Connected to server." << std::endl;
        break;
    }
    std::string userInput;
    do {
        std::cout << "Enter command: ";

        std::getline(std::cin, userInput);
        send(clientSocket, userInput.c_str(), userInput.size(), 0);

        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

        char buffer[1024];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::cout << "Server response: " << buffer << std::endl;
            std::cout << "Время обработки: "
                << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count()
                << " секунд "
                << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
                << " миллисекунд "
                << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()
                << " микросекунд "
                << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()
                << " наносекунд "
                << std::endl;

        }
    } while (userInput != "exit");

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
