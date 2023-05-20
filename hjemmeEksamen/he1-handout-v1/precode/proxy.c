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

/* This struct should contain the information that you want
 * keep for one connected client.
 * Clients are used as a linked list/stack.
 */
struct Client {
    struct Client *next;
    int fd;
    char ID, type;
};

struct Client *start = NULL;
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
    if (c->next == NULL) { 
        free(c);
        return;
        }

    struct Client *tmp = c->next;

    if (start == c) { 
        free(start);
        start = tmp;
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
    char buffer[sizeof(char)];
    int fd;
    int read;
    fd = tcp_accept(server_sock);
    printf("HASDASD sicksock 3!=%d\n", fd);
    if (fd < 0) {
        fprintf(stderr, "handleNewClient: %s\n", strerror(errno));
        return -1;
    }
    struct Client *c;

    c->fd = fd;

    read = tcp_read(fd, buffer, 1);
    memcpy(&c->type, buffer, 1);
    
    read = tcp_read(fd, buffer, 1);
    memcpy(&c->ID, buffer, 1);

    push(c);
    printf("HASDASD sicksosdsadasdasdck 3!=%d\n", fd);
    printf("clientes socke sicksock 3!=%d\n", c->fd);
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
    char *buffer;
    int *bufsiz;
    if (!msg) { deleteRecord(msg); }
    struct Client *dest = findByID(msg->dest);
    
    if (dest->type == 'X') {
        buffer = recordToXML(msg, bufsiz);
        if (buffer) {
        tcp_write(dest->fd, buffer, *bufsiz);
        }

        clearRecord(msg);
        free(msg);
    } else if (dest->type == 'B') {
        buffer = recordToBinary(msg, bufsiz);
        if (buffer) {
        tcp_write(dest->fd, buffer, *bufsiz);
        }

        clearRecord(msg);
        free(msg);
    }
    deleteRecord(msg);
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
void handleClient( Client* client ) {
    if (client == NULL) {
        fprintf(stderr, "handleClient: Client is null");
        return; }

    int read;
    int bytesRead = 0;
    char buf[BUFSIZ];
    
    printf("HEI JEG LESER\n");
    read = tcp_read(client->ID, buf, bytesRead);
    printf("HEI JEG HAR LESET %s\n", buf);

    if (client->type == 'X') {
        forwardMessage(XMLtoRecord(buf, sizeof(buf), &bytesRead));
    } else if (client->type == 'B') {
        forwardMessage(BinaryToRecord(buf, sizeof(buf), &bytesRead));
    }
    

    // When done reading and all records has been sent
    removeClient(client);
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
    fd_set current_sockets, ready_sockets;


    FD_ZERO(&current_sockets);
    FD_SET(server_sock, &current_sockets);
    
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
        ready_sockets = current_sockets;
        printf("Waiting\n");
        if (tcp_wait(&ready_sockets, server_sock + 1) < 0) {
            fprintf(stderr, "Error proxy main: tcp_wait: %s\n", strerror(errno));
        }

        printf("JHER HAR VENTET\n");
        
        for (int i=0; i<=max_fd; ++i) {
            if (FD_ISSET(i, &ready_sockets)){
                printf("HEiASDASDASDA jeg er na inne i FDISSET\n");
                printf("HEI FAEN HER ER I%d\n", i);
                if (i == server_sock) {
                    int client_sock = handleNewClient(server_sock);
                    if (client_sock < 0) { continue; }
                    if (client_sock > max_fd) { max_fd = client_sock; }

                    printf("New client %d\n", client_sock);
                    printf("New client FD %d\n", getFirst()->fd);
                    printf("New client TYPE %c\n", getFirst()->type);
                    printf("New client ID %c\n", getFirst()->ID);

                    FD_SET(client_sock, &current_sockets);
                    connectedClients++;
                } else {
                    printf("Handling client %d\n", i);
                    handleClient(findByFD(i));
                    FD_CLR(i, &current_sockets);                    
                    connectedClients--;
                }
            }
        }
        
    }
    while(connectedClients);
    printf("JER hARE ferdig med lokken\n");

    /* add your cleanup code */
    while (start) { removeClient(start); }

    tcp_close( server_sock );

    return 0;
}

