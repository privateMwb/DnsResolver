# DnsPro Benchmark Results

## Lookup

| Test | Iteration | DnsPro |
|---|---|---|
| Existing Name+type | 10K | 8.52 ms |
| Existing Name+type | 100K | 75.89 ms |
| Existing Name+type | 1M | 702.79 ms |
| Missing Name | 10K | 1.98 ms |
| Missing Name | 100K | 19.56 ms |
| Missing Name | 1M | 195.44 ms |
| Existing Name, Missing Type | 10K | 2.43 ms |
| Existing Name, Missing Type | 100K | 24.34 ms |
| Existing Name, Missing Type | 1M | 243.26 ms |

## Build

| Test | Iteration | DnsPro |
|---|---|---|
| Query, Single Question | 10K | 5.62 ms |
| Query, Single Question | 100K | 55.99 ms |
| Query, Single Question | 1M | 589.59 ms |
| Response, 4 Answer Records | 10K | 15.91 ms |
| Response, 4 Answer Records | 100K | 159.05 ms |
| Response, 4 Answer Records | 1M | 1.90 s |

## Parse

| Test | Iteration | DnsPro |
|---|---|---|
| Query, Single Question | 10K | 4.61 ms |
| Query, Single Question | 100K | 90.30 ms |
| Query, Single Question | 1M | 469.59 ms |
| Response, 4 Answer Records | 10K | 22.45 ms |
| Response, 4 Answer Records | 100K | 232.94 ms |
| Response, 4 Answer Records | 1M | 3.97 s |

## Record

| Test | Iteration | DnsPro |
|---|---|---|
| Existing Name+type Bucket | 10K | 12.10 ms |
| Existing Name+type Bucket | 100K | 137.03 ms |
| Existing Name+type Bucket | 1M | 1.25 s |
| Missing Name | 10K | 1.90 ms |
| Missing Name | 100K | 19.42 ms |
| Missing Name | 1M | 192.23 ms |

## Resolve

| Test | Iteration | DnsPro |
|---|---|---|
| Answer Found | 10K | 23.81 ms |
| Answer Found | 100K | 291.24 ms |
| Answer Found | 1M | 2.72 s |
| NXDOMAIN | 10K | 17.69 ms |
| NXDOMAIN | 100K | 176.63 ms |
| NXDOMAIN | 1M | 1.77 s |
| NODATA | 10K | 18.30 ms |
| NODATA | 100K | 182.63 ms |
| NODATA | 1M | 2.07 s |

## Message Move

| Test | Iteration | DnsPro |
|---|---|---|
| Move-construct | 10K | 436.00 us |
| Move-construct | 100K | 4.38 ms |
| Move-construct | 1M | 43.69 ms |
| Move-assign | 10K | 385.23 us |
| Move-assign | 100K | 3.82 ms |
| Move-assign | 1M | 38.11 ms |

## Answer Count Growth

| Test | Iteration | DnsPro |
|---|---|---|
| 4 Answer Records | 10K | 22.42 ms |
| 4 Answer Records | 100K | 239.14 ms |
| 4 Answer Records | 1M | 2.51 s |
| 16 Answer Records | 10K | 70.59 ms |
| 16 Answer Records | 100K | 845.41 ms |
| 16 Answer Records | 1M | 7.25 s |
| 64 Answer Records | 10K | 286.58 ms |
| 64 Answer Records | 100K | 2.97 s |
| 64 Answer Records | 1M | 69.21 s |
| 4 Answer Records | 10K | 20.17 ms |
| 4 Answer Records | 100K | 175.58 ms |
| 4 Answer Records | 1M | 3.26 s |
| 16 Answer Records | 10K | 43.94 ms |
| 16 Answer Records | 100K | 403.31 ms |
| 16 Answer Records | 1M | 3.76 s |
| 64 Answer Records | 10K | 114.78 ms |
| 64 Answer Records | 100K | 1.78 s |
| 64 Answer Records | 1M | 13.92 s |

## Label Depth Growth

| Test | Iteration | DnsPro |
|---|---|---|
| 2 Labels Deep | 10K | 9.12 ms |
| 2 Labels Deep | 100K | 92.08 ms |
| 2 Labels Deep | 1M | 495.69 ms |
| 8 Labels Deep | 10K | 5.67 ms |
| 8 Labels Deep | 100K | 56.68 ms |
| 8 Labels Deep | 1M | 596.54 ms |
| 32 Labels Deep | 10K | 14.52 ms |
| 32 Labels Deep | 100K | 144.01 ms |
| 32 Labels Deep | 1M | 1.90 s |
| 2 Labels Deep | 10K | 14.53 ms |
| 2 Labels Deep | 100K | 111.52 ms |
| 2 Labels Deep | 1M | 1.39 s |
| 8 Labels Deep | 10K | 24.67 ms |
| 8 Labels Deep | 100K | 160.36 ms |
| 8 Labels Deep | 1M | 1.20 s |
| 32 Labels Deep | 10K | 22.36 ms |
| 32 Labels Deep | 100K | 223.16 ms |
| 32 Labels Deep | 1M | 2.26 s |

## Zone Size Growth

| Test | Iteration | DnsPro |
|---|---|---|
| 100 Names Stored | 10K | 13.99 ms |
| 100 Names Stored | 100K | 105.92 ms |
| 100 Names Stored | 1M | 717.41 ms |
| 1,000 Names Stored | 10K | 7.28 ms |
| 1,000 Names Stored | 100K | 72.54 ms |
| 1,000 Names Stored | 1M | 740.67 ms |
| 10,000 Names Stored | 10K | 7.29 ms |
| 10,000 Names Stored | 100K | 72.24 ms |
| 10,000 Names Stored | 1M | 717.68 ms |

## Canonicalize

| Test | Iteration | DnsPro |
|---|---|---|
| Mixed-case Name | 10K | 16.88 ms |
| Mixed-case Name | 100K | 114.68 ms |
| Mixed-case Name | 1M | 1.26 s |

## Name Parse

| Test | Iteration | DnsPro |
|---|---|---|
| Uncompressed Name | 10K | 4.63 ms |
| Uncompressed Name | 100K | 45.79 ms |
| Uncompressed Name | 1M | 470.27 ms |
| Name Via Compression Pointer | 10K | 7.13 ms |
| Name Via Compression Pointer | 100K | 72.88 ms |
| Name Via Compression Pointer | 1M | 725.03 ms |
