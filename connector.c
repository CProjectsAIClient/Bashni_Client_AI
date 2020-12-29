#include <sys/epoll.h>

#include "connector.h"
#include "performConnection.h"

#define PIPE_BUF 128

// epoll(): https://suchprogramming.com/epoll-in-3-easy-steps/
void startConnector(int fd_sock, int fd_pipe) {
    //Create Epoll instance
    int epoll_fd = epoll_create1(0);
    //Create Pipe read buffer
    char read_buffer[PIPE_BUF];

    if(epoll_fd == -1) {
        perror("Failed to create epoll file descriptor\n");
        return 1;
    }
    //Add socket File descriptor to epoll for listening
    registerFd(epoll_fd, fd_sock);
    //Add pipe File descriptor to epoll for listening
    registerFd(epoll_fd, fd_pipe);

    //Listening for incoming data
    struct epoll_event event = waitForInput(epoll_fd);

    //Checking from where data came
    if (event.data.fd == fd_sock) {
        //Data came from socket (GameServer)

        //myread
    } else if (event.data.fd == fd_pipe) {
        //Data came from pipe (Thinker)

        read(fd_pipe, read_buffer, PIPE_BUF);
        //mywrite to server
    }
    
    close(epoll_fd);
}

void getPositions(game game1,struct player* enemies_list){

}

void sendPostitions(){
    
}

void registerFd(int epoll_fd, int fd) {
    //Create Epoll event for listening to incoming data on file descriptor fd
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;

    //Add event to epoll instance
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event)) {
        perror("Failed to add file descriptor to epoll\n");
        close(epoll_fd);
        return 1;
    }
}

struct epoll_event waitForInput(int epoll_fd) {
    struct epoll_event events[1];
    // Wait for incoming data (1 event) on all registered file descriptors with a 20 seconds timeout
    int event_count = epoll_wait(epoll_fd, events, 1, 20000);

    //Check if event count is greater than 0. Otherwise the timout was exceeded which means an error
    if (event_count > 0) {
        return events[0];
    } else {
        perror("Timeout of 20 seconds from epoll reached");
        exit(-3);
    }
}