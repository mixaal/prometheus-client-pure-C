# Prometheus Exporter - Pure C 

This is a simple prometheus metrics exporter for pure C projects that are not allowed to use bigger
libraries taking care of prometheus due to licensing issues.

## Latest Build

 * Includes the prometheus download and running the `promtool check metrics` to ensure that metrics we are creating are compliant with prometheus scaper.

[![Build Status](https://travis-ci.com/mixaal/prometheus-exporter-pure-C.svg?branch=master)](https://travis-ci.com/mixaal/prometheus-exporter-pure-C)



## Quickstart

```
make clean && make
```

## Example Usage

```
  metric_init();
  metric_register_new("tenants_running_query", "number of tenants running queries", GAUGE, NULL, 0);
  metric_inc("tenants_running_query", 30.0f);
  metric_release_all();
```
## Example Output

```
# HELP tenants_running_query number of tenants running queries
# TYPE tenants_running_query gauge
tenants_running_query 0.00
# HELP csqls_running_query number of cloud sql instances running queries
# TYPE csqls_running_query gauge
csqls_running_query 30.00
# HELP allocations_per_service number of allocations per service
# TYPE allocations_per_service gauge
allocations_per_service{service="my-service-1",} 70.000000
allocations_per_service{service="my-service-2",} 28.000000
# HELP requests requests per code
# TYPE requests gauge
requests{code="200",method="get",} 280.000000
requests{code="400",method="get",} 180.000000
requests{code="500",method="get",} 80.000000
```


