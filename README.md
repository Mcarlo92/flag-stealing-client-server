# Client server Game of capture the Flag -

The application consists of two source files:

1) server.c
2) client.c


The server will perform all the typical functions of a server:
- Manages clients that connect to it to request services from it
- Manages communication tools

In particular, the services provided by the server to the client are the following:
- Login to the platform
- Registration to the platform
- Movement in one of the 4 directions (North, South, East, West)
- Logout
- Display of connected users
- Visualization of the obstacles encountered by all users participating in the game
- Location of allied soldiers with relative life remaining
- Print of the game map

The client will allow the user to use the services offered by the server.

Compilation of sources

For the server we will run the following command in the shell:

gcc server.c -o server -pthread

For the client we will run the following command in the shell:

gcc client.c -o client -pthread

After compiling we run the server always typing in the shell

./server port
    
The port is an alphanumeric value, on which the server listens.
We remind users that not all ports are the same, in particular:
- Ports between 0 and 1023 are reserved
- Ports between 1024 and 49151 contain registered services
- Ports between 49152 and 65535 are free ports
  
If the execution is successful, the following screen will be shown:
    ![image](https://user-images.githubusercontent.com/33984528/149159952-33d2f036-6cdd-4d34-ba7a-43a05f34ba1e.png)
    
Then we launch the client in a new terminal
./client port address
In <port> we insert the value of the port on which the server is listening
That is, that choice in the execution of the server.
In <address> we enter the address of our server, however being the local server,
we can omit this parameter, which will be automatically set to 127.0.0.1
    ![image](https://user-images.githubusercontent.com/33984528/149159838-155fda6d-6496-48a2-b154-96b741240935.png)
