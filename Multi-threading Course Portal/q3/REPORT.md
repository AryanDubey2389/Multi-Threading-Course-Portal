## How to run?
 - Initially use g++ -o server server_prog.cpp -lpthread for compiling server.
 - And then use ./server `<No of worker threads>` to start the server. 
 - Then use Client : g++ -o client client_sim.cpp -lpthread for compiling client.
 - And then use ./client and give input as given to run.

# Working of the code:
### Client(client_sim.cpp):
* Initially I created a vector of pairs in which i store input string and sleep time. Then I initialized threads and then in thread function in which the input is basically the index is called and then it sends the given message to the server. Thus multiple requests are made.
* Then the corresponding result is sent back to the client.
* I also applied mutex locks to avoid cluttering of the output.

### Server(server_prog.cpp):
* Initially while worker threads are being initialized server is put into sleep. Then the threads were initialized and put into wait on a semaphore to avoid busy wait condition. I also declared a queue that stores file descripters of different connections between client and server.
* And then i made an infinite loop where each thread function waits on the mutex,now as soon as it get signalled it pops one of the file descriptors from the queue and extracts the given string from it. 
* After performing given operations a string is sent back to the client which describes whether it is sucessful or not. Then after threads perform the task as mentioned i put into a sleep for 2 seconds.