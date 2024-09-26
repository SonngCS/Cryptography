#include "FileData.h"

FileData::FileData(){

}
FileData::FileData(string file_name, string new_file_path){
    this->original_file_path = file_name;
    this->destination_file_path = new_file_path;
    this->setFileName();
}

void FileData::setOriginalPath(string path){
    this->original_file_path = path;
}

void FileData::setNewPath(string new_path){
    this->destination_file_path = new_path + this->file_name;
}

void FileData::setFileName(){
    this->file_name = getSubstring('/');
}

void FileData::setUserName(){
    char username[UNLEN+1];
    DWORD username_len = UNLEN+1;
    GetUserName((TCHAR*)username, &username_len);
    this->user_name = username;
}

void FileData::receive_file(int *iResult, int *iSendResult, SOCKET ClientSocket, char *recvbuf){

    FILE *out;
    char file_name[DEFAULT_BUFLEN];
    strcpy(file_name, this->getOriginalPath().c_str());
    file_name[this->getOriginalPath().length() + 1] = '\0';
    //cout << file_name << endl;
    if ((out = fopen(this->getNewPath().c_str(), "wb")) == NULL){
        cout << "Open macro failed to open file" << endl;
        return;
    }
    //delete[] file_name;
    *iSendResult = send(ClientSocket, (char *) file_name, DEFAULT_BUFLEN, 0);
    if (*iSendResult == SOCKET_ERROR) {
        cout << "send failed with error: " << WSAGetLastError() << endl;
        closesocket(ClientSocket);
        WSACleanup();
        fclose(out);
        return;
    }
    string finalizer;

    // Receive until the peer shuts down the connection
    bool isTXT = this->isItTXT();

    while(true){

        *iResult = recv(ClientSocket, recvbuf, DEFAULT_BUFLEN, 0);
        finalizer = recvbuf;
        if(finalizer == "Finished") break;

        if (*iResult > 0) {
            for(int i = 0; i < DEFAULT_BUFLEN; i++){
                fputc(recvbuf[i], out);

                if(isTXT && recvbuf[i] == '\0'){
                    fputc('\0', out);
                    break;
                }
            }

            // Echo the buffer back to the sender
            *iSendResult = send(ClientSocket, recvbuf, DEFAULT_BUFLEN, 0);
            if (*iSendResult == SOCKET_ERROR) {
                cout << "send failed with error: " << WSAGetLastError() << endl;
                closesocket(ClientSocket);
                WSACleanup();
                fclose(out);
                return;
            }
        }
        else if (*iResult == 0)
            cout << "Closing conection with client" << endl;
        else  {
            cout << "recv failed with error: " << WSAGetLastError() << endl;
            closesocket(ClientSocket);
            WSACleanup();
            fclose(out);
            return;
        }
    }
    fclose(out);
}

void FileData::send_file(int *iResult, SOCKET ConnectSocket, char * sendbuf, char * recvbuf, string file_name_){

    FILE * in;

    this->setOriginalPath(file_name_);
    this->setFileName();
    cout << this->getOriginalPath() << endl;

    if ((in = fopen(this->getOriginalPath().c_str(), "rb")) == NULL){
        cout << "Open macro failed to open file" << endl;
        return;
    }

    int len;
    int c = getc(in);
    bool isTXT = this->isItTXT();

    while(c != EOF){
        len = 0;
        while((len < DEFAULT_BUFLEN) && c != EOF){

            sendbuf[len++] = c;
            c = getc(in);
        }

        if(isTXT && c == EOF){
            sendbuf[len] = '\0';
        }

        *iResult = send(ConnectSocket, sendbuf, DEFAULT_BUFLEN, 0);
        if (*iResult == SOCKET_ERROR) {
            cout << "send failed with error: " << WSAGetLastError() << endl;
            closesocket(ConnectSocket);
            WSACleanup();
            fclose(in);
            return;
        }

        *iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
        if (*iResult > 0) {
            continue;
        }else{
            cout << "Echo not received from server, closing connection" << endl;
            closesocket(ConnectSocket);
            WSACleanup();
            fclose(in);
            return;
        }
    }
    fclose(in);
    *iResult = send(ConnectSocket, "Finished", DEFAULT_BUFLEN, 0);
}

void FileData::sendDirectories(int* iResult, SOCKET ConnectSocket, char* sendbuf, char* recvbuf, const int opt){
    string user_path;
    this->setUserName();
    switch(opt){
    case 1:
        user_path = "C:\\Users\\" + this->user_name + "\\Downloads";
        break;
    case 2:
        user_path = "C:\\Users\\" + this->user_name + "\\Documents";
        break;
    case 3:
        user_path = "C:\\Users\\" + this->user_name + "\\Desktop";
        break;
    case 4:
        user_path = "C:\\Users\\" + this->user_name + "\\Pictures";
        break;
    case 5:
        *iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
        if (*iResult == SOCKET_ERROR) {
            cout << "send failed with error: " << WSAGetLastError() << endl;
            closesocket(ConnectSocket);
            WSACleanup();
            return;
        }
        user_path = recvbuf;
        break;
    default:
        cout << "Path not recognized" << endl;
        break;
    }

    try
    {
        for(const auto & entry : fs::directory_iterator(user_path, fs::directory_options::skip_permission_denied)){

            strcpy(sendbuf, entry.path().generic_string().c_str());

            *iResult = send(ConnectSocket, sendbuf, DEFAULT_BUFLEN, 0);
            if (*iResult == SOCKET_ERROR) {
                cout << "send failed with error: " << WSAGetLastError() << endl;
                closesocket(ConnectSocket);
                WSACleanup();
                return;
            }

            *iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
            if (*iResult > 0) {
                continue;
            }else{
                cout << "Echo not received from server, closing connection" << endl;
                closesocket(ConnectSocket);
                WSACleanup();
                return;
            }
        }
    }catch(const std::exception& e){
        cout << "Error with given path" << endl;
    }

    *iResult = send(ConnectSocket, "END", DEFAULT_BUFLEN, 0);
    if (*iResult == SOCKET_ERROR) {
        cout << "send failed with error: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }

    *iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
    if (*iResult <= 0) {
        cout << "Echo not received from server, closing connection" << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }
}

void FileData::receiveDirectories(int* iResult, SOCKET ClientSocket, char* sendbuf, char* recvbuf, const int opt){
    string user_path;
    string flag = "BEGIN";
    char user_path_[1000];
    if(opt == 5){
        cout << "Give the custom directory name: ";
        //cin.ignore();
        getline(cin, user_path);

        strcpy(user_path_, user_path.c_str());
        user_path_[user_path.length() + 1] = '\0';
        *iResult = send(ClientSocket, (char *) user_path_, DEFAULT_BUFLEN, 0);
    }

    while(flag != "END"){

        *iResult = recv(ClientSocket, recvbuf, DEFAULT_BUFLEN, 0);
        if (*iResult > 0) {
            flag = recvbuf;
            if(flag != "END"){
                cout << flag << endl;
            }
            //continue;
        }else{
            cout << "Echo not received from server, closing connection" << endl;
            closesocket(ClientSocket);
            WSACleanup();
            return;
        }

        *iResult = send(ClientSocket, sendbuf, DEFAULT_BUFLEN, 0);
        if (*iResult == SOCKET_ERROR) {
            cout << "send failed with error: " << WSAGetLastError() << endl;
            closesocket(ClientSocket);
            WSACleanup();
            return;
        }
    }
}

string FileData::getSubstring(const char c){
    size_t position = this->original_file_path.rfind(c);

    if(position != std::string::npos){
        return this->original_file_path.substr(position);
    }

    throw std::runtime_error("File does not contain the chosen character\n");
}

string FileData::getOriginalPath(){
    return this->original_file_path;
}
string FileData::getNewPath(){
    return this->destination_file_path;
}

string FileData::getFileName(){
    return this->file_name;
}

string FileData::getUserName(){
    return this->user_name;
}

bool FileData::isItTXT(){
    return (this->getSubstring('.') == ".txt" || this->getSubstring('.') == ".TXT");
}
