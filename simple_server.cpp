/* run using ./server <port> */
#include "http_server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>

#include <pthread.h>


#include <vector>

#include <sys/stat.h>

#include <fstream>
#include <sstream>

vector<string> split(string &s, char delim) {
    vector<string> elems;

    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        if (!item.empty())
            elems.push_back(item);
    }

    return elems;
}

HTTP_Request::HTTP_Request(string request) {
    vector<string> lines = split(request, '\n');
    vector<string> first_line = split(lines[0], ' ');

    this->HTTP_version = "1.0"; // We'll be using 1.0 irrespective of the request

    this->method = first_line[0];
    this->url = first_line[1];

    if (this->method != "GET") {
        cerr << "Method '" << this->method << "' not supported" << endl;
        exit(1);
    }
}

HTTP_Response *handle_request(string req) {
    HTTP_Request *request = new HTTP_Request(req);

    HTTP_Response *response = new HTTP_Response();

    string url = string("html_files") + request->url;

    response->HTTP_version = "1.0";

    struct stat sb;
    if (stat(url.c_str(), &sb) == 0)
    {
        response->status_code = "200";
        response->status_text = "OK";
        response->content_type = "text/html";

        string body;

        if (S_ISDIR(sb.st_mode)) {
            ifstream myfile;
            myfile.open("html_files/" + request->url + "/index.html");
            string mystring = "";
            if(myfile.is_open()) {
                while(myfile.good()) {
                    string temp;
                    myfile >> temp;
                    mystring += temp + " ";
                }
            }
            response->body = mystring;
        }
        else {
            ifstream myfile;
            myfile.open("html_files/" + request->url );
            string mystring = "";
            if(myfile.is_open()) {
                while(myfile.good()) {
                    string temp;
                    myfile >> temp;
                    mystring += temp + " ";
                }
            }
            response->body = mystring;
        }
    }

    else {
        response->status_code = "404";
        response->body = "file not found";
    }

    delete request;

    return response;
}

string HTTP_Response::get_string() {

    string str = string("HTTP/1.1 ") + this->status_code + string(this->status_code == "200" ? " OK" : " FORBIDDEN") + "\n" +
                 string("Content-Type:") + this->content_type + "\n" +
                 string("Connection:close\n\n") +
                 this->body;
    return str;
}

void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    /* create socket */

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    /* fill in port number to listen on. IP address can be anything (INADDR_ANY)
     */

    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    /* bind socket to this port number on this machine */

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    /* listen for incoming connection requests */


    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    /* accept a new request, create a newsockfd */

    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");

    /* read message from client */

    bzero(buffer, 256);
    n = read(newsockfd, buffer, 255);
    if (n < 0)
        error("ERROR reading from socket");

    string req = "";
    for(int i = 0 ; i < 256 ; i++) {
        req = req + buffer[i];
    }

    /* send reply to client */

    HTTP_Response *res = handle_request(req);
    char res_arr[res->body.length()];
    cout << res->get_string() << endl;
    strcpy(res_arr, res->get_string().c_str());
    n = write(newsockfd, res_arr, res->get_string().length());
    if (n < 0)
        error("ERROR writing to socket");


    return 0;
}
