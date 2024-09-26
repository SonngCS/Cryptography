#include "client.h"
#include "FileData.h"

int __cdecl main(void){
    FILE *in;
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    char sendbuf[DEFAULT_BUFLEN];
    char recvbuf[DEFAULT_BUFLEN];
    char recvfilname[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        cout << "WSAStartup failed with error: " << iResult << endl;
        return 2;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(DEFAULT_SERVER, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        cout << "getaddrinfo failed with error: " << iResult << endl;
        WSACleanup();
        fclose(in);
        return 1;
    }

    do{
        // Attempt to connect to an address until one succeeds
        for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

            // Create a SOCKET for connecting to server
            ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
                ptr->ai_protocol);
            if (ConnectSocket == INVALID_SOCKET) {
                cout << "socket failed with error: " << WSAGetLastError() << endl;
                WSACleanup();
                fclose(in);
                return 1;
            }

            // Connect to server.
            iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
            if (iResult == SOCKET_ERROR) {
                closesocket(ConnectSocket);
                ConnectSocket = INVALID_SOCKET;
                continue;
            }
            break;
        }

        Sleep(7000);
    }while(ConnectSocket == INVALID_SOCKET);

    freeaddrinfo(result);

    FileData file;
    int option;
    string file_name;
    string opt_char;

    while(true){
        iResult = recv(ConnectSocket, recvfilname, DEFAULT_BUFLEN, 0);
        if(iResult <= 0) break;
        opt_char = recvfilname;
        option = opt_char[0] - '0';
        //cout << "receive buffer " << opt_char << endl;
        //cout << "option: " << option << endl;
        switch(option){
            case 1:
                file.sendDirectories(&iResult, ConnectSocket, sendbuf, recvbuf, (recvfilname[2] - '0'));
                break;
            case 2:
                iResult = recv(ConnectSocket, recvfilname, DEFAULT_BUFLEN, 0);
                if (iResult <= 0) {
                    cout << "Filename not received from server, closing connection" << endl;
                    closesocket(ConnectSocket);
                    WSACleanup();
                    return 1;
                }
                file_name = recvfilname;
                file.send_file(&iResult, ConnectSocket, sendbuf, recvbuf, file_name);
                break;
            case 3:
                break;
            default:
                cout << "Option not recognized" << endl;
                break;
        }
    }

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();
    //fclose(in);

    return 0;
}