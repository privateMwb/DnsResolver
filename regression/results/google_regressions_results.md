#PROJECT_NAMERegression Report

## Google Benchmarks

| Benchmark | Current | Baseline | Δ |
|---|---|---|---|
| Lookup_ExistingNameType | 1283.25 ns | 134.816 ns | -89.5% |
| Lookup_MissingName | 221.087 ns | 60.3935 ns | -72.7% |
| Lookup_ExistingNameMissingType | 404.136 ns | 70.3982 ns | -82.6% |
| Build_QuestionOnly | 1362.07 ns | 121.621 ns | -91.1% |
| Build_FourAnswerRecords | 3769.24 ns | 398.559 ns | -89.4% |
| Parse_QuestionOnly | 1056.48 ns | 62.3937 ns | -94.1% |
| Parse_FourAnswerRecords | 67321.9 ns | 301.212 ns | -99.6% |
| AddRecord_ExistingBucket | 2314.98 ns | 289.245 ns | -87.5% |
| RemoveRecord_MissingName | 330.819 ns | 58.0601 ns | -82.4% |
| Resolve_AnswerFound | 5556.08 ns | 446.334 ns | -92.0% |
| Resolve_NXDOMAIN | 3928.28 ns | 354.937 ns | -91.0% |
| Resolve_NODATA | 4060.91 ns | 368.888 ns | -90.9% |
| Message_MoveConstruct | 67.5668 ns | 7.03845 ns | -89.6% |
| Message_MoveAssign | 72.5856 ns | 6.6823 ns | -90.8% |
| ParseAt/FourAnswerRecords | 5294.67 ns | 303.022 ns | -94.3% |
| ParseAt/SixteenAnswerRecords | 17202.4 ns | 1756.72 ns | -89.8% |
| ParseAt/SixtyFourAnswerRecords | 66771.3 ns | 6194 ns | -90.7% |
| BuildAt/FourAnswerRecords | 6015.91 ns | 397.748 ns | -93.4% |
| BuildAt/SixteenAnswerRecords | 4150.79 ns | 1122 ns | -73.0% |
| BuildAt/SixtyFourAnswerRecords | 12198.8 ns | 4088.26 ns | -66.5% |
| ParseAt/TwoLabelsDeep | 725.143 ns | 62.8943 ns | -91.3% |
| ParseAt/EightLabelsDeep | 625.621 ns | 110.601 ns | -82.3% |
| ParseAt/ThirtyTwoLabelsDeep | 1874.24 ns | 399.242 ns | -78.7% |
| BuildAt/TwoLabelsDeep | 642.921 ns | 120.491 ns | -81.3% |
| BuildAt/EightLabelsDeep | 1064.02 ns | 188.485 ns | -82.3% |
| BuildAt/ThirtyTwoLabelsDeep | 3725.37 ns | 408.266 ns | -89.0% |
| LookupAt/Names100 | 1094.4 ns | 143.315 ns | -86.9% |
| LookupAt/Names1000 | 888.213 ns | 142.556 ns | -84.0% |
| LookupAt/Names10000 | 880.978 ns | 143.002 ns | -83.8% |
| Canonicalize_MixedCaseName | 1223.21 ns | 251.538 ns | -79.4% |
| NameParse_Uncompressed | 451.074 ns | 62.2685 ns | -86.2% |
| NameParse_Compressed | 725.082 ns | 97.8783 ns | -86.5% |
