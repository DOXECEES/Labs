#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <vector>

#include <openssl/err.h>
#include <openssl/ssl.h>

#pragma comment(lib, "Ws2_32.lib")

class Client
{
public:

    constexpr static auto MAX_BUFFER_SIZE = 4092;

    enum class Protocol
    {
        TELNET = 23,
        HTTP = 80,
        HTTPS = 443,
    };

    Client() = default;

    ~Client()
    {
        WSACleanup();
    }

    static bool Init()
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
            return false;
        }

        SSL_library_init();

        return true;
    }

    void RefreshAddress()
    {
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        memcpy(&serverAddress.sin_addr.s_addr, hostIp->h_addr_list[0], hostIp->h_length);
    }

    int MakeConnection()
    {
        if (hostIp == nullptr)
        {
            std::cerr << "Host ip incorrect: " << WSAGetLastError() << std::endl;
            return WSAGetLastError();
        }

        client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (client == INVALID_SOCKET)
        {
            std::cerr << "socket failed: " << WSAGetLastError() << std::endl;
            return WSAGetLastError();
        }

        if (serverAddress.sin_addr.S_un.S_addr == 0)
        {
            RefreshAddress();
        }

        if (connect(client, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
        {
            closesocket(client);
            return WSAGetLastError();
        }

        return WSAGetLastError();
    }

    int MakeHTTPSConnection()
    {
        if (MakeConnection())
        {
            return WSAGetLastError();
        }

        ctx = SSL_CTX_new(SSLv23_client_method());
        if (ctx == nullptr) {
            std::cerr << "Error creating SSL context." << std::endl;
            HandleOpenSSLErrors();
            return WSAGetLastError();
        }

        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);

        if (SSL_connect(ssl) != 1) {
            std::cerr << "Error establishing SSL connection." << std::endl;
            SSL_free(ssl);
            closesocket(client);
            return WSAGetLastError();
        }

        return WSAGetLastError();

    }

    void Send(std::string_view message) const
    {
        if (send(client, message.data(), message.length(), 0) == SOCKET_ERROR)
        {
            closesocket(client);
        }


        char buffer[MAX_BUFFER_SIZE];
        int bytesRead;
        while ((bytesRead = recv(client, buffer, MAX_BUFFER_SIZE, 0)) > 0) {
            std::cout.write(buffer, bytesRead);
        }
        if (bytesRead < 0) {
            std::cerr << "Error receiving HTTP response." << std::endl;
        }
        else if (bytesRead == 0)
            std::cerr << "SERVER CLOSE." << std::endl;

        closesocket(client);

    }


    void SendHttps(std::string_view message) const
    {
        if (SSL_write(ssl, message.data(), static_cast<int>(message.length())) != static_cast<int>(message.length())) {
            std::cerr << "Error sending HTTPS request." << std::endl;
            SSL_free(ssl);
            closesocket(client);
            WSACleanup();
        }

        char buffer[MAX_BUFFER_SIZE];
        int bytesRead;
        while ((bytesRead = SSL_read(ssl, buffer, sizeof(buffer))) > 0) {
            std::cout.write(buffer, bytesRead);
        }
        if (bytesRead < 0) {
            std::cerr << "Error receiving HTTPS response." << std::endl;
        }
    }

    void Close()
    {
        closesocket(client);
        serverAddress = {};
    }

    bool SetHostIp(std::string_view hostName) {
        hostIp = nullptr;
        hostIp = gethostbyname(hostName.data());
        if (hostIp == nullptr)
            return false;

        return true;

    };

    int MakeConnectionAny()
    {
        switch (protocol)
        {
        case Protocol::TELNET:
        case Protocol::HTTP:
            return MakeConnection();

        case Protocol::HTTPS:
            return MakeHTTPSConnection();

        default:
            break;
        }
    }


    void SendAny(std::string_view message)
    {
        switch (protocol)
        {
        case Protocol::TELNET:
        case Protocol::HTTP:
            Send(message);
            break;

        case Protocol::HTTPS:
            SendHttps(message);
            break;
       
        default:
            break;
        }
    }

    inline void SetProtocol(Protocol protocol) { port = static_cast<uint16_t>(protocol); this->protocol = protocol; };

private:

    static void HandleOpenSSLErrors() {
        ERR_print_errors_fp(stderr);
        abort();
    }

    SSL_CTX* ctx = nullptr;
    SSL* ssl = nullptr;

    hostent* hostIp = nullptr;

    SOCKET client = INVALID_SOCKET;
    Protocol protocol = Protocol::HTTP;
    uint16_t port = static_cast<uint16_t>(Protocol::HTTP);

    sockaddr_in serverAddress = {};
};



int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    Client::Init();
    auto client = new Client();

    int selectedProtocol = 0;
    int selectedPort = 0;
    std::string address  = "";


    while (true)
    {
        while(selectedProtocol == 0)
        {
            std::cout << "Выберите протокол или порт" << std::endl;
            std::cout << "1. HTTP" << std::endl;
            std::cout << "2. HTTPS" << std::endl;
            std::cout << "3. TELNET" << std::endl;
            std::string temp;
            std::getline(std::cin, temp);
            
            selectedProtocol = std::stoi(temp);

            switch (selectedProtocol)
            {
            case 1:
                client->SetProtocol(Client::Protocol::HTTP);
                break;
            case 2:
                client->SetProtocol(Client::Protocol::HTTPS);
                break;
            case 3:
                client->SetProtocol(Client::Protocol::TELNET);
                break;
            
            default:
                selectedProtocol = 0;
                break;
            }
        }

        while (address == "")
        {
            std::cout << "Введите адрес" << std::endl;
            std::getline(std::cin, address);
            if (!client->SetHostIp(address))
            {
                std::cout << "Неверный адрес. Попробуйте снова." << std::endl;
                address = "";
            }
        }

        while (true)
        {
            std::cout << "Введите запрос или введите exit для выхода" << std::endl;
            std::string str;

            //std::cin.ignore();
            std::getline(std::cin,str);
            
            if (str == "exit")
                return 0;

            str += '\n';
            if (client->MakeConnectionAny() != 0)
            {
                std::cout << "Соедиенение с сервером утеряно" << std::endl;
                break;
            }
            else 
                client->SendAny(str);
        }
        
        
        client->Close();

    }

    return 0;
}
