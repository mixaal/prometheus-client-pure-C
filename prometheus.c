#include "all.h"

static _metric_internal_holder_t holder;


void metric_init(void)
{
   holder.current_top = 0;
}

static void error(const char *message)
{
   fprintf(stderr, "ERR: %s\n", message);
}

//NOTE: Right now we don't use the labels value
void metric_register_new(const char *name, const char *help, metric_type_t mtype, const char **labels, size_t label_no) 
{
  if(holder.current_top>=MAX_METRICS) {
     error("max metrics capacity exahusted");
     return;
  }
  metric_t *m = xmalloc(sizeof(*m));
  holder.array[holder.current_top] = m;
  holder.current_top ++;
  m -> name = name;
  m -> help = help;
  m -> type = mtype;
  m -> _value_holders = NULL;
  m -> _label_no = label_no;
  m -> _value = PROM_VALZERO;
}

static metric_t *find_metric(const char *name)
{
  metric_t *m = NULL;
  for(int i=0; i<holder.current_top; i++) {
    m = holder.array[i];
    if(m!=NULL && !strcmp(m->name, name)) return holder.array[i];
  }
  return NULL;
}

// compare all label values, 1 if all match, 0 otherwise
static _Bool _is_label_value(metric_label_value_combination_t *lvc, metric_label_t **label_values, size_t label_no)
{
   for(int i=0; i<label_no; i++) {
     metric_label_t *lv1 = lvc->label_values[i];
     metric_label_t *lv2 = label_values[i];
     if(strcmp(lv1->label, lv2->label) || strcmp(lv1->value, lv2->value)) {
        return 0;
     }
   }
   return 1;
}

// Find a value for a given label-valued combination, NULL if cannot find, ptr to the combination otherwise
static metric_label_value_combination_t *_find_metric_value_combination(metric_label_t **label_values, metric_t *m) 
{
   if(m==NULL) {
      error("illegal argument: metric is NULL");
      return NULL;
   }
   metric_label_value_combination_t *llist = m -> _value_holders;
   if(llist == NULL) {
     // Don't have any combination yet
     return NULL;
   }
   while(llist!=NULL) {
     if(_is_label_value(llist, label_values, m->_label_no)) return llist;
     llist = llist -> next;
   }

   // Unable to find the combination
   return NULL;
}

void metric_inc_labeled(const char *name, metric_label_t **labels, PROM_VALTYPE amount)
{
   metric_t *m = find_metric(name);
  if(m==NULL) {
    error("can't find metric");
    return;
  }
  if(m -> _label_no == 0) {
    error("scalar-based metric, use non-label API instead!");
    return;
  } 
  if(m -> type == COUNTER && amount < 0) {
    error("counter value can't be decreased - use gauge type instead");
    return;
  }
  metric_label_value_combination_t *lvc = _find_metric_value_combination(labels, m);
  if(lvc!=NULL) {
    lvc -> value += amount;
  } else {
    // Need to create a new combination
    lvc = xmalloc(sizeof(metric_label_value_combination_t));
    lvc -> value = amount;
    lvc -> next = NULL;
    lvc -> label_values = labels;
    if ( m -> _value_holders == NULL ) {
      // this means we didn't have any labeled holders yet, this one is the first one.
      m -> _value_holders = lvc;
    } else {
      // In this case we want to find the last one:
      metric_label_value_combination_t *llist = m -> _value_holders;
      while ( llist -> next != NULL) llist = llist -> next;
      llist -> next = lvc;
    }
  }
}

void metric_inc(const char *name, PROM_VALTYPE amount)
{
  metric_t *m = find_metric(name);
  if(m==NULL) {
    error("can't find metric");
    return;
  }
  if(m -> _label_no > 0) {
    error("label-based metric, use label API instead!");
    return;
  } 
  if(m -> type == COUNTER && amount < 0) {
    error("counter value can't be decreased - use gauge type instead");
    return;
  }
  m -> _value += amount;
}

void metric_set(const char *name, PROM_VALTYPE value)
{
  metric_t *m = find_metric(name);
  if(m==NULL) {
    error("can't find metric");
    return;
  }
  if(m -> _label_no > 0) {
    error("label-based metric, use label API instead!");
    return;
  } 
  if(m -> type == COUNTER ) {
    error("counter value can't be set - use gauge type instead");
    return;
  }
  m -> _value = value;
}


static char *get_type_name(metric_type_t mtype)
{
  switch(mtype) {
    case COUNTER: return "counter";
    case GAUGE: return "gauge";
    default: return "unknown";
  }
}

static void _metric_print(FILE *fp, metric_t *m)
{
  if(fp==NULL || m==NULL) return;

  metric_label_value_combination_t *llist = m -> _value_holders;
  if(llist!=NULL || m -> _label_no ==0) {
    fprintf(fp, "# HELP %s %s\n", m -> name, m -> help);
    fprintf(fp, "# TYPE %s %s\n", m -> name, get_type_name(m -> type));
  }
  if( m -> _label_no == 0 ) {
    fprintf(fp, "%s %0.2f\n", m -> name, m -> _value);
  } else {
    while( llist != NULL ) {
      fprintf(fp, "%s{", m -> name);
      for(int i=0; i<m -> _label_no; i++) {
         metric_label_t *lv = llist->label_values[i];
         fprintf(fp, "%s=\"%s\",", lv -> label, lv -> value);
      }
      fprintf(fp, "} %f\n", llist -> value );
      llist = llist -> next;
    }
  }
}

void metric_print(FILE *fp)
{
  for (int i=0; i<holder.current_top; i++) {
    metric_t *m = holder.array[i];
    _metric_print(fp, m);
  }
}
