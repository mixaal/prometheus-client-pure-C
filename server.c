#include "all.h"

metrics_server_t *server_create(int port)
{
  metrics_server_t *server = xmalloc(sizeof(metrics_server_t));
  int err = 0;
  server -> listen_port = port;
  server -> _listen_fd = err = socket(AF_INET, SOCK_STREAM, 0);
  if (err < 0) {
    perror("socket");
    return NULL;
  }
  struct sockaddr_in server_addr = { 0 };
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);
  err = bind(server->_listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if (err < 0) {
    perror("bind");
    return NULL;
  }  
  err = listen(server->_listen_fd, BACKLOG_NO);
  if (err < 0) {
    perror("listen");
    return NULL;
  }
  return server;
}

static void drain_socket(int fd)
{
  ssize_t nread;
  char buf[100];

  for(;;) {
     nread = read(fd, buf, sizeof(buf));
     if(nread<=0 || nread<sizeof(buf)) return;
  }
}

int server_accept(metrics_server_t *server)
{
  if(server == NULL) {
    return -1;
  }
  int err = 0;
  int conn_fd;
  socklen_t client_len;
  struct sockaddr_in client_addr;
  client_len = sizeof(client_addr);
  err = (conn_fd = accept(server->_listen_fd, (struct sockaddr*)&client_addr, &client_len));
  if(err < 0) {
    perror("accept");
    return err;
  }
  drain_socket(conn_fd);
  return conn_fd;
}

void server_close(int conn_fd)
{
  if(conn_fd >= 0) {
     int err = close(conn_fd);
     if(err < 0) {
       perror("close");
     }
  }
}

void server_write_metrics(int conn_fd)
{
  if(conn_fd < 0) return;

  FILE *fp = fdopen(conn_fd, "w");
  if(fp==NULL) {
    perror("fdopen");
    return;
  }
  fprintf(fp, "HTTP/1.1 200 OK\r\n");
  fprintf(fp, "Content-Type: text/plain; charset=ISO-8859-1\r\n");
  fprintf(fp, "Server: mikc-v0.1\r\n");
  fprintf(fp, "Connection: close\r\n");
  fprintf(fp, "\r\n");
  metric_print(fp);
  //fprintf(fp, "\r\n");
  fflush(fp);
  server_close(conn_fd);
}

void server_destroy(metrics_server_t *server)
{
 //FIXME
}
