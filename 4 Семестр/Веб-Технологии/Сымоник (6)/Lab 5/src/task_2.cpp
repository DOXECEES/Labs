#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <winsock2.h>
#include <sstream>

#pragma comment(lib, "Ws2_32.lib")


enum class State
{
    READY,
    PAUSED,
    STOPED
};

volatile State serverState = State::READY;


void handleClient(SOCKET clientSocket) {

    int bytesReceived;
    std::string response;
    char buffer[1024];


    do {
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0)
        {
            buffer[bytesReceived] = '\0';
            std::string request(buffer);
            std::cout << request << std::endl;

            std::vector <std::string> tokens;
            std::istringstream iss(request);
            std::string s;

            while (std::getline(iss, s, ' '))
            {
                tokens.push_back(s);
            }

            std::string command = tokens[0];


            if (command == "exit") {
                response = "Сервер выключается";
                serverState = State::STOPED;
            }
            else if (command == "pause")
            {
                response = "Сервер на паузе.";
                serverState = State::PAUSED;

            }
            else if (command == "ready")
            {
                response = "Сервер в состоянии готовности";
                serverState = State::READY;
            }
            else if (command == "echo")
            {
                if (tokens.size() > 1)
                {
                    response = tokens[1];
                }
                else
                    response = "Недостаточно аргументов";
            }
            else if (command == "rand")
            {
                response = std::to_string(0 + rand() % 100);
            }
            else
            {
                response = "Неизвестная команда";
            }

            send(clientSocket, response.c_str(), response.size(), 0);
        }
    } while (bytesReceived > 0 && response != "Сервер выключается.");

    closesocket(clientSocket);
}

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    srand(time(NULL));

    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);

    if (WSAStartup(ver, &wsData) != 0) {
        std::cerr << "Неудалось инициализировать winsock" << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Неудалось создать сокет" << std::endl;
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(54000);
    serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Неудалось привязать сокет" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Неудалось запустить прослушивание" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Сервер запущен." << std::endl;

    while (true) {

        if (serverState == State::STOPED)
            break;

        if (serverState != State::PAUSED)
        {
            SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
            if (clientSocket == INVALID_SOCKET) {
                std::cerr << "Неудалось притяль клиената" << std::endl;
                closesocket(serverSocket);
                WSACleanup();
                return 1;
            }

            std::thread clientThread(handleClient, clientSocket);
            clientThread.join();
        }
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
