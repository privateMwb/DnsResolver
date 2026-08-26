#PROJECT_NAMERegression Report

## Google Benchmarks

| Benchmark | Current | Baseline | Δ |
|---|---|---|---|
| Lookup_ExistingNameType | 997.555 ns | 134.816 ns | -86.5% |
| Lookup_MissingName | 123.722 ns | 60.3935 ns | -51.2% |
| Lookup_ExistingNameMissingType | 169.609 ns | 70.3982 ns | -58.5% |
| Build_QuestionOnly | 637.213 ns | 121.621 ns | -80.9% |
| Build_FourAnswerRecords | 1997.49 ns | 398.559 ns | -80.0% |
| Parse_QuestionOnly | 474.53 ns | 62.3937 ns | -86.9% |
| Parse_FourAnswerRecords | 2482.83 ns | 301.212 ns | -87.9% |
| AddRecord_ExistingBucket | 1163.46 ns | 289.245 ns | -75.1% |
| RemoveRecord_MissingName | 116.532 ns | 58.0601 ns | -50.2% |
| Resolve_AnswerFound | 3303.08 ns | 446.334 ns | -86.5% |
| Resolve_NXDOMAIN | 1584.62 ns | 354.937 ns | -77.6% |
| Resolve_NODATA | 1849.59 ns | 368.888 ns | -80.1% |
| Message_MoveConstruct | 41.4212 ns | 7.03845 ns | -83.0% |
| Message_MoveAssign | 38.2982 ns | 6.6823 ns | -82.6% |
| ParseAt/FourAnswerRecords | 2832.48 ns | 303.022 ns | -89.3% |
| ParseAt/SixteenAnswerRecords | 7122.65 ns | 1756.72 ns | -75.3% |
| ParseAt/SixtyFourAnswerRecords | 27806.4 ns | 6194 ns | -77.7% |
| BuildAt/FourAnswerRecords | 2490.59 ns | 397.748 ns | -84.0% |
| BuildAt/SixteenAnswerRecords | 3576.87 ns | 1122 ns | -68.6% |
| BuildAt/SixtyFourAnswerRecords | 12176.1 ns | 4088.26 ns | -66.4% |
| ParseAt/TwoLabelsDeep | 1108.69 ns | 62.8943 ns | -94.3% |
| ParseAt/EightLabelsDeep | 907.536 ns | 110.601 ns | -87.8% |
| ParseAt/ThirtyTwoLabelsDeep | 1878.8 ns | 399.242 ns | -78.8% |
| BuildAt/TwoLabelsDeep | 809.613 ns | 120.491 ns | -85.1% |
| BuildAt/EightLabelsDeep | 1373.72 ns | 188.485 ns | -86.3% |
| BuildAt/ThirtyTwoLabelsDeep | 2510.4 ns | 408.266 ns | -83.7% |
| LookupAt/Names100 | 994.841 ns | 143.315 ns | -85.6% |
| LookupAt/Names1000 | 849.246 ns | 142.556 ns | -83.2% |
| LookupAt/Names10000 | 839.025 ns | 143.002 ns | -83.0% |
| Canonicalize_MixedCaseName | 1182.96 ns | 251.538 ns | -78.7% |
| NameParse_Uncompressed | 839.632 ns | 62.2685 ns | -92.6% |
| NameParse_Compressed | 1287.93 ns | 97.8783 ns | -92.4% |
