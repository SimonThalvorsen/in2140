/*
 * This is a file that implements the operation on TCP sockets that are used by
 * all of the programs used in this assignment.
 *
 * *** YOU MUST IMPLEMENT THESE FUNCTIONS ***
 *
 * The parameters and return values of the existing functions must not be changed.
 * You can add function, definition etc. as required.
 */
#include "connection.h"

void check_error(int val, char* msg) {
  if (val < 0) {
    fprintf(stderr, "%s failed. Errno-value: (%d)\n", msg, errno);
    fprintf(stderr, "%s : %s\n", msg, (char*) strerror(errno));
    return;
    }
  return;
}

int tcp_connect( char* hostname, int port ) {
    struct sockaddr_in server_addr;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        fprintf(stderr, "Error creating socket\n");
        return -1;
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(hostname);

    if (connect(sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to server\n");
        close(sock);
        return -1;
    }

    return sock;
}

int tcp_read( int sock, char* buffer, int n ) {
    int bytes = read(sock, buffer, n);
    if (bytes == 0) {
        fprintf(stderr, "Error reading socket: EOF");
        return bytes;
    } else if (bytes < 0) {
        fprintf(stderr, "Error reading from socket: %s\n", strerror(errno));
        return bytes;
    }
    return bytes;
}

int tcp_write( int sock, char* buffer, int bytes ) {
    int written_bytes = write(sock, buffer, bytes);
    if (write < 0) {
        fprintf(stderr, "Error writing to socket: %s\n", strerror(errno));
        return written_bytes;
    }
    return written_bytes;
}

int tcp_write_loop( int sock, char* buffer, int bytes ) {
    int written_bytes = 0;
    while (written_bytes < bytes) {
        int write = tcp_write(sock, buffer, bytes-written_bytes);
        if (write < 0) {
            return write;
        }
        written_bytes += write;
    }
    return written_bytes;
}

void tcp_close( int sock ) {
  close(sock);
}

int tcp_create_and_listen( int port ) {
    return 0;
}

int tcp_accept( int server_sock ) {
    int newSock;
    newSock = accept(server_sock, NULL, NULL);
    if (newSock < 0) {
        fprintf(stderr, "Error accepting socket: %s\n", strerror(errno));
    }
    return newSock;
}

int tcp_wait( fd_set* waiting_set, int wait_end ) {
    /* TO BE IMPLEMENTED */
    return 0;
}

int tcp_wait_timeout( fd_set* waiting_set, int wait_end, int seconds ) {
    /* TO BE IMPLEMENTED */
    return 0;
}

