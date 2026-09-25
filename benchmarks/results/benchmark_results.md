# DnsProBenchmark Results

## Lookup

| Test | Iteration | DnsPro |
|---|---|---|
| Existing Name Type | 10K | 11.25 ms |
| Existing Name Type | 100K | 132.95 ms |
| Existing Name Type | 1M | 1.34 s |
| Missing Name | 10K | 1.92 ms |
| Missing Name | 100K | 19.01 ms |
| Missing Name | 1M | 189.94 ms |
| Existing Name Missing Type | 10K | 2.36 ms |
| Existing Name Missing Type | 100K | 23.88 ms |
| Existing Name Missing Type | 1M | 236.38 ms |

## Build

| Test | Iteration | DnsPro |
|---|---|---|
| Query, Single Question | 10K | 7.76 ms |
| Query, Single Question | 100K | 118.15 ms |
| Query, Single Question | 1M | 1.20 s |
| Response, 4 Answer Records | 10K | 30.27 ms |
| Response, 4 Answer Records | 100K | 231.85 ms |
| Response, 4 Answer Records | 1M | 3.98 s |

## Parse

| Test | Iteration | DnsPro |
|---|---|---|
| Query, Single Question | 10K | 12.71 ms |
| Query, Single Question | 100K | 154.71 ms |
| Query, Single Question | 1M | 1.31 s |
| Response, 4 Answer Records | 10K | 49.59 ms |
| Response, 4 Answer Records | 100K | 865.99 ms |
| Response, 4 Answer Records | 1M | 4.22 s |

## Record

| Test | Iteration | DnsPro |
|---|---|---|
| Existing Name+type Bucket | 10K | 12.02 ms |
| Existing Name+type Bucket | 100K | 110.75 ms |
| Existing Name+type Bucket | 1M | 1.34 s |
| Missing Name | 10K | 1.85 ms |
| Missing Name | 100K | 18.61 ms |
| Missing Name | 1M | 185.49 ms |

## Resolve

| Test | Iteration | DnsPro |
|---|---|---|
| Answer Found | 10K | 37.74 ms |
| Answer Found | 100K | 404.66 ms |
| Answer Found | 1M | 4.82 s |
| NXDOMAIN | 10K | 36.97 ms |
| NXDOMAIN | 100K | 369.29 ms |
| NXDOMAIN | 1M | 3.72 s |
| NODATA | 10K | 37.62 ms |
| NODATA | 100K | 378.42 ms |
| NODATA | 1M | 3.76 s |

## Message Move

| Test | Iteration | DnsPro |
|---|---|---|
| Move-construct | 10K | 685.00 us |
| Move-construct | 100K | 6.92 ms |
| Move-construct | 1M | 70.63 ms |
| Move-assign | 10K | 1.08 ms |
| Move-assign | 100K | 7.11 ms |
| Move-assign | 1M | 70.31 ms |

## Answer Count Growth

| Test | Iteration | DnsPro |
|---|---|---|
| 4 Answer Records | 10K | 49.42 ms |
| 4 Answer Records | 100K | 470.25 ms |
| 4 Answer Records | 1M | 4.57 s |
| 16 Answer Records | 10K | 168.10 ms |
| 16 Answer Records | 100K | 1.40 s |
| 16 Answer Records | 1M | 15.07 s |
| 64 Answer Records | 10K | 276.80 ms |
| 64 Answer Records | 100K | 2.76 s |
| 64 Answer Records | 1M | 49.19 s |
| 4 Answer Records | 10K | 32.48 ms |
| 4 Answer Records | 100K | 327.05 ms |
| 4 Answer Records | 1M | 3.21 s |
| 16 Answer Records | 10K | 60.67 ms |
| 16 Answer Records | 100K | 609.32 ms |
| 16 Answer Records | 1M | 6.21 s |
| 64 Answer Records | 10K | 172.23 ms |
| 64 Answer Records | 100K | 1.73 s |
| 64 Answer Records | 1M | 17.53 s |

## Label Depth Growth

| Test | Iteration | DnsPro |
|---|---|---|
| 2 Labels Deep | 10K | 9.76 ms |
| 2 Labels Deep | 100K | 100.42 ms |
| 2 Labels Deep | 1M | 997.14 ms |
| 8 Labels Deep | 10K | 11.71 ms |
| 8 Labels Deep | 100K | 129.55 ms |
| 8 Labels Deep | 1M | 1.21 s |
| 32 Labels Deep | 10K | 30.54 ms |
| 32 Labels Deep | 100K | 309.32 ms |
| 32 Labels Deep | 1M | 3.12 s |
| 2 Labels Deep | 10K | 11.84 ms |
| 2 Labels Deep | 100K | 133.33 ms |
| 2 Labels Deep | 1M | 1.27 s |
| 8 Labels Deep | 10K | 21.17 ms |
| 8 Labels Deep | 100K | 213.82 ms |
| 8 Labels Deep | 1M | 2.17 s |
| 32 Labels Deep | 10K | 48.78 ms |
| 32 Labels Deep | 100K | 436.76 ms |
| 32 Labels Deep | 1M | 4.22 s |

## Zone Size Growth

| Test | Iteration | DnsPro |
|---|---|---|
| 100 Names Stored | 10K | 19.42 ms |
| 100 Names Stored | 100K | 161.78 ms |
| 100 Names Stored | 1M | 1.56 s |
| 1,000 Names Stored | 10K | 15.53 ms |
| 1,000 Names Stored | 100K | 156.57 ms |
| 1,000 Names Stored | 1M | 1.59 s |
| 10,000 Names Stored | 10K | 15.43 ms |
| 10,000 Names Stored | 100K | 156.76 ms |
| 10,000 Names Stored | 1M | 1.55 s |

## Canonicalize

| Test | Iteration | DnsPro |
|---|---|---|
| Mixed-case Name | 10K | 21.36 ms |
| Mixed-case Name | 100K | 236.91 ms |
| Mixed-case Name | 1M | 2.52 s |

## Name Parse

| Test | Iteration | DnsPro |
|---|---|---|
| Uncompressed Name | 10K | 9.87 ms |
| Uncompressed Name | 100K | 105.28 ms |
| Uncompressed Name | 1M | 1.14 s |
| Name Via Compression Pointer | 10K | 14.78 ms |
| Name Via Compression Pointer | 100K | 148.04 ms |
| Name Via Compression Pointer | 1M | 1.48 s |
