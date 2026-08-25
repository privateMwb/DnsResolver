# DnsPro Benchmark Results

## Lookup

| Test | Iteration | DnsPro |
|---|---|---|
| Existing Name+type | 10K | 6.44 ms |
| Existing Name+type | 100K | 83.68 ms |
| Existing Name+type | 1M | 679.75 ms |
| Missing Name | 10K | 1.76 ms |
| Missing Name | 100K | 17.58 ms |
| Missing Name | 1M | 175.52 ms |
| Existing Name, Missing Type | 10K | 2.22 ms |
| Existing Name, Missing Type | 100K | 21.87 ms |
| Existing Name, Missing Type | 1M | 218.92 ms |

## Build

| Test | Iteration | DnsPro |
|---|---|---|
| Query, Single Question | 10K | 11.21 ms |
| Query, Single Question | 100K | 58.01 ms |
| Query, Single Question | 1M | 505.47 ms |
| Response, 4 Answer Records | 10K | 14.35 ms |
| Response, 4 Answer Records | 100K | 144.99 ms |
| Response, 4 Answer Records | 1M | 2.62 s |

## Parse

| Test | Iteration | DnsPro |
|---|---|---|
| Query, Single Question | 10K | 8.67 ms |
| Query, Single Question | 100K | 86.53 ms |
| Query, Single Question | 1M | 865.20 ms |
| Response, 4 Answer Records | 10K | 42.03 ms |
| Response, 4 Answer Records | 100K | 419.92 ms |
| Response, 4 Answer Records | 1M | 12.86 s |

## Record

| Test | Iteration | DnsPro |
|---|---|---|
| Existing Name+type Bucket | 10K | 27.83 ms |
| Existing Name+type Bucket | 100K | 224.77 ms |
| Existing Name+type Bucket | 1M | 2.27 s |
| Missing Name | 10K | 2.70 ms |
| Missing Name | 100K | 26.99 ms |
| Missing Name | 1M | 270.71 ms |

## Resolve

| Test | Iteration | DnsPro |
|---|---|---|
| Answer Found | 10K | 46.62 ms |
| Answer Found | 100K | 464.77 ms |
| Answer Found | 1M | 4.14 s |
| NXDOMAIN | 10K | 16.82 ms |
| NXDOMAIN | 100K | 213.06 ms |
| NXDOMAIN | 1M | 2.57 s |
| NODATA | 10K | 18.38 ms |
| NODATA | 100K | 236.03 ms |
| NODATA | 1M | 2.12 s |

## Message Move

| Test | Iteration | DnsPro |
|---|---|---|
| Move-construct | 10K | 1.07 ms |
| Move-construct | 100K | 7.53 ms |
| Move-construct | 1M | 64.99 ms |
| Move-assign | 10K | 593.00 us |
| Move-assign | 100K | 6.05 ms |
| Move-assign | 1M | 57.14 ms |

## Answer Count Growth

| Test | Iteration | DnsPro |
|---|---|---|
| 4 Answer Records | 10K | 20.32 ms |
| 4 Answer Records | 100K | 247.10 ms |
| 4 Answer Records | 1M | 2.25 s |
| 16 Answer Records | 10K | 70.03 ms |
| 16 Answer Records | 100K | 822.94 ms |
| 16 Answer Records | 1M | 11.92 s |
| 64 Answer Records | 10K | 568.19 ms |
| 64 Answer Records | 100K | 5.10 s |
| 64 Answer Records | 1M | 38.87 s |
| 4 Answer Records | 10K | 25.17 ms |
| 4 Answer Records | 100K | 219.22 ms |
| 4 Answer Records | 1M | 2.14 s |
| 16 Answer Records | 10K | 50.28 ms |
| 16 Answer Records | 100K | 559.67 ms |
| 16 Answer Records | 1M | 5.49 s |
| 64 Answer Records | 10K | 155.37 ms |
| 64 Answer Records | 100K | 1.54 s |
| 64 Answer Records | 1M | 16.12 s |

## Label Depth Growth

| Test | Iteration | DnsPro |
|---|---|---|
| 2 Labels Deep | 10K | 8.69 ms |
| 2 Labels Deep | 100K | 89.55 ms |
| 2 Labels Deep | 1M | 896.91 ms |
| 8 Labels Deep | 10K | 10.81 ms |
| 8 Labels Deep | 100K | 114.54 ms |
| 8 Labels Deep | 1M | 1.07 s |
| 32 Labels Deep | 10K | 27.50 ms |
| 32 Labels Deep | 100K | 275.07 ms |
| 32 Labels Deep | 1M | 2.90 s |
| 2 Labels Deep | 10K | 10.78 ms |
| 2 Labels Deep | 100K | 107.52 ms |
| 2 Labels Deep | 1M | 1.13 s |
| 8 Labels Deep | 10K | 19.28 ms |
| 8 Labels Deep | 100K | 204.65 ms |
| 8 Labels Deep | 1M | 2.08 s |
| 32 Labels Deep | 10K | 39.16 ms |
| 32 Labels Deep | 100K | 486.45 ms |
| 32 Labels Deep | 1M | 3.89 s |

## Zone Size Growth

| Test | Iteration | DnsPro |
|---|---|---|
| 100 Names Stored | 10K | 13.89 ms |
| 100 Names Stored | 100K | 153.93 ms |
| 100 Names Stored | 1M | 1.56 s |
| 1,000 Names Stored | 10K | 13.88 ms |
| 1,000 Names Stored | 100K | 141.70 ms |
| 1,000 Names Stored | 1M | 1.46 s |
| 10,000 Names Stored | 10K | 14.22 ms |
| 10,000 Names Stored | 100K | 141.94 ms |
| 10,000 Names Stored | 1M | 1.45 s |

## Canonicalize

| Test | Iteration | DnsPro |
|---|---|---|
| Mixed-case Name | 10K | 19.04 ms |
| Mixed-case Name | 100K | 212.46 ms |
| Mixed-case Name | 1M | 2.24 s |

## Name Parse

| Test | Iteration | DnsPro |
|---|---|---|
| Uncompressed Name | 10K | 9.06 ms |
| Uncompressed Name | 100K | 89.55 ms |
| Uncompressed Name | 1M | 927.20 ms |
| Name Via Compression Pointer | 10K | 13.18 ms |
| Name Via Compression Pointer | 100K | 131.91 ms |
| Name Via Compression Pointer | 1M | 1.35 s |
