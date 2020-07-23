#ifndef __PROMETHEUS_API_H__
#define __PROMETHEUS_API_H__ 1

#define PROM_VALTYPE float
#define PROM_VALZERO 0.0f

/**
 * Metric type, currently only counter and gauge implemented
 */
typedef enum { GAUGE, COUNTER } metric_type_t;

/**
 * Metric label and label value
 */
typedef struct {
  const char *label;
  const char *value;
} metric_label_t;

/**
 * Value for a given label combination
 */
typedef struct _label_node {
  metric_label_t **label_values;
  PROM_VALTYPE value;
  struct _label_node *next;
} metric_label_value_combination_t;

/**
 * Metric definition
 */
typedef struct {
  /**
   * Metric name
   */
  const char *name;
  /**
   * Metric help/description
   */
  const char *help;
  /**
   * Metric type
   */
  metric_type_t type;

  /**
   * Labeled metric values.
   */
  metric_label_value_combination_t *_value_holders;
 
  /**
   * Scalar metric value. (there can be union of labled and scalar metrics)
   */
  PROM_VALTYPE _value;

  /**
   * Number of labels, can't be changed after registration. 
   */
  size_t _label_no;

  /**
   * For labeled metrics, check for labels provided. Store on definition here or NULL for scalar metrics 
   */
  const char **_labels;
} metric_t; 

/**
 * API contract
 */
void metric_register_new(const char *name, const char *help, metric_type_t mtype, const char **labels, size_t label_no);
void metric_inc(const char *name, PROM_VALTYPE value);
void metric_set(const char *name, PROM_VALTYPE value);
void metric_inc_labeled(const char *name, metric_label_t **labels, PROM_VALTYPE value);
void metric_set_labeled(const char *name, metric_label_t **labels, PROM_VALTYPE value);
void metric_release(const char *name);
void metric_release_all(void);
void metric_init(void);
void metric_print(FILE *fp);
metric_label_t **with_labels(size_t label_no, ...) ;

#define MAX_METRICS 100
typedef struct {
  metric_t *array[MAX_METRICS];
  size_t current_top;
}  _metric_internal_holder_t;

#endif /* __PROMETHEUS_API_H__ */
