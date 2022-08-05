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
typedef long long LL;
#define debug(x) cout << #x << " : " << x << endl
const LL buff_sz = 1048576;
pair<string, LL> read_string_from_socket(LL fd, LL bytes)
{
    pair<string,LL> return_var;
    return_var.first.resize(bytes);
    return_var.second = read(fd, &return_var.first[0], bytes - 1);
    return_var.first[return_var.second] = 0;
    if (return_var.second <= 0)
    {
        cerr << "Failed to read data from socket. Seems server has closed socket\n";
        exit(-1);
    }
    return_var.first.resize(return_var.second);
    return return_var;
}
LL send_string_on_socket(LL fd, const string &s)
{
    LL return_var = write(fd, s.c_str(), s.length());
    if (return_var < 0)
    {
        cerr << "Failed to SEND DATA on socket.\n";
        exit(-1);
    }
    return return_var;
}
LL get_socket_fd(struct sockaddr_in *ptr)
{
    struct sockaddr_in server = *ptr;
    LL socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        perror("Error in socket creation for CLIENT");
        exit(-1);
    }
    memset(&server, 0, sizeof(server)); 
    server.sin_port = htons(8002); 
    server.sin_family = AF_INET;
    if (connect(socket_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Problem in connecting to the server");
        exit(-1);
    }
    return socket_fd;
}
vector < pair <LL,string> > v;
pthread_mutex_t mutex_print = PTHREAD_MUTEX_INITIALIZER;
void *begin_process(void * arg)
{
    struct sockaddr_in server;
    LL socket_fd = get_socket_fd(&server);
    LL val = *(LL*)arg;
    string str;
    str = v[val].second;
    sleep(v[val].first);    
    send_string_on_socket(socket_fd, str);
    string to_send;
    LL num_bytes_read;
    string output_msg;
    send_string_on_socket(socket_fd, to_send);
    tie(output_msg, num_bytes_read) = read_string_from_socket(socket_fd, buff_sz);
    pthread_mutex_lock(&mutex_print);
    cout<<val<<":"<< output_msg << endl;
    pthread_mutex_unlock(&mutex_print);
    return NULL;
}
int main(int argc, char *argv[])
{
    char arr[10000];
    pthread_t thrd[10000];
    LL tot_num_user_comnd;
    cin >> tot_num_user_comnd;
    LL value;
    for(LL i = 0; i < tot_num_user_comnd; ++i)
    {
        cin >> value;
        scanf("%[^\n]",arr);
        v.push_back({value,arr});
    }
    for(LL i=0;i<tot_num_user_comnd;i++)
    {
   	    LL* a = (LL*)malloc(sizeof(LL));
   	    *a = i;
        pthread_create(&thrd[i],NULL,&begin_process,a);
    }
    for(LL i = 0; i < tot_num_user_comnd; ++i)
        pthread_join(thrd[i],NULL);
    return 0;
}
