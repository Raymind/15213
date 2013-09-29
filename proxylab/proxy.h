void connection_handler(int client_fd);
void server_to_client(rio_t server, int client_fd, char** urii);
rio_t proxy_to_server(char** hostnamee, int* port_num, char** pathh,
            void* client_buffer, rio_t client, int * server_fd, int client_fd);
int parse_url(char** urll, char** path, char** hostnamee,
              int* port_num);
void *thread(void *vargp);
int open_clientfd_r(char *hostname, int port);
void clean(char* path, char* hostname, char* url);