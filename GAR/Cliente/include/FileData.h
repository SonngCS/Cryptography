#ifndef FILEDATA_H
#define FILEDATA_H

#include "client.h"
#include <string>

using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::getline;

class FileData{

private:
    string original_file_path;
    string destination_file_path;
    string file_name;
    string user_name;
public:
    FileData();
    FileData(string, string);

    void setOriginalPath(string);
    void setNewPath(string);
    void setFileName();
    void setUserName();
    void receive_file(int *, int *, SOCKET, char *);
    void send_file(int *, SOCKET, char *, char *, string);
    void sendDirectories(int *, SOCKET, char *, char *, const int);
    void receiveDirectories(int *, SOCKET, char *, char *, const int);
    string getSubstring(const char);
    string getOriginalPath();
    string getNewPath();
    string getFileName();
    string getUserName();
    bool isItTXT();

};

#endif
