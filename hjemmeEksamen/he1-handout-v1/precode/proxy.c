/*
 * This is the main program for the proxy, which receives connections for sending and receiving clients
 * both in binary and XML format. Many clients can be connected at the same time. The proxy implements
 * an event loop.
 *
 * *** YOU MUST IMPLEMENT THESE FUNCTIONS ***
 *
 * The parameters and return values of the existing functions must not be changed.
 * You can add function, definition etc. as required.
 */
#include "xmlfile.h"
#include "connection.h"
#include "record.h"
#include "recordToFormat.h"
#include "recordFromFormat.h"

#include <arpa/inet.h>
#include <stddef.h>
#include <sys/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>

/* This struct should contain the information that you want
 * keep for one connected client.
 * Clients are used as a linked list/stack.
 */
struct Client {
    struct Client *next;
    int fd;
    char ID, type;
    char buf[BUFSIZ];
    int bytesRead;
};

struct Client *start;
int numClients = 0;

struct Client* getFirst() {
    return start;
}
void push(struct Client *c) {
    struct Client *tmp = start;
    start = c;
    start->next = tmp;
}
struct Client* pop() {
    if (start) {
        struct Client *tmp = start;
        start = start->next;
        return tmp;
    }
    return NULL;
}
struct Client* findByFD(int fd) {
    if (start->fd == fd) { return start; }
    struct Client *tmp = start;
    while (tmp->fd != fd) {
        tmp = tmp->next;
        if (!tmp) { return NULL; }
    }
    return (tmp->fd == start->fd) ? NULL : tmp;
}
struct Client* findByID(char ID) {
    if (start->ID == ID) { return start; }
    struct Client *tmp = start;
    while (tmp->ID != ID) {
        tmp = tmp->next;
        if (!tmp) { return NULL; }
    }
    return (tmp->ID == start->ID) ? NULL : tmp;
}
void stackRemove(struct Client *c) {
    tcp_close(c->fd); //TODO, Trenger jeg denne?

    if (c->next == NULL) { 
        free(c);
        return;
        }

    struct Client *tmp = c->next;

    if (start == c) { 
        free(start);
        start = tmp;
        return;
        }
    c->fd = tmp->fd;
    c->ID = tmp->ID;
    c->type = tmp->type;
    c->next = tmp->next;
    free(tmp);
}


typedef struct Client Client;


void usage( char* cmd )
{
    fprintf( stderr, "Usage: %s <port>\n"
                     "       This is the proxy server. It takes as imput the port where it accepts connections\n"
                     "       from \"xmlSender\", \"binSender\" and \"anyReceiver\" applications.\n"
                     "       <port> - a 16-bit integer in host byte order identifying the proxy server's port\n"
                     "\n",
                     cmd );
    exit( -1 );
}

/*
 * This function is called when a new connection is noticed on the server
 * socket.
 * The proxy accepts a new connection and creates the relevant data structures.
 *
 * *** The parameters and return values of this functions can be changed. ***
 */
 int handleNewClient( int server_sock ) {
    char buf[sizeof(char)];
    int fd;
    int read;
    fd = tcp_accept(server_sock);
    if (fd < 0) {
        fprintf(stderr, "handleNewClient: %s\n", strerror(errno));
        return -1;
    }
    struct Client *c = malloc(sizeof(struct Client));
    c->bytesRead = 0;
    c->fd = fd;

    read = tcp_read(fd, buf, 1);
    if (read < 0) {
        fprintf(stderr, "handle new client, tcp_read failed\n");
        return -1;
    }
    memcpy(&c->type, buf, 1);
    
    read = tcp_read(fd, buf, 1);
    if (read < 0) {
        fprintf(stderr, "handle new client, tcp_read failed\n");
        return -1;
    }
    memcpy(&c->ID, buf, 1);


    //printf("New client FD:   %d\n", c->fd);
    //printf("New client TYPE: %c\n", c->type);
    //printf("New client ID:   %c\n", c->ID);

    push(c);
    return fd;
}


/*
 * This function is called when a connection is broken by one of the connecting
 * clients. Data structures are clean up and resources that are no longer needed
 * are released.
 *
 * *** The parameters and return values of this functions can be changed. ***
 */
void removeClient( Client* client ) {
    stackRemove(client);
}

/*
 * This function is called when the proxy received enough data from a sending
 * client to create a Record. The 'dest' field of the Record determines the
 * client to which the proxy should send this Record.
 *
 * If no such client is connected to the proxy, the Record is discarded without
 * error. Resources are released as appropriate.
 *
 * If such a client is connected, this functions find the correct socket for
 * sending to that client, and determines if the Record must be converted to
 * XML format or to binary format for sendig to that client.
 *
 * It does then send the converted messages.
 * Finally, this function deletes the Record before returning.
 *
 * *** The parameters and return values of this functions can be changed. ***
 */
void forwardMessage( Record* msg ) {
    printf("BROR skal forwarde IKKE\n");
    char *buf;
    int bufsiz;
    if (!msg) { deleteRecord(msg); }
    if (!msg->dest) { deleteRecord(msg); return; }
    struct Client *dest = findByID(msg->dest);
    
    if (dest->type == 'X') {
        buf = recordToXML(msg, &bufsiz);
        if (buf) { tcp_write_loop(dest->fd, buf, bufsiz); }

    } else if (dest->type == 'B') {
        buf = recordToBinary(msg, &bufsiz);
        if (buf) { tcp_write_loop(dest->fd, buf, bufsiz); }

    }
    free(buf);
    deleteRecord(msg);
    printf("HERI JEG HAR SENDE return to main\n");
    //free(dest);
}

/*
 * This function is called whenever activity is noticed on a connected socket,
 * and that socket is associated with a client. This can be sending client
 * or a receiving client.
 *
 * The calling function finds the Client structure for the socket where acticity
 * has occurred and calls this function.
 *
 * If this function receives data that completes a record, it creates an internal
 * Record data structure on the heap and calls forwardMessage() with this Record.
 *
 * If this function notices that a client has disconnected, it calls removeClient()
 * to release the resources associated with it.
 *
 * *** The parameters and return values of this functions can be changed. ***
 */
int handleClient( Client* client ) {
    if (client == NULL) {
        fprintf(stderr, "handleClient: Client is null\n");
        return -1; }

    int read;

    
    read = tcp_read(client->fd, client->buf, BUFSIZ);
    if (read == 0) { return read; }
    if (read < 0) {
        fprintf(stderr, "handle client, tcp_read failed\n");
        return -1;
    }

    if (client->type == 'X') {
        struct Record *r = XMLtoRecord(client->buf, BUFSIZ, &client->bytesRead);

        printf("Forwarding -> XML\n");
        if (r) { forwardMessage(r); }

    } else if (client->type == 'B') {
        struct Record *r = BinaryToRecord(client->buf, BUFSIZ, &client->bytesRead);

        printf("Forwarding -> BINARY\n");
        if (r) { forwardMessage(r); }
    }
    return read;

}

int main( int argc, char* argv[] ) {
    int port;
    int server_sock;

    if( argc != 2 )
    {
        usage( argv[0] );
    }

    port = atoi( argv[1] );

    server_sock = tcp_create_and_listen( port );
    if( server_sock < 0 ) exit( -1 );
    int max_fd = server_sock; 

    /* add your initialization code */
    fd_set currentSockets, readySockets;


    FD_ZERO(&currentSockets);
    FD_SET(server_sock, &currentSockets);
    
    /*
     * The following part is the event loop of the proxy. It waits for new connections,
     * new data arriving on existing connection, and events that indicate that a client
     * has disconnected.
     *
     * This function uses handleNewClient() when activity is seen on the server socket
     * and handleClient() when activity is seen on the socket of an existing connection.
     *
     * The loops ends when no clients are connected any more.
     */
    int connectedClients = 0;
    do
    {
        readySockets = currentSockets;
        printf("Waiting for connections: \n");
        if (tcp_wait(&readySockets, max_fd + 1) < 0) {
            fprintf(stderr, "Error proxy main: tcp_wait: %s\n", strerror(errno));
        }
        

        for (int fd=0; fd<=max_fd; ++fd) {
            if (FD_ISSET(fd, &readySockets)){
                if (fd == server_sock) {
                    int clientSock;
                    clientSock = handleNewClient(server_sock);
                    printf("New client on sock: %d\n", clientSock);
                    FD_SET(clientSock, &currentSockets);
                    if (clientSock < 0) { continue; }
                    if (clientSock > max_fd) { max_fd = clientSock; }

                    printf("New client TYPE %c\n", getFirst()->type);
                    printf("New client ID %c\n\n\n", getFirst()->ID);

                    connectedClients++;

                } else {
                    printf("Handling client socket: %d\n", fd);

                    int check = handleClient(findByFD(fd));
                    if (!check){
                        connectedClients--;
                        FD_CLR(fd, &currentSockets);                    
                        removeClient(findByFD(fd));
                        printf("Client removed socket: %d\n", fd);
                    }
                }
            }
        }
    }
    while(connectedClients);

    /* add your cleanup code */
    // Clients are removed when they no longer send data, line 321

    tcp_close( server_sock );
    printf("Server socket closed.\nExiting...\n");

    return 0;
}

