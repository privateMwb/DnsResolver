#DnsProRegression Report

## Parse

| Benchmark | Current | v1.0.0 | Δ |
|---|---|---|---|
| Parse_QuestionOnly | 525 ns | 525 ns | +0.0% |
| Parse_FourAnswerRecords | 2.43 us | 2.43 us | +0.0% |

## ParseAt

| Benchmark | Current | v1.0.0 | Δ |
|---|---|---|---|
| ParseAt/ParseAt_FourAnswerRecords | 4.71 us | 4.71 us | +0.0% |
| ParseAt/ParseAt_SixteenAnswerRecords | 15.57 us | 15.57 us | +0.0% |
| ParseAt/ParseAt_SixtyFourAnswerRecords | 60.23 us | 60.23 us | +0.0% |
| ParseAt/ParseAt_TwoLabelsDeep | 954 ns | 954 ns | +0.0% |
| ParseAt/ParseAt_EightLabelsDeep | 1.16 us | 1.16 us | +0.0% |
| ParseAt/ParseAt_ThirtyTwoLabelsDeep | 3.04 us | 3.04 us | +0.0% |

## Summary

| Result | Count |
|---|---|
| Current faster | 0 (0%) |
| v1.0.0 faster | 0 (0%) |
| Tie | 8 (100%) |
