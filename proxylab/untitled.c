#include <stdio.h>
#include <stdlib.h>
#include "csapp.h"

#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

static const char *user_agent = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static const char *accept_encoding = "Accept-Encoding: gzip, deflate\r\n";
static const char *text = "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
static const char *connection = "Connection: close\r\n";
static const char *proxy_connection = "Proxy-Connection: close\r\n";


void connection_handler(int client_fd);
void server_to_client(rio_t server, int client_fd);
rio_t proxy_to_server(char** hostnamee, int* port_num, char** pathh, void* client_buffer, rio_t client, int * server_fd);
int parse_url(char** urll, char** path, char** url_port, char** hostnamee, int* port_num);
void *thread(void *vargp);
int open_clientfd(char *hostname, int port);


sem_t mutex;

/* Main function waits and accepts requests and creates threads to
 * handle them. Each thread is passed to the thread function
 * so it is detached properly and then passed on to the connection_handler
 */
int main(int argc, char** argv)
{
    int listen_fd, port, *client_fd;
    unsigned int clientlen;
    struct sockaddr_in clientaddr;
    pthread_t tid;
    /* Check command line args */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    //ignoring sigpipe signal
    Signal(SIGPIPE, SIG_IGN);

    //getting port number if theres one
    port = atoi(argv[1]);

    //open a port for listening for the client
    listen_fd = Open_listenfd(port);

    sem_init(&mutex, 0, 1);
    while(1)
    {
        clientlen = sizeof(clientaddr);
        client_fd = Malloc(sizeof(int));
        *client_fd = Accept(listen_fd, (SA *)&clientaddr, &clientlen);
        Pthread_create(&tid, NULL, thread, client_fd);
    }
    return 0;
}

/* Thread funtion which detaches each thread and frees the client_fd
 * which was malloced in th main functon. Each thread is then passed
 * on to the connection_handler function.
 */
void *thread(void *vargp){
    int client_fd = *((int *)vargp);
    Pthread_detach(pthread_self());
    Free(vargp);
    connection_handler(client_fd);
    return NULL;
}

/* connection_handler function reads the client buffer, parses the url
 * through the parse_url function, creates the request to send the server
 * with the proxy_to_server function, and finally sends the data back to
 * the client with the server_to_client function
 */
void connection_handler(int client_fd)
{
    // printf("ENTERED connection_handler \n");
    rio_t client;
    char client_buffer[MAXBUF];
    char method[10];
    char* url = Malloc(sizeof(char)*MAXLINE);
    char version[15];
    rio_t server;
    int server_fd;

    memset(client_buffer, 0, sizeof(client_buffer));

    //initializing rio with connfd
    Rio_readinitb(&client, client_fd);

    //assiocitaing buffer witn connfd
    Rio_readlineb(&client, client_buffer, MAXLINE);

    //splits line into GET, the url and version number
    sscanf(client_buffer, "%s %s %s", method, url, version);
    strcpy(version, "HTTP/1.0");

    if(!strcmp(method, "GET"))
    {
        char* path = Malloc(sizeof(char)*MAXBUF);
        char* url_port = Malloc(sizeof(char)*5);
        char* hostname = Malloc(sizeof(char)*MAXBUF);
        int port_num;
        int checkUrl = parse_url(&url, &path, &url_port, &hostname, &port_num);

        if(checkUrl == -1){
            Close(client_fd);
            return;
        }
        fflush(stdout);

        //open server connection and send the information to server
        server = proxy_to_server(&hostname, &port_num, &path, &client_buffer, client, &server_fd);

        //send the server response to client again

        server_to_client(server, client_fd);

        Free(url);
        Free(path);
        Free(url_port);
        Free(hostname);

        Close(server_fd);
    }
    Close(client_fd);
}

void server_to_client(rio_t server, int client_fd)
{
    char new_buffer[MAXBUF];
    memset(new_buffer, '\0', MAXLINE);
    int line;
//    memset(new_buffer, 0, strlen(new_buffer));
    while(((line =  Rio_readnb(&server, new_buffer, MAXLINE)) > 0))
        //&& new_buffer[0] != '\r')
    {
        printf("%s", new_buffer);
        Rio_writen(client_fd, new_buffer, line);
        memset(new_buffer, 0, strlen(new_buffer));
    }
}

/* proxy_to_server function puts together the request to send to the server
 * and sends it. It takes the values from the parse_url function and
 * gets any remaining values from the client_buffer to put into the server
 * request
 */
rio_t proxy_to_server(char** hostnamee, int* port_num, char** pathh,
                      void* client_buffer, rio_t client, int * server_fd)
{
    rio_t server;
    int length;
    char transfer_path[MAXLINE];
    char transfer_host[MAXLINE];
    char transfer[MAXLINE];

    char *hostname = *hostnamee;
    char *path = *pathh;
    int line;

    int flag = 1;

    //opening connection to server
    *server_fd = Open_clientfd(hostname, *port_num);
    Rio_readinitb(&server, *server_fd);


    //WRITING THE PATH to the transfer string
    length = strlen("GET ") + strlen(path) + strlen(" HTTP/1.0");
    strcpy(transfer_path, "GET ");
    strcat(transfer_path, path);
    strcat(transfer_path, " HTTP/1.0\r\n");
    strcpy(transfer, transfer_path);


    //WRITING THE HOSTNAME to the tranfer string
    length = strlen(hostname) + strlen("Host: ");
    strcpy(transfer_host, "Host: \0");
    strcat(transfer_host, hostname);
    strcat(transfer_host, "\r\n");
    strcat(transfer, transfer_host);

    //WRITING all the static variables to the transfer string
    strcat(transfer, user_agent);
    strcat(transfer, text);
    strcat(transfer, accept_encoding);
    strcat(transfer, connection);
    strcat(transfer, proxy_connection);

    //WRITING ANY OF THE REMAINIG STUFF in the client buffer to the transfer
    while((line = Rio_readlineb(&client, client_buffer, MAXLINE))
        && line > 2)
    {
        if(!strncmp(client_buffer, "GET", 3) ||
           !strncmp(client_buffer, "Host:", 5) ||
           !strncmp(client_buffer, "User-Agent:", 11) ||
           !strncmp(client_buffer, "Accept:", 7) ||
           !strncmp(client_buffer, "Accept-Encoding", 15) ||
           !strncmp(client_buffer, "Proxy-Connection", 16) ||
           !strncmp(client_buffer, "Connection", 10))
        {
            flag = 0;
        }

        else flag =1;

        if(flag)
        {
            strcat(transfer, client_buffer);
        }
    }

    strcat(transfer, "\r\n");

    printf("%s", transfer);

    //WRITING THE TRANSFER STRING TO THE SERVER
    Rio_writen(*server_fd, (void *)transfer, strlen(transfer));

    return server;
}

/* parse_url function splits the url into the hostname, port number and
 * the path, if the url is not valid it returns -1 otherwise if it is valid
 * it returns 0
 */
int parse_url(char** urll, char** pathh, char** url_port, char** hostnamee, int* port_num)
{

    int i = 7; //starting the traversal at 7 to accoutn for http://
    char check[7];
    int curr_i = 7;

    char * url = *urll;
    char * hostname = *hostnamee;
    char * path = *pathh;

    strcat(url, "\0");
    memset(hostname, 0, sizeof(hostname));
    memset(path, 0, sizeof(path));
    memset(check, 0, sizeof(check));

    strncpy(check, url, 7);

    /*checking to make sure that the first 7 characters have to be
    http://
    */
    if(!strcmp(check, "http://"))
    {
        //GETTING THE HOSTNAME
        while(url[i] != '/' && url[i] != ':'
                  && url[i] != '\0')
        {
            hostname[i-curr_i] = url[i];
            i++;
        }
        hostname[i-curr_i] = '\0';

        printf("hostname %s\n", hostname);
        fflush(stdout);

        //GETTING THE PORT IF THERE IS ONE
        if(url[i] == ':')
        {
            curr_i = i;
            while(url[i] != '/' && url[i] != '\0')
            {
                *url_port[i - curr_i] = url[i];
                i++;
            }
            *url_port[i-curr_i] = '\0';
            *port_num = atoi(*url_port);
        }

        //DEAULT PORT IF THERE WAS NO PORT GIVEN
        else{
            *port_num = 80;
        }

        curr_i = i;


        //GETTING PATH
        while(url[i] != '\0')
        {
            path[i-curr_i] = url[i];
            i++;
        }
        return 0;
    }
    return -1;
}

/*
 * open_clientfd - open connection to server at <hostname, port>
 *   and return a socket descriptor ready for reading and writing.
 *   Returns -1 and sets errno on Unix error.
 *   Returns -2 and sets h_errno on DNS (gethostbyname) error.
 */
/* $begin open_clientfd */
int open_clientfd(char *hostname, int port)
{
    int clientfd;
    struct hostent *hp;
    struct sockaddr_in serveraddr;

    if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    return -1; /* check errno for cause of error */

    /* Fill in the server's IP address and port */
    P(&mutex);
    if ((hp = gethostbyname(hostname)) == NULL){
    V(&mutex);
    return -2; /* check h_errno for cause of error */
    }
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)hp->h_addr_list[0],
      (char *)&serveraddr.sin_addr.s_addr, hp->h_length);
    serveraddr.sin_port = htons(port);
    V(&mutex);

    /* Establish a connection with the server */
    if (connect(clientfd, (SA *) &serveraddr, sizeof(serveraddr)) < 0)
        return -1;
    return clientfd;
}
