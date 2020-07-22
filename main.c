#include "all.h"

int main(void)
{
  metric_init();
  metric_register_new("tenants_running_query", "number of tenants running queries", GAUGE, NULL, 0);
  metric_register_new("csqls_running_query", "number of cloud sql instances running queries", GAUGE, NULL, 0);
  metric_register_new("allocations_per_service", "number of cloud sql instances running queries", GAUGE, NULL, 1);
  metric_inc("csqls_running_query", 30.0f);
  metric_label_t **labels = xmalloc(sizeof(metric_label_t *));
  labels[0] = xmalloc(sizeof(metric_label_t));
  labels[0]->label = "service1";
  labels[0]->value = 20.0f;
  metric_inc_labeled("allocations_per_service", labels, 40);
  metric_inc_labeled("allocations_per_service", labels, 30);
  labels = xmalloc(sizeof(metric_label_t *));
  labels[0] = xmalloc(sizeof(metric_label_t));
  labels[0]->label = "service2";
  labels[0]->value = 10.0f;
  metric_inc_labeled("allocations_per_service", labels, 18);
  metric_print(stderr);
  //metric_release_all();
  return 0;
}
