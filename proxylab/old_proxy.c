#include <stdio.h>
#include "csapp.h"

#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum,
         char *shortmsg, char *longmsg);
void connection_handler(int connfd);
void *handle_thread(void* vargp);


int main(int argc, char** argv)
{

    printf("In main \n");

    int listenfd, connfd, port;
    unsigned int clientlen;
    struct sockaddr_in clientaddr;

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
    listenfd = Open_listenfd(port);
    printf("opened listenfd for client \n");

    while (1) {
        printf("In while loop to create threads \n");
        pthread_t tid;
        clientlen = sizeof(clientaddr);

        //waiting for connections
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        int* connect_descriptor = Malloc(sizeof(int));
        *connect_descriptor = connfd;
        printf("About to go into handle thread \n");

        //creating threads and calling handle_thread
        pthread_create(&tid, NULL, handle_thread, (void*)connect_descriptor);
    }
    return 0;
}

void *handle_thread(void* vargp)
{
    printf("in handle thread \n");
    int connfd = *((int*)vargp);

    //detaches thread for later so no memory leaks
    Pthread_detach(pthread_self());

    //free last malloced argument
    Free(vargp);
    printf("Done freeign and going into connection handler \n");
    connection_handler(connfd);
    return NULL;
}



void connection_handler(int connfd)
{
    printf("Entered connetion_handler \n");


    //initializing static variables from spec
    char* user_agent = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3)Gecko/20120305 Firefox/10.0.3\r\n";
    char* text = "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
    char* accept_encoding = "Accept-Encoding: gzip, deflate\r\n";
    char* connection = "Connection: close\r\n";
    char* proxy_connection = "Proxy-Connection: close\r\n";

    rio_t rio;
    char buffer[MAXLINE];
    char s_buf[MAXLINE];
    char method[MAXLINE];
    char url[MAXLINE];
    char version[MAXLINE];

    ssize_t line;
    rio_t server_buffer;


    printf("intialized everything \n");
    fflush(stdout);

    //initializing rio with connfd
    Rio_readinitb(&rio, connfd);

    //assiocitaing buffer witn connfd
    Rio_readlineb(&rio, buffer, MAXLINE);
    printf("%s", buffer);

    //splits line into GET, the url and version number
    sscanf(buffer, "%s %s %s", method, url, version);

    //changing version to standard 1.0
    strcpy(version, "HTTP/1.0");

    if(!strcasecmp(method, "GET")) {
        printf("The method is Get \n");
        fflush(stdout);
        char hostname[MAXLINE];
        char path[MAXLINE];
        char port_s[500];
        int serverfd;
        int port;
        int i = 11;
        int curr_i;
        char check[MAXLINE];

        //checking if the first 11 characters of buffer are "GET http://"
        strncpy(check, buffer, 11);
        if(!strcmp(check, "GET http://")) {

            //getting the url stop when we reach : for the port or a /
            while(buffer[i] != '/' && buffer[i] != ':'
                  && buffer[i] != '\0')
            {
                hostname[i-11] = buffer[i];
                i += 1;
            }

            //adding a null char for ending the string
            hostname[i - 11] = '\0';
            printf("Checkign if parsing is working \n");
            printf("Hostname: %s \n", hostname);


            curr_i = i;

            //if the above loop was stopped because of : then we have a port
            if(buffer[i] == ':'){
                printf("Port was given \n");
                while(buffer[i] != '/' && buffer[i] != '\0')
               {
                    printf("current num is %c \n", buffer[i]);
                    port_s[i - curr_i] = buffer[i];
                    i+=1;
                }
                //convert port into an integer
                port = atoi(port_s);
            }
            curr_i = i;
            port = 80;

            //getting the last part which is the path
            while(buffer[i] != ' '){
                path[i-curr_i] = buffer[i];
                i++;
            }

        printf("The path is: %s\n", path);
        printf("Past parsing \n");
        fflush(stdout);

        //openign connection to the server
        printf("opening connection to server1 \n");
        fflush(stdout);
        serverfd = Open_clientfd(hostname, port);
        printf("opening connection to server2\n");
        fflush(stdout);
        Rio_readinitb(&server_buffer, serverfd);
        printf("opening connection to server3 \n");
        fflush(stdout);

        printf("The sbuf before anything is %s\n", s_buf);
         fflush(stdout);

        Rio_readlineb(&server_buffer, s_buf, MAXLINE);

        printf("opening connection to server done");
        fflush(stdout);

        //writing GET path HTTP/1.0 to the server_buffer
        int length = strlen("GET ") + strlen(path) + strlen(" HTTP/1.0");
        char val[MAXLINE];
        strcpy(val, "GET ");
        char* pussycat1 = strcat(val, path);
        char* pussycat2 = strcat(pussycat1, " HTTP/1.0");
        Rio_writen(serverfd, pussycat2, length);


        //writing Host: hostname to the serverbuffer
        length = strlen(hostname) + strlen("Host: ");
        char val1[MAXLINE];
        strcpy(val1, "Host: ");
        printf("hostname %s\n", hostname);
        char* kittycat = strcat(val1, hostname);
        Rio_writen(serverfd, kittycat, length);
        Rio_writen(serverfd, "\r\n", strlen("\r\n"));

        printf("port %d\n", port);


        //writing all static variables from above to the server_buffer
        Rio_writen(serverfd, user_agent, strlen(user_agent));
        Rio_writen(serverfd, text, strlen(text));
        Rio_writen(serverfd, accept_encoding, strlen(accept_encoding));
        Rio_writen(serverfd, connection, strlen(connection));
        Rio_writen(serverfd, proxy_connection, strlen(proxy_connection));

        while(line = Rio_readnb(&rio, buffer, MAXLINE))
        {
            if(strncmp(buffer, "GET", 3) ||
               strncmp(buffer, "Host:", 5) ||
               strncmp(buffer, "User-Agent:", 11) ||
               strncmp(buffer, "Accept-Encoding", 15) ||
               strncmp(buffer, "Proxy-Connection", 16))
            {
                Rio_writen(serverfd, buffer, line);

            }
        }
        printf("\n\n\n");
        printf("The s_buf is: \n\n\n");
        printf("%s", s_buf);
        /*now we have the server file descriptor and we need to send
        the buffer over this serverfd*/


        Rio_readlineb(&server_buffer, s_buf, MAXLINE);
        Rio_writen(serverfd, buffer, strlen(buffer));
        strcat(s_buf, "\r\n");

        //server to the client through the proxy

        Rio_writen(connfd, s_buf, strlen(s_buf));

        //close the serverfd once done
        close(serverfd);
    }
    }


    else{
        printf("The mehod wasnt get so we are closing rn");
        Close(connfd);
    }
}

/*
 * read_requesthdrs - read and parse HTTP request headers
 */
void read_requesthdrs(rio_t *rp)
{
    char buf[MAXLINE];

    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
    while(strcmp(buf, "\r\n")) {
    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
    }
    return;
}

/*
 * parse_uri - parse URI into filename and CGI args
 *             return 0 if dynamic content, 1 if static
 */
int parse_uri(char *uri, char *filename, char *cgiargs)
{
    char *ptr;

    if (!strstr(uri, "cgi-bin")) {  /* Static content */
    strcpy(cgiargs, "");
    strcpy(filename, ".");
    strcat(filename, uri);
    if (uri[strlen(uri)-1] == '/')
        strcat(filename, "home.html");
    return 1;
    }
    else {  /* Dynamic content */
    ptr = index(uri, '?');
    if (ptr) {
        strcpy(cgiargs, ptr+1);
        *ptr = '\0';
    }
    else
        strcpy(cgiargs, "");
    strcpy(filename, ".");
    strcat(filename, uri);
    return 0;
    }
}
/*
 * clienterror - returns an error message to the client
 */
void clienterror(int fd, char *cause, char *errnum,
         char *shortmsg, char *longmsg)
{
    char buf[MAXLINE], body[MAXBUF];

    /* Build the HTTP response body */
    sprintf(body, "<html><title>Tiny Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

    /* Print the HTTP response */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
}