# HTTP / Boundary RED Tests (Track A)

| 항목 | 내용 |
|------|------|
| Phase | 1 RED only — 수동·스모크 검증 스펙 (GTest 미포함) |
| 레거시 | `src/cpp/main.cpp` 등 **수정 금지** — As-Is 동작 기준 |
| 검증 | 서버 기동 후 `curl`/브라우저 또는 Phase 2+ in-process 스모크 |
| 근거 | `docs/test_plan.md` §4.2, `docs/PRD.md` §3.2·부록 A, `docs/analysis.md` §6 |

---

## RED 스펙 테이블

| Test ID | Given | When | Then | Invariant (PRD) | Expected RED |
|---------|-------|------|------|-----------------|----------------|
| **T-02** | Session 비어 있음 또는 기존 N건 | `POST /analyze` `Content-Type: application/x-www-form-urlencoded`, `text=배송이 너무 늦어요. 화가 납니다.` | HTML stat: 감정 **부정 +1**, 키워드 **배송 +1**; success alert `{N}개의 피드백이 입력되었습니다.` | F-02, F-06 | **RED (As-Is):** `Constants::SENTIMENT_KEYWORDS["부정"]`에 `화가`/`늦` 미포함 → 집계 **중립 +1** 가능 (DEF-009). **To-Be:** PRD 예시대로 부정 +1 |
| **T-08** | Session에 기존 N건 | `POST /analyze`, `text=` 또는 `text=%20%20` (공백만) | 피드백 **미추가**, 건수 **N 유지**; 집계는 기존 Session 기준 | F-02 (빈·공백 미추가) | Green (As-Is trim 로직 일치) |
| **T-09** | `Session::getCurrentFeedbacks()` **비어 있음** | `POST /filter` `sentiment=전체&keyword=전체` | HTML warning: `분석할 피드백이 없습니다.`; HTTP **200** | F-04 | Green |
| **T-10** | Session에 1건 이상, 필터 조건과 **불일치** (예: sentiment=긍정, 본문은 부정 키워드만) | `POST /filter` | HTML warning: `필터링 결과가 없습니다.`; HTTP **200** | F-04 | Green |
| **EX-04** / **M-6** | `POST /filter` **미실행** 또는 마지막 필터 0건 (`fil_data` 비어 있음) | `GET /download` | Response `200`, `Content-Type: text/csv; charset=UTF-8`, Body: BOM `\xEF\xBB\xBF` + `text\n` **헤더만**, 데이터 행 **0건** | F-05, AC-5 | Green (As-Is). **To-Be (Phase 4):** warning `다운로드할 필터 결과가 없습니다.` |
| **T-05** / **EX-06** | multipart CSV 본문 `id,comment\n1,hello` (`text` 컬럼 없음) | `POST /upload` field `file` | HTML error: `파일 업로드 중 오류가 발생했습니다.` (warning `CSV에 text 컬럼이 없습니다.` 대안은 error 우선) | F-03, AC-2, H-2 | **RED (As-Is):** `fields[0]`(`1`)을 Feedback text로 적재·success alert — H-2 |
| **T-11** / **AC-7** | textarea `text=첫 줄%0A두 번째 줄` (URL-encoded 개행) | `POST /analyze` → `POST /filter` (전체/전체) → `GET /download` | Session·필터 목록·CSV 본문에 **리터럴 `\n` 유지** (한 셀/한 행 내 개행) | F-02, F-04, F-05, AC-7 | Green (입력·Feedback 모델). **RED (다운로드):** 필터 전 `/download` 시 0건 (EX-04) |
| **HTTP-S-01** | 서버 `http://localhost:8080` 기동 | `GET /` | `200 OK`, `text/html; charset=UTF-8`, UTF-8 한국어 UI (제목·섹션·폼 action 경로) | F-01, AC-5 | Green |
| **HTTP-S-02** | Session 0건 | `POST /analyze` 유효 `text` 1건 | 감정 3분류·키워드 5카테고리 stat 표시 | F-02, AC-5 | T-02와 연동 시 **RED** 가능 |
| **HTTP-S-03** | CSV `text\n{본문}\n` | `POST /upload` multipart `file` | `text` 컬럼 값만 Session 적재 | F-03, AC-2, AC-5 | T-05 케이스 **RED** |
| **HTTP-S-04** | Session ≥1건 | `POST /filter` `sentiment=전체\|긍정\|중립\|부정`, `keyword=전체\|배송\|품질\|가격\|서비스\|사용성` | 조건 일치 시 집계·목록·다운로드 버튼; 0건 시 T-10 warning | F-04, AC-5 | sentiment=**중립** 시 **RED** (H-1, HTTP-S-06) |
| **HTTP-S-05** | `POST /filter` 성공(≥1건) 직후 | `GET /download` | BOM + `text\n` + 필터 결과 본문; `Content-Disposition: attachment; filename="filtered_feedback.csv"` | F-05, AC-5 | Green (필터 후) |

---

## 부록 — 수동 검증 스크립트 예시 (PowerShell)

```powershell
# T-09
Invoke-WebRequest -Uri http://localhost:8080/filter -Method POST -Body "sentiment=전체&keyword=전체" -ContentType "application/x-www-form-urlencoded"

# T-02
Invoke-WebRequest -Uri http://localhost:8080/analyze -Method POST -Body ([System.Web.HttpUtility]::UrlEncode("text=배송이 너무 늦어요. 화가 납니다.")) -ContentType "application/x-www-form-urlencoded"
```

---

## P0 HTTP 보조 (test_plan §9)

| ID | Given | When | Then | Expected RED |
|----|-------|------|------|--------------|
| **HTTP-S-06** | Session: `그냥 그래요. 특별한 감정 없음.` 1건 | `POST /filter` `sentiment=중립&keyword=전체` | stat에 해당 1건 | **RED** — H-1 (S_KEYWORDS 중립 vs Analyzer 기본 중립) |
| **HTTP-S-07** | Session: `택배가 빨라요.` 1건 | `POST /filter` `sentiment=전체&keyword=배송` | stat에 1건 | As-Is **Green** (sub `type`에 `택배`). **H-3 RED**는 `keyword=품질` + `품질이 좋아요.` (main-only) |
