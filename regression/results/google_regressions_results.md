#PROJECT_NAMERegression Report

## Google Benchmarks

| Benchmark | Current | Baseline | Δ |
|---|---|---|---|
| Lookup_ExistingNameType | 997.555 ns | 131.337 ns | -86.8% |
| Lookup_MissingName | 123.722 ns | 59.5642 ns | -51.9% |
| Lookup_ExistingNameMissingType | 169.609 ns | 71.2176 ns | -58.0% |
| Build_QuestionOnly | 637.213 ns | 129.91 ns | -79.6% |
| Build_FourAnswerRecords | 1997.49 ns | 437.891 ns | -78.1% |
| Parse_QuestionOnly | 474.53 ns | 62.5834 ns | -86.8% |
| Parse_FourAnswerRecords | 2482.83 ns | 346.866 ns | -86.0% |
| AddRecord_ExistingBucket | 1163.46 ns | 285.973 ns | -75.4% |
| RemoveRecord_MissingName | 116.532 ns | 56.778 ns | -51.3% |
| Resolve_AnswerFound | 3303.08 ns | 487.693 ns | -85.2% |
| Resolve_NXDOMAIN | 1584.62 ns | 391.765 ns | -75.3% |
| Resolve_NODATA | 1849.59 ns | 402.015 ns | -78.3% |
| Message_MoveConstruct | 41.4212 ns | 6.23786 ns | -84.9% |
| Message_MoveAssign | 38.2982 ns | 5.92231 ns | -84.5% |
| ParseAt/FourAnswerRecords | 2832.48 ns | 327.721 ns | -88.4% |
| ParseAt/SixteenAnswerRecords | 7122.65 ns | 2044.2 ns | -71.3% |
| ParseAt/SixtyFourAnswerRecords | 27806.4 ns | 6715.49 ns | -75.8% |
| BuildAt/FourAnswerRecords | 2490.59 ns | 440.394 ns | -82.3% |
| BuildAt/SixteenAnswerRecords | 3576.87 ns | 1238.41 ns | -65.4% |
| BuildAt/SixtyFourAnswerRecords | 12176.1 ns | 4293.48 ns | -64.7% |
| ParseAt/TwoLabelsDeep | 1108.69 ns | 66.3312 ns | -94.0% |
| ParseAt/EightLabelsDeep | 907.536 ns | 122.837 ns | -86.5% |
| ParseAt/ThirtyTwoLabelsDeep | 1878.8 ns | 435.619 ns | -76.8% |
| BuildAt/TwoLabelsDeep | 809.613 ns | 127.901 ns | -84.2% |
| BuildAt/EightLabelsDeep | 1373.72 ns | 256.037 ns | -81.4% |
| BuildAt/ThirtyTwoLabelsDeep | 2510.4 ns | 710.523 ns | -71.7% |
| LookupAt/Names100 | 994.841 ns | 137.538 ns | -86.2% |
| LookupAt/Names1000 | 849.246 ns | 138.452 ns | -83.7% |
| LookupAt/Names10000 | 839.025 ns | 146.019 ns | -82.6% |
| Canonicalize_MixedCaseName | 1182.96 ns | 235.29 ns | -80.1% |
| NameParse_Uncompressed | 839.632 ns | 63.8993 ns | -92.4% |
| NameParse_Compressed | 1287.93 ns | 99.5857 ns | -92.3% |
