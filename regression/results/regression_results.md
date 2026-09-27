#DnsProRegression Report

## Lookup

| Test | Iteration | Current | v1.0.0 | Δ |
|---|---|---|---|---|
| Exist name type | 10K | 1.16 us | 66 ns | -94.3% |
| Exist name type | 100K | 1.25 us | 64 ns | -94.9% |
| Exist name type | 1M | 1.19 us | 62 ns | -94.8% |
| Missing name | 10K | 176 ns | 30 ns | -83.2% |
| Missing name | 100K | 176 ns | 30 ns | -82.9% |
| Missing name | 1M | 176 ns | 31 ns | -82.6% |
| Exist name missing type | 10K | 219 ns | 37 ns | -82.9% |
| Exist name missing type | 100K | 219 ns | 38 ns | -82.9% |
| Exist name missing type | 1M | 219 ns | 37 ns | -83.0% |

## Build

| Test | Iteration | Current | v1.0.0 | Δ |
|---|---|---|---|---|
| Build Query(single q) | 10K | 940 ns | 58 ns | -93.9% |
| Build Query(single q) | 100K | 1.07 us | 59 ns | -94.5% |
| Build Query(single q) | 1M | 974 ns | 60 ns | -93.8% |
| Build Response(4 ans rec) | 10K | 2.20 us | 202 ns | -90.8% |
| Build Response(4 ans rec) | 100K | 2.64 us | 196 ns | -92.6% |
| Build Response(4 ans rec) | 1M | 2.54 us | 194 ns | -92.4% |

## Parse

| Test | Iteration | Current | v1.0.0 | Δ |
|---|---|---|---|---|
| Parse Query(single q) | 10K | 808 ns | 34 ns | -95.8% |
| Parse Query(single q) | 100K | 769 ns | 43 ns | -94.4% |
| Parse Query(single q) | 1M | 755 ns | 32 ns | -95.7% |
| Parse Response(4 ans rec) | 10K | 3.91 us | 174 ns | -95.5% |
| Parse Response(4 ans rec) | 100K | 3.99 us | 183 ns | -95.4% |
| Parse Response(4 ans rec) | 1M | 4.39 us | 174 ns | -96.0% |

## Record

| Test | Iteration | Current | v1.0.0 | Δ |
|---|---|---|---|---|
| Existing name+type bucket | 10K | 2.29 us | 239 ns | -89.6% |
| Existing name+type bucket | 100K | 2.64 us | 199 ns | -92.5% |
| Existing name+type bucket | 1M | 11.15 us | 236 ns | -97.9% |
| Missing name | 10K | 176 ns | 35 ns | -79.8% |
| Missing name | 100K | 176 ns | 36 ns | -79.4% |
| Missing name | 1M | 176 ns | 35 ns | -80.0% |

## Resolve

| Test | Iteration | Current | v1.0.0 | Δ |
|---|---|---|---|---|
| Answer found | 10K | 4.60 us | 241 ns | -94.8% |
| Answer found | 100K | 4.64 us | 245 ns | -94.7% |
| Answer found | 1M | 5.15 us | 234 ns | -95.5% |
| NXDOMAIN | 10K | 3.37 us | 180 ns | -94.7% |
| NXDOMAIN | 100K | 3.34 us | 182 ns | -94.6% |
| NXDOMAIN | 1M | 3.30 us | 185 ns | -94.4% |
| NODATA | 10K | 3.35 us | 189 ns | -94.3% |
| NODATA | 100K | 3.36 us | 195 ns | -94.2% |
| NODATA | 1M | 3.42 us | 193 ns | -94.4% |

## Message Move

| Test | Iteration | Current | v1.0.0 | Δ |
|---|---|---|---|---|
| Move-construct | 10K | 62 ns | 2 ns | -96.0% |
| Move-construct | 100K | 71 ns | 3 ns | -96.2% |
| Move-construct | 1M | 63 ns | 2 ns | -96.1% |
| Move-assign | 10K | 57 ns | 3 ns | -94.6% |
| Move-assign | 100K | 57 ns | 3 ns | -95.0% |
| Move-assign | 1M | 59 ns | 3 ns | -95.1% |

## Answer Count Growth

| Test | Iteration | Current | v1.0.0 | Δ |
|---|---|---|---|---|
| 4 answer records | 10K | 4.24 us | 181 ns | -95.7% |
| 4 answer records | 100K | 4.24 us | 180 ns | -95.8% |
| 4 answer records | 1M | 4.32 us | 177 ns | -95.9% |
| 16 answer records | 10K | 15.10 us | 666 ns | -95.6% |
| 16 answer records | 100K | 14.32 us | 650 ns | -95.5% |
| 16 answer records | 1M | 13.91 us | 642 ns | -95.4% |
| 64 answer records | 10K | 58.28 us | 3.28 us | -94.4% |
| 64 answer records | 100K | 55.20 us | 3.30 us | -94.0% |
| 64 answer records | 1M | 62.10 us | 3.30 us | -94.7% |
| 4 answer records | 10K | 4.24 us | 190 ns | -95.5% |
| 4 answer records | 100K | 4.24 us | 206 ns | -95.1% |
| 4 answer records | 1M | 4.32 us | 210 ns | -95.1% |
| 16 answer records | 10K | 15.10 us | 534 ns | -96.5% |
| 16 answer records | 100K | 14.32 us | 548 ns | -96.2% |
| 16 answer records | 1M | 13.91 us | 554 ns | -96.0% |
| 64 answer records | 10K | 58.28 us | 1.96 us | -96.6% |
| 64 answer records | 100K | 55.20 us | 1.88 us | -96.6% |
| 64 answer records | 1M | 62.10 us | 1.91 us | -96.9% |

## Label Depth Growth

| Test | Iteration | Current | v1.0.0 | Δ |
|---|---|---|---|---|
| 2 labels deep | 10K | 962 ns | 32 ns | -96.7% |
| 2 labels deep | 100K | 966 ns | 32 ns | -96.7% |
| 2 labels deep | 1M | 966 ns | 32 ns | -96.7% |
| 8 labels deep | 10K | 1.17 us | 58 ns | -95.0% |
| 8 labels deep | 100K | 1.20 us | 58 ns | -95.2% |
| 8 labels deep | 1M | 1.18 us | 58 ns | -95.1% |
| 32 labels deep | 10K | 3.08 us | 209 ns | -93.2% |
| 32 labels deep | 100K | 3.04 us | 210 ns | -93.1% |
| 32 labels deep | 1M | 3.06 us | 216 ns | -93.0% |
| 2 labels deep | 10K | 962 ns | 105 ns | -89.0% |
| 2 labels deep | 100K | 966 ns | 62 ns | -93.6% |
| 2 labels deep | 1M | 966 ns | 58 ns | -94.0% |
| 8 labels deep | 10K | 1.17 us | 93 ns | -92.0% |
| 8 labels deep | 100K | 1.20 us | 93 ns | -92.2% |
| 8 labels deep | 1M | 1.18 us | 93 ns | -92.1% |
| 32 labels deep | 10K | 3.08 us | 200 ns | -93.5% |
| 32 labels deep | 100K | 3.04 us | 206 ns | -93.2% |
| 32 labels deep | 1M | 3.06 us | 200 ns | -93.5% |

## Zone Size Growth

| Test | Iteration | Current | v1.0.0 | Δ |
|---|---|---|---|---|
| 100 names stored | 10K | 1.71 us | 63 ns | -96.3% |
| 100 names stored | 100K | 1.60 us | 64 ns | -96.0% |
| 100 names stored | 1M | 1.60 us | 64 ns | -96.0% |
| 1,000 names stored | 10K | 1.55 us | 64 ns | -95.9% |
| 1,000 names stored | 100K | 1.59 us | 64 ns | -96.0% |
| 1,000 names stored | 1M | 1.57 us | 64 ns | -96.0% |
| 10,000 names stored | 10K | 1.59 us | 67 ns | -95.8% |
| 10,000 names stored | 100K | 1.58 us | 67 ns | -95.8% |
| 10,000 names stored | 1M | 1.55 us | 64 ns | -95.9% |

## Canonicalize

| Test | Iteration | Current | v1.0.0 | Δ |
|---|---|---|---|---|
| Mixed-case name | 10K | 2.06 us | 88 ns | -95.7% |
| Mixed-case name | 100K | 2.30 us | 164 ns | -92.9% |
| Mixed-case name | 1M | 2.44 us | 190 ns | -92.2% |

## Name Parse

| Test | Iteration | Current | v1.0.0 | Δ |
|---|---|---|---|---|
| Uncompressed name | 10K | 966 ns | 35 ns | -96.4% |
| Uncompressed name | 100K | 957 ns | 34 ns | -96.4% |
| Uncompressed name | 1M | 964 ns | 35 ns | -96.4% |
| Compression pointer name | 10K | 1.46 us | 106 ns | -92.8% |
| Compression pointer name | 100K | 1.45 us | 76 ns | -94.7% |
| Compression pointer name | 1M | 1.48 us | 54 ns | -96.4% |

## Summary

| Result | Count |
|---|---|
| Current faster | 0 (0%) |
| v1.0.0 faster | 96 (100%) |
| Tie | 0 (0%) |
