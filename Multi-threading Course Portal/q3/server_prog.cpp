#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>

/////////////////////////////
#include <iostream>
#include <assert.h>
#include <tuple>
using namespace std;
#define Green_color "\033[0;32m"
#define Reset_color "\x1b[0m"
#define debug(x) cout << #x << " : " << x << endl
typedef long long LL;
const LL buff_sz = 1048576;
pair<string, LL> reading_string(const LL &fd, LL bytes)
{
    pair<string,LL> return_var;
    return_var.first.resize(bytes);
    return_var.second = read(fd, &return_var.first[0], bytes - 1);
    return_var.first[return_var.second] = 0;
    debug(return_var.second);
    if (return_var.second <= 0)
        cerr << "Failed to read data from socket." << endl;
    return_var.first.resize(return_var.second);
    return return_var;
}
LL checking(string &arr)
{
    LL flg = 0,i=0;
    for(i=0;i<arr.size();++i)
    {
        if(arr[i]<'0')
            flg = 1;
        if(arr[i]>'9')
            flg = 1;
    }
    return flg;
}
LL sending_string(LL fd, const string &s)
{
    LL return_var = write(fd, s.c_str(), s.length());
    if (return_var < 0)
        cerr << "Failed to SEND DATA via socket.\n";
    return return_var;
}
pair < string,LL> map_val[105]; 
queue <LL> Q;
pthread_mutex_t qu_handler = PTHREAD_MUTEX_INITIALIZER;
pthread_t worker_threads[105];
sem_t semaphore;
pthread_mutex_t handling[105];
void* handle_connection(void*)
{
    vector <string> v;
    LL received_num, sent_num,ret_val;
    while(1)
    {
        sem_wait(&semaphore); 
        ret_val = 1;
        pthread_mutex_lock(&qu_handler);
        LL client_socket_fd;
        if(Q.empty()==1)
        { 
            pthread_mutex_unlock(&qu_handler);
            continue;
        }
        client_socket_fd = Q.front();
        Q.pop();
        pthread_mutex_unlock(&qu_handler);
        string cmd;
        tie(cmd, received_num) = reading_string(client_socket_fd, buff_sz);
        ret_val = received_num;
        string str1 = "";
        if (ret_val <= 0)
        {
            close(client_socket_fd);
            cout << "Server could not read msg sent from client" << endl;
            return NULL;
        }
        LL i = 0,j = 0;
        for(i=0;i<cmd.length();++i) 
        {
            if(cmd[i]==' ')
            {
                if(str1.size()!=0)
                    v.push_back(str1);
                str1="";
                continue;
            }
            str1=str1+cmd[i];
        }
        string send_back_mg="";
        if(str1.size()!=0)
            v.push_back(str1);
        string thread_name = to_string(pthread_self());
        thread_name+=":";
        LL flg1=0;
        send_back_mg+=thread_name;
        if(v[0]=="insert") 
        {
            if(v.size()!=3 || (flg1!=1 && (checking(v[1]))))
                flg1 = 1;
            if(flg1)
                 send_back_mg=send_back_mg+"Parameter is Incorrect";
            else if(!flg1)
            {
                pthread_mutex_lock(&handling[atoi(v[1].c_str())]);
                if(!map_val[atoi(v[1].c_str())].second)
                {
                    map_val[atoi(v[1].c_str())].first = v[2];
                    send_back_mg=send_back_mg+"Insertion Sucessful";
                    map_val[atoi(v[1].c_str())].second = 1;
                }
                else
                    send_back_mg=send_back_mg+"Key already exists";
                pthread_mutex_unlock(&handling[atoi(v[1].c_str())]);
            }
        }
        else if(v[0]=="delete")
        {
            if(v.size()!=2 || (flg1!=1 && (checking(v[1]))))
                flg1 = 1;
            if(flg1)
                 send_back_mg=send_back_mg+"Parameter is Incorrect";
            else if(!flg1)
            {
                pthread_mutex_lock(&handling[atoi(v[1].c_str())]);
                if(map_val[atoi(v[1].c_str())].second==1)
                {
                    map_val[atoi(v[1].c_str())].first = "";
                    send_back_mg=send_back_mg+"Deletion successful";
                    map_val[atoi(v[1].c_str())].second = 0;
                }
                else
                    send_back_mg=send_back_mg+"No such key exists";
                pthread_mutex_unlock(&handling[atoi(v[1].c_str())]);
            }
        }
        else if(v[0]=="update")
        {
            if(v.size()!=3 || (flg1!=1 && (checking(v[1]))))
                flg1 = 1;
            if(flg1)
                send_back_mg=send_back_mg+"Parameter is Incorrect";
            else if(!flg1)
            {
                pthread_mutex_lock(&handling[atoi(v[1].c_str())]);
                if(map_val[atoi(v[1].c_str())].second==1)
                {
                    map_val[atoi(v[1].c_str())].first = v[2];
                    send_back_mg=send_back_mg+v[2];
                }
                else
                    send_back_mg=send_back_mg+"Key not exist";
                pthread_mutex_unlock(&handling[atoi(v[1].c_str())]);
            }
        }
        else if(v[0]=="concat")
        {
            if((v.size()!=3) || (flg1!=1 && (checking(v[1]))) || (flg1!=1 && (checking(v[2]))))
                flg1 = 1;
            if(flg1)
                 send_back_mg=send_back_mg+"Parameter is Incorrect";
            else if(!flg1)
            {
                LL flag = 0;
                pthread_mutex_lock(&handling[min(atoi(v[1].c_str()),atoi(v[2].c_str()))]);
                pthread_mutex_lock(&handling[max(atoi(v[1].c_str()),atoi(v[2].c_str()))]);
                if(!map_val[atoi(v[1].c_str())].second || (!map_val[atoi(v[2].c_str())].second))
                    flag=1;
                if(flag)
                    send_back_mg=send_back_mg+"Concat failed as at least one of the keys does not exist";
                else
                {
                    string str1,str2;
                    str1 = map_val[atoi(v[1].c_str())].first;
                    str2 = map_val[atoi(v[2].c_str())].first;
                    map_val[atoi(v[1].c_str())].first = str1+str2;
                    map_val[atoi(v[2].c_str())].first = str2+str1;
                    send_back_mg+=map_val[atoi(v[2].c_str())].first;
                }
                pthread_mutex_unlock(&handling[max(atoi(v[1].c_str()),atoi(v[2].c_str()))]);
                pthread_mutex_unlock(&handling[min(atoi(v[1].c_str()),atoi(v[2].c_str()))]);
            }
            
        }
        else if(v[0]=="fetch")
        {
            if(v.size()!=2 || (flg1!=1 && (checking(v[1]))))
                flg1 = 1;
            if(flg1)
                send_back_mg=send_back_mg+"Parameter is Incorrect";
            else if(!flg1)
            {
                pthread_mutex_lock(&handling[atoi(v[1].c_str())]);
                if(map_val[atoi(v[1].c_str())].second==1)
                    send_back_mg=send_back_mg+map_val[atoi(v[1].c_str())].first;
                else
                    send_back_mg=send_back_mg+"Key not exist";
                pthread_mutex_unlock(&handling[atoi(v[1].c_str())]);
            }
        }
        else
            send_back_mg=send_back_mg+"Request is Invalid";
        cout << "Client sent : " << cmd << endl;        
        v.clear();
        LL sent_to_client = sending_string(client_socket_fd, send_back_mg);
        sleep(2);
        close(client_socket_fd);
        if (sent_to_client == -1)
            perror("Error while writing to client. Seems socket has been closed");
    }
    return NULL;
}
int main(int argc, char *argv[])
{
    cout <<"M(Number of requests throughout the simulation) = "<< atoi(argv[1]) << endl;
    LL client_socket_fd, port_number;
    socklen_t clilen;
    struct sockaddr_in serv_addr_obj, client_addr_obj;
    sem_init(&semaphore,0,0);
    LL i = 0, j = 0;
    for(i = 0; i < 101; ++i)
    {
        handling[i] = PTHREAD_MUTEX_INITIALIZER;
        map_val[i] = {"",0};
    }
    for(i = 0; i < atoi(argv[1]); ++i)
        pthread_create(&worker_threads[i], NULL, handle_connection,NULL);
    sleep(5);
    LL wel_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (wel_socket_fd < 0)
    {
        perror("ERROR creating welcoming socket");
        return 0;
    }
    serv_addr_obj.sin_family = AF_INET;
    serv_addr_obj.sin_addr.s_addr = INADDR_ANY;
    bzero((char *)&serv_addr_obj, sizeof(serv_addr_obj));
    port_number = 8002;
    serv_addr_obj.sin_port = htons(port_number); 
    if (bind(wel_socket_fd, (struct sockaddr *)&serv_addr_obj, sizeof(serv_addr_obj)) < 0)
    {
        perror("Error on bind on welcome socket: ");
        return 0;
    }
    cout << "Server has started listening on the LISTEN PORT" << endl;
    listen(wel_socket_fd, 20);
    clilen = sizeof(client_addr_obj);
    for(;;)
    {
        cout << "Waiting for a new client to request for a connection" << endl;
        client_socket_fd = accept(wel_socket_fd, (struct sockaddr *)&client_addr_obj, &clilen);
        if (client_socket_fd < 0)
        {
            perror("ERROR while accept() system call occurred in SERVER");
            return 0;
        }
        pthread_mutex_lock(&qu_handler);
        Q.push(client_socket_fd);
        printf(Green_color "New client connected from port number %d and IP %s \n" Reset_color, ntohs(client_addr_obj.sin_port), inet_ntoa(client_addr_obj.sin_addr));
        sem_post(&semaphore);
        pthread_mutex_unlock(&qu_handler);
    }
    close(wel_socket_fd);
    return 0;
}
