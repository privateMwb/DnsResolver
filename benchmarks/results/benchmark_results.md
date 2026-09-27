# DnsProBenchmark Results

## Lookup

| Test | Iteration | DnsPro |
|---|---|---|
| Exist Name Type | 10K | 6.56 ms |
| Exist Name Type | 100K | 117.91 ms |
| Exist Name Type | 1M | 1.17 s |
| Missing Name | 10K | 1.76 ms |
| Missing Name | 100K | 17.60 ms |
| Missing Name | 1M | 175.69 ms |
| Exist Name Missing Type | 10K | 2.18 ms |
| Exist Name Missing Type | 100K | 21.87 ms |
| Exist Name Missing Type | 1M | 218.52 ms |

## Build

| Test | Iteration | DnsPro |
|---|---|---|
| Build Query(single Q) | 10K | 11.20 ms |
| Build Query(single Q) | 100K | 102.03 ms |
| Build Query(single Q) | 1M | 1.26 s |
| Build Response(4 Ans Rec) | 10K | 30.94 ms |
| Build Response(4 Ans Rec) | 100K | 282.65 ms |
| Build Response(4 Ans Rec) | 1M | 2.97 s |

## Parse

| Test | Iteration | DnsPro |
|---|---|---|
| Parse Query(single Q) | 10K | 8.12 ms |
| Parse Query(single Q) | 100K | 78.52 ms |
| Parse Query(single Q) | 1M | 789.02 ms |
| Parse Response(4 Ans Rec) | 10K | 39.79 ms |
| Parse Response(4 Ans Rec) | 100K | 420.99 ms |
| Parse Response(4 Ans Rec) | 1M | 4.11 s |

## Record

| Test | Iteration | DnsPro |
|---|---|---|
| Existing Name+type Bucket | 10K | 11.40 ms |
| Existing Name+type Bucket | 100K | 109.75 ms |
| Existing Name+type Bucket | 1M | 2.04 s |
| Missing Name | 10K | 2.68 ms |
| Missing Name | 100K | 26.93 ms |
| Missing Name | 1M | 270.50 ms |

## Resolve

| Test | Iteration | DnsPro |
|---|---|---|
| Answer Found | 10K | 47.73 ms |
| Answer Found | 100K | 469.38 ms |
| Answer Found | 1M | 4.73 s |
| NXDOMAIN | 10K | 32.38 ms |
| NXDOMAIN | 100K | 323.29 ms |
| NXDOMAIN | 1M | 3.24 s |
| NODATA | 10K | 33.52 ms |
| NODATA | 100K | 335.00 ms |
| NODATA | 1M | 3.36 s |

## Message Move

| Test | Iteration | DnsPro |
|---|---|---|
| Move-construct | 10K | 619.08 us |
| Move-construct | 100K | 6.24 ms |
| Move-construct | 1M | 62.05 ms |
| Move-assign | 10K | 631.08 us |
| Move-assign | 100K | 5.89 ms |
| Move-assign | 1M | 58.70 ms |

## Answer Count Growth

| Test | Iteration | DnsPro |
|---|---|---|
| 4 Answer Records | 10K | 42.00 ms |
| 4 Answer Records | 100K | 419.55 ms |
| 4 Answer Records | 1M | 4.21 s |
| 16 Answer Records | 10K | 133.42 ms |
| 16 Answer Records | 100K | 1.34 s |
| 16 Answer Records | 1M | 13.55 s |
| 64 Answer Records | 10K | 260.81 ms |
| 64 Answer Records | 100K | 3.02 s |
| 64 Answer Records | 1M | 44.09 s |
| 4 Answer Records | 10K | 27.93 ms |
| 4 Answer Records | 100K | 315.33 ms |
| 4 Answer Records | 1M | 3.20 s |
| 16 Answer Records | 10K | 60.60 ms |
| 16 Answer Records | 100K | 626.10 ms |
| 16 Answer Records | 1M | 5.47 s |
| 64 Answer Records | 10K | 120.01 ms |
| 64 Answer Records | 100K | 1.31 s |
| 64 Answer Records | 1M | 12.15 s |

## Label Depth Growth

| Test | Iteration | DnsPro |
|---|---|---|
| 2 Labels Deep | 10K | 5.89 ms |
| 2 Labels Deep | 100K | 82.97 ms |
| 2 Labels Deep | 1M | 779.88 ms |
| 8 Labels Deep | 10K | 8.30 ms |
| 8 Labels Deep | 100K | 76.19 ms |
| 8 Labels Deep | 1M | 880.43 ms |
| 32 Labels Deep | 10K | 22.02 ms |
| 32 Labels Deep | 100K | 211.35 ms |
| 32 Labels Deep | 1M | 2.08 s |
| 2 Labels Deep | 10K | 8.41 ms |
| 2 Labels Deep | 100K | 75.63 ms |
| 2 Labels Deep | 1M | 1.05 s |
| 8 Labels Deep | 10K | 9.97 ms |
| 8 Labels Deep | 100K | 124.06 ms |
| 8 Labels Deep | 1M | 1.73 s |
| 32 Labels Deep | 10K | 36.25 ms |
| 32 Labels Deep | 100K | 359.13 ms |
| 32 Labels Deep | 1M | 3.57 s |

## Zone Size Growth

| Test | Iteration | DnsPro |
|---|---|---|
| 100 Names Stored | 10K | 9.56 ms |
| 100 Names Stored | 100K | 125.06 ms |
| 100 Names Stored | 1M | 1.24 s |
| 1,000 Names Stored | 10K | 12.64 ms |
| 1,000 Names Stored | 100K | 125.73 ms |
| 1,000 Names Stored | 1M | 1.18 s |
| 10,000 Names Stored | 10K | 11.35 ms |
| 10,000 Names Stored | 100K | 120.66 ms |
| 10,000 Names Stored | 1M | 1.18 s |

## Canonicalize

| Test | Iteration | DnsPro |
|---|---|---|
| Mixed-case Name | 10K | 14.19 ms |
| Mixed-case Name | 100K | 220.57 ms |
| Mixed-case Name | 1M | 1.59 s |

## Name Parse

| Test | Iteration | DnsPro |
|---|---|---|
| Uncompressed Name | 10K | 6.74 ms |
| Uncompressed Name | 100K | 81.62 ms |
| Uncompressed Name | 1M | 781.52 ms |
| Compression Pointer Name | 10K | 8.86 ms |
| Compression Pointer Name | 100K | 126.86 ms |
| Compression Pointer Name | 1M | 1.21 s |
