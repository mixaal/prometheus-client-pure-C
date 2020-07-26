#ifndef __PROMETHEUS_EXPORTER_SERVER_H__
#define __PROMETHEUS_EXPORTER_SERVER_H__ 1

#define BACKLOG_NO 10


typedef struct {
  int listen_port;
  int _listen_fd;
} metrics_server_t;

metrics_server_t *server_create(int port);
int server_accept(metrics_server_t *server);
void server_write_metrics(int conn_fd);
void server_close(int conn_fd);
void server_destroy(metrics_server_t *server);

#endif /* __PROMETHEUS_EXPORTER_SERVER_H__ */
