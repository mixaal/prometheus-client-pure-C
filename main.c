#include "all.h"

int main(void)
{
  metric_init();
  metric_register_new("tenants_running_query", "number of tenants running queries", GAUGE, NULL, 0);
  metric_register_new("csqls_running_query", "number of cloud sql instances running queries", GAUGE, NULL, 0);
  const char *allocation_labels[] = {"service"};
  metric_register_new("allocations_per_service", "number of allocations per service", GAUGE, allocation_labels, 1);
  const char *req_labels[] = {"code", "method"};
  metric_register_new("requests", "requests per code", GAUGE, req_labels, 2);
  metric_inc("csqls_running_query", 30.0f);
  // Since user is responsible to free the memory allocated with method ``with_labels()'' this is not
  // the best way of loosing the pointer
  metric_inc_labeled("allocations_per_service", with_labels(1, "service", "my-service-1"), 40);
  metric_inc_labeled("allocations_per_service", with_labels(1, "service", "my-service-1"), 30);
  metric_inc_labeled("allocations_per_service", with_labels(1, "service", "my-service-2"), 18);
  metric_set_labeled("allocations_per_service", with_labels(1, "service", "my-service-2"), 28);
  metric_set_labeled("requests", with_labels(2, "code", "200", "method", "get"), 280);
  metric_set_labeled("requests", with_labels(2, "code", "400", "method", "get"), 180);
  metric_set_labeled("requests", with_labels(2, "code", "500", "method", "get"), 80);
  metric_print(stdout);
#ifdef _START_METRICS_SERVER
  metrics_server_t * server = server_create(2000);
  if(server == NULL) {
    exit(EXIT_FAILURE);
  }
  for(;;) {
    int conn_fd = server_accept(server);
    server_write_metrics(conn_fd);
  }
#endif /* _START_METRICS_SERVER */  
  //metric_release_all();

  return EXIT_SUCCESS;
}
