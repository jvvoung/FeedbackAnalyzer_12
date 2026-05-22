# Appendix A — HTTP Endpoint Regression Checklist (AC-5)

| ID | Method | Path | 검증 항목 | 상태 |
|----|--------|------|-----------|------|
| HTTP-S-01 | GET | `/` | HTML 200, UTF-8, success alert | Green (수동) |
| HTTP-S-02 | POST | `/analyze` | text 입력 → stat 갱신, 빈/공백 skip (T-08) | Green |
| HTTP-S-03 | POST | `/upload` | CSV `text` 컬럼 필수, 없으면 error alert (T-05/EX-06) | Green |
| HTTP-S-04 | POST | `/filter` | sentiment/keyword 필터, empty/zero warning (T-09/T-10) | Green |
| HTTP-S-05 | GET | `/download` | fil_data 없으면 warning, 있으면 UTF-8 BOM CSV (M-6/AC-7) | Green |

> Phase 1 GREEN: Track B 단위 테스트 14/14 Pass 후 본 체크리스트로 HTTP 스모크 수동 검증.
