#PROJECT_NAMERegression Report

## Lookup

| Test | Iteration | Current | Baseline | Δ |
|---|---|---|---|---|
| Existing name+type | 10K | 643.67 ns | 138.01 ns | -78.6% |
| Existing name+type | 100K | 836.78 ns | 138.92 ns | -83.4% |
| Existing name+type | 1M | 679.75 ns | 141.91 ns | -79.1% |
| Missing name | 10K | 175.69 ns | 65.06 ns | -63.0% |
| Missing name | 100K | 175.84 ns | 58.9 ns | -66.5% |
| Missing name | 1M | 175.52 ns | 59.96 ns | -65.8% |
| Existing name, missing type | 10K | 221.69 ns | 70.74 ns | -68.1% |
| Existing name, missing type | 100K | 218.68 ns | 72.71 ns | -66.8% |
| Existing name, missing type | 1M | 218.92 ns | 72.04 ns | -67.1% |

## Build

| Test | Iteration | Current | Baseline | Δ |
|---|---|---|---|---|
| Query, single question | 10K | 1121.35 ns | 130.06 ns | -88.4% |
| Query, single question | 100K | 580.15 ns | 129.76 ns | -77.6% |
| Query, single question | 1M | 505.47 ns | 129.88 ns | -74.3% |
| Response, 4 answer records | 10K | 1435.14 ns | 440.61 ns | -69.3% |
| Response, 4 answer records | 100K | 1449.89 ns | 442.8 ns | -69.5% |
| Response, 4 answer records | 1M | 2619.87 ns | 441.45 ns | -83.1% |

## Parse

| Test | Iteration | Current | Baseline | Δ |
|---|---|---|---|---|
| Query, single question | 10K | 1121.35 ns | 73.25 ns | -93.5% |
| Query, single question | 100K | 580.15 ns | 62.76 ns | -89.2% |
| Query, single question | 1M | 505.47 ns | 65.75 ns | -87.0% |
| Response, 4 answer records | 10K | 1435.14 ns | 333.58 ns | -76.8% |
| Response, 4 answer records | 100K | 1449.89 ns | 339.08 ns | -76.6% |
| Response, 4 answer records | 1M | 2619.87 ns | 347.05 ns | -86.8% |

## Record

| Test | Iteration | Current | Baseline | Δ |
|---|---|---|---|---|
| Existing name+type bucket | 10K | 2782.85 ns | 374.12 ns | -86.6% |
| Existing name+type bucket | 100K | 2247.69 ns | 329.22 ns | -85.4% |
| Existing name+type bucket | 1M | 2272.32 ns | 324.18 ns | -85.7% |
| Missing name | 10K | 175.69 ns | 56.12 ns | -68.1% |
| Missing name | 100K | 175.84 ns | 57.46 ns | -67.3% |
| Missing name | 1M | 175.52 ns | 56.66 ns | -67.7% |

## Resolve

| Test | Iteration | Current | Baseline | Δ |
|---|---|---|---|---|
| Answer found | 10K | 4661.63 ns | 484.84 ns | -89.6% |
| Answer found | 100K | 4647.68 ns | 483.41 ns | -89.6% |
| Answer found | 1M | 4138.72 ns | 484.15 ns | -88.3% |
| NXDOMAIN | 10K | 1681.84 ns | 397.46 ns | -76.4% |
| NXDOMAIN | 100K | 2130.63 ns | 396.53 ns | -81.4% |
| NXDOMAIN | 1M | 2571.07 ns | 394.94 ns | -84.6% |
| NODATA | 10K | 1837.91 ns | 402 ns | -78.1% |
| NODATA | 100K | 2360.29 ns | 395.62 ns | -83.2% |
| NODATA | 1M | 2122.35 ns | 398.7 ns | -81.2% |

## Message Move

| Test | Iteration | Current | Baseline | Δ |
|---|---|---|---|---|
| Move-construct | 10K | 106.65 ns | 9.57 ns | -91.0% |
| Move-construct | 100K | 75.28 ns | 9.38 ns | -87.5% |
| Move-construct | 1M | 64.99 ns | 5.39 ns | -91.7% |
| Move-assign | 10K | 59.3 ns | 5.95 ns | -90.0% |
| Move-assign | 100K | 60.53 ns | 5.91 ns | -90.2% |
| Move-assign | 1M | 57.14 ns | 5.85 ns | -89.8% |

## Answer Count Growth

| Test | Iteration | Current | Baseline | Δ |
|---|---|---|---|---|
| 4 answer records | 10K | 2031.65 ns | 315.78 ns | -84.5% |
| 4 answer records | 100K | 2471 ns | 312.72 ns | -87.3% |
| 4 answer records | 1M | 2250.11 ns | 315.45 ns | -86.0% |
| 16 answer records | 10K | 7002.89 ns | 1220.01 ns | -82.6% |
| 16 answer records | 100K | 8229.37 ns | 1222.81 ns | -85.1% |
| 16 answer records | 1M | 11916.8 ns | 1219.16 ns | -89.8% |
| 64 answer records | 10K | 56819.4 ns | 6107.97 ns | -89.3% |
| 64 answer records | 100K | 50993.3 ns | 6138.3 ns | -88.0% |
| 64 answer records | 1M | 38868.9 ns | 6149.22 ns | -84.2% |
| 4 answer records | 10K | 2031.65 ns | 438.03 ns | -78.4% |
| 4 answer records | 100K | 2471 ns | 436.94 ns | -82.3% |
| 4 answer records | 1M | 2250.11 ns | 436.95 ns | -80.6% |
| 16 answer records | 10K | 7002.89 ns | 1233.42 ns | -82.4% |
| 16 answer records | 100K | 8229.37 ns | 1235.52 ns | -85.0% |
| 16 answer records | 1M | 11916.8 ns | 1235.55 ns | -89.6% |
| 64 answer records | 10K | 56819.4 ns | 4340.63 ns | -92.4% |
| 64 answer records | 100K | 50993.3 ns | 4327.8 ns | -91.5% |
| 64 answer records | 1M | 38868.9 ns | 4316.69 ns | -88.9% |

## Label Depth Growth

| Test | Iteration | Current | Baseline | Δ |
|---|---|---|---|---|
| 2 labels deep | 10K | 868.98 ns | 62.87 ns | -92.8% |
| 2 labels deep | 100K | 895.51 ns | 63.11 ns | -93.0% |
| 2 labels deep | 1M | 896.91 ns | 62.89 ns | -93.0% |
| 8 labels deep | 10K | 1080.68 ns | 107.43 ns | -90.1% |
| 8 labels deep | 100K | 1145.37 ns | 106.68 ns | -90.7% |
| 8 labels deep | 1M | 1071.14 ns | 106.41 ns | -90.1% |
| 32 labels deep | 10K | 2749.64 ns | 372.44 ns | -86.5% |
| 32 labels deep | 100K | 2750.69 ns | 366.99 ns | -86.7% |
| 32 labels deep | 1M | 2903.93 ns | 367.39 ns | -87.3% |
| 2 labels deep | 10K | 868.98 ns | 129.35 ns | -85.1% |
| 2 labels deep | 100K | 895.51 ns | 126.59 ns | -85.9% |
| 2 labels deep | 1M | 896.91 ns | 126.04 ns | -85.9% |
| 8 labels deep | 10K | 1080.68 ns | 252.08 ns | -76.7% |
| 8 labels deep | 100K | 1145.37 ns | 252.12 ns | -78.0% |
| 8 labels deep | 1M | 1071.14 ns | 252.37 ns | -76.4% |
| 32 labels deep | 10K | 2749.64 ns | 721.58 ns | -73.8% |
| 32 labels deep | 100K | 2750.69 ns | 721.48 ns | -73.8% |
| 32 labels deep | 1M | 2903.93 ns | 723.09 ns | -75.1% |

## Zone Size Growth

| Test | Iteration | Current | Baseline | Δ |
|---|---|---|---|---|
| 100 names stored | 10K | 1388.88 ns | 266.73 ns | -80.8% |
| 100 names stored | 100K | 1539.29 ns | 154.14 ns | -90.0% |
| 100 names stored | 1M | 1556.53 ns | 143.71 ns | -90.8% |
| 1,000 names stored | 10K | 1387.55 ns | 143.5 ns | -89.7% |
| 1,000 names stored | 100K | 1416.96 ns | 144.66 ns | -89.8% |
| 1,000 names stored | 1M | 1456.41 ns | 146.83 ns | -89.9% |
| 10,000 names stored | 10K | 1422.4 ns | 144.66 ns | -89.8% |
| 10,000 names stored | 100K | 1419.36 ns | 144.7 ns | -89.8% |
| 10,000 names stored | 1M | 1454.3 ns | 143.64 ns | -90.1% |

## Canonicalize

| Test | Iteration | Current | Baseline | Δ |
|---|---|---|---|---|
| Mixed-case name | 10K | 1904.37 ns | 174.28 ns | -90.8% |
| Mixed-case name | 100K | 2124.57 ns | 202.49 ns | -90.5% |
| Mixed-case name | 1M | 2236.01 ns | 294.89 ns | -86.8% |

## Name Parse

| Test | Iteration | Current | Baseline | Δ |
|---|---|---|---|---|
| Uncompressed name | 10K | 906.22 ns | 67.05 ns | -92.6% |
| Uncompressed name | 100K | 895.46 ns | 68.94 ns | -92.3% |
| Uncompressed name | 1M | 927.2 ns | 71.12 ns | -92.3% |
| Name via compression pointer | 10K | 1317.65 ns | 104.52 ns | -92.1% |
| Name via compression pointer | 100K | 1319.15 ns | 103.53 ns | -92.2% |
| Name via compression pointer | 1M | 1353.53 ns | 102.93 ns | -92.4% |
