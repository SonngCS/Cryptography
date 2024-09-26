#include "server.h"
#include "FileData.h"

namespace fs = std::filesystem;

int __cdecl main(void) 
{

    FILE *out;
    WSADATA wsaData;
    int iResult;

    string new_path_files = "D:/Codigos/C++/GAR/CLION/received_files/";

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        cout << "WSAStartup failed with error: " << iResult << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        cout << "getaddrinfo failed with error: " << iResult << endl;
        WSACleanup();
        fclose(out);
        return 1;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        cout << "socket failed with error: " << WSAGetLastError() << endl;
        freeaddrinfo(result);
        WSACleanup();
        fclose(out);
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        cout << "bind failed with error: " << WSAGetLastError() << endl;
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        fclose(out);
        return 1;
    }

    freeaddrinfo(result);
    
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        cout << "listen failed with error: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        WSACleanup();
        fclose(out);
        return 1;
    }

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        cout << "accept failed with error: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        WSACleanup();
        fclose(out);
        return 1;
    }

    // No longer need server socket
    closesocket(ListenSocket);

    cout << "GGGGGGGGG    AAAAAAA    RRRRRRRRR" << endl;
    cout << "G          AA      AA  RR      RR" << endl;
    cout << "G         AA        AA RR       RR" << endl;
    cout << "G   GGGG  AAAAAAAAAAAA RRRRRRRRR" << endl;
    cout << "G      GG AA        AA RR       RR" << endl;
    cout << "G       G AA        AA RR        RR" << endl;
    cout << "GGGGGGGGG AA        AA RR         RR" << endl;
    cout << "                                      V. 0.5" << endl;
    cout << "Gerenciador de Arquivos Remotos" << endl;

    int opt = -1;
    string opt2;
    string opt2_;
    FileData file;
    string file_path;
    string username_string;
    string user_path;
    char username[UNLEN+1];
    DWORD username_len = UNLEN+1;
    GetUserName((TCHAR*)username, &username_len);
    username_string = username;

    while(opt != 3){
        cout << "\n\nOptions:\n1. Scan directories from client machine\n2. Receive a file\n3. Close connection" << endl;
        cin >> opt;

        switch(opt)
        {
        case 1:
            cout << "Which directory do you want?\n1. Downlaods\n2. Documents\n3. Desktop\n4. Pictures\n5. Custom directory" << endl;
            cin.ignore();
            getline(cin, opt2_);
            opt2 = "1;" + opt2_;
            //cout << "aaaa " << opt2 << endl;
            iResult = send(ClientSocket, (char *) opt2.c_str(), DEFAULT_BUFLEN, 0);
            if (iResult == SOCKET_ERROR) {
                cout << "send failed with error: " << WSAGetLastError() << endl;
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }

            file.receiveDirectories(&iResult, ClientSocket, recvbuf, recvbuf, opt2[2] - '0');

            break;
        case 2:
            cout << "Enter file name and path: " << endl;
            cin.ignore();
            getline(cin, file_path);
            file.setOriginalPath(file_path);
            file.setFileName();
            file.setNewPath(new_path_files);

            cout << "File at " << file.getOriginalPath() << " will be saved at " << file.getNewPath() << endl;

            opt2 = "2";
            iResult = send(ClientSocket, (char *) opt2.c_str(), DEFAULT_BUFLEN, 0);
            if (iResult == SOCKET_ERROR) {
                cout << "send failed with error: " << WSAGetLastError() << endl;
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }

            file.receive_file(&iResult, &iSendResult, ClientSocket, recvbuf);
            break;
        case 3:
            cout << "Closing connection with client." << endl;
            break;
        default:
            cout << "Option not recognized" << endl;
        }
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();
    return 0;
}