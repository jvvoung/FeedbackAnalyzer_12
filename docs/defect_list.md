# Feedback Analyzer — 결함 목록 (Defect List)

| 항목 | 내용 |
|------|------|
| **작성일** | 2026-05-22 |
| **Phase** | Phase 1 — **RED** (failing test 고정 → Green 대기) |
| **근거 문서** | `docs/analysis.md` §6·§7, `docs/test_plan.md` §4~§5, `docs/PRD.md` §7.1 (AC-1~AC-4), `README.md` §RED 단계 To-Do |
| **연결 test_plan ID** | T-03, T-05, T-06 (P0) · T-02, EX-04 (P1) |
| **ctest 스냅샷** | 2026-05-22 — **31/34 Passed**, **3 Failed** (`build/Testing/Temporary/LastTestsFailed.log`) |

---

## 1. P0 결함 (High — 반드시 수정)

| ID | Severity | 기능/레이어 | test_plan ID | Failing Test (RED) | 재현 절차 | 기대값 | 실제값 | 근본 원인 (파일:줄) | 수정 요약 | 상태 |
|----|----------|-------------|--------------|-------------------|-----------|--------|--------|---------------------|-----------|------|
| DEF-001 | High (P0) | Filters / sentiment=중립 | T-03, AC-1, H-1, M-7 | `FiltersTest.Given_MixedFeedbacksIncludingNeutral_When_FilterSentimentNeutral_Then_MatchesTextAnalyzerSet` *(canonical: `Given_NeutralText_When_FilterSentimentNeutral_Then_MatchesTextAnalyzerSet`)* | Session에 Feedback 2건: `"그냥 그래요. 특별한 감정 없음."`, `"와우 정말 대단해요."` → `filters.fil(feedbacks, u8"중립", u8"전체")` | Filters 결과 = TextAnalyzer `중립` 집합 (1건, canonical 텍스트만) | 2건 반환 (`"와우…"` 포함 — Filters `S_KEYWORDS["긍정"]`에 `"와우"` 있음, TextAnalyzer `Constants`에는 없음) | `Filters.cpp:5-20` (`S_KEYWORDS` 별도 정의) · `Filters.h:33-38` (중립=전용 키워드 필요) · `TextAnalyzer.h:29-34` (긍/부 미매칭→중립) | `Constants::SENTIMENT_KEYWORDS` 단일 소스 + TextAnalyzer와 동일 판정 순서(긍→부→중립) | **Open** |
| DEF-002 | High (P0) | CsvParser / POST `/upload` | T-05, AC-2, H-2 | `CsvParserTest.Given_NoTextColumnCsv_When_Parse_Then_ZeroFeedbacks` *(canonical: `Given_CsvWithoutTextColumn_When_Parse_Then_ZeroFeedbacks`)* | CSV `"id,comment\n1,hello\n"` → `CsvParser::parse(csv)` | `text` 컬럼 없음 → 0건 적재 또는 parse 실패; `fields[0]`("1") 사용 **금지** | 1건 적재 (`Feedback("1")`) | `tests/support/CsvParser.cpp:106-110` (text 없을 때 `textIndex=0` fallback) · `main.cpp:303-305` (동일 `fields[0]` 패턴) | 헤더 `text` 인덱스 탐색; 없으면 0건 + error/warning; main 업로드 핸들러도 CsvParser 위임 | **Open** |
| DEF-003 | High (P0) | Filters / keyword=품질·배송 | T-06, AC-3, H-3 | `FiltersTest.Given_QualityMainKeywordOnlyText_When_FilterKeywordQuality_Then_IncludedInResult` *(canonical: `Given_MainKeywordOnly_When_FilterDelivery_Then_Included`)* | Feedback `"품질이 좋아요."` → `filters.fil(feedbacks, u8"전체", u8"품질")` | 1건 포함 (`main` 키워드 `"품질"` 매칭) | 0건 | `Filters.h:55-56` (`if (subEntry.first == "main") continue;`) | `TextAnalyzer::kw`와 동일 — `CATEGORY_KEYWORDS[cat]["main"]` 매칭 포함 | **Open** |
| DEF-004 | High (P0) | 빌드·테스트 인프라 | AC-4, H-4 | — *(인프라 결함; P0 기능 테스트는 DEF-001~003)* | 레거시 As-Is: `CMakeLists.txt`에 `enable_testing()`·GTest 타깃 없음, `tests/` 미존재 | Google Test + `ctest` 타깃, Phase 1 UT 실행 가능 | **RED 단계 해소:** `feedback_analyzer_tests` 34건 등록 (`CMakeLists.txt:42-72`); ctest 실행 가능 | 레거시: 테스트 인프라 전무 (`docs/analysis.md` §2.1.4) | FetchContent GTest, `tests/*`, `gtest_discover_tests` — **완료**; AC-4 Green·커버리지 ≥90%는 H-1~H-3 수정 후 | **Fixed** |

### P0 Failing Test 상세 (ctest 2026-05-22)

| 결함 ID | Failing Test | Failure 요약 |
|---------|--------------|--------------|
| DEF-001 | `FiltersTest.Given_MixedFeedbacksIncludingNeutral_When_FilterSentimentNeutral_Then_MatchesTextAnalyzerSet` | `filtered.size()=2`, `expectedNeutral.size()=1` — 이중 감성 사전 불일치 |
| DEF-002 | `CsvParserTest.Given_NoTextColumnCsv_When_Parse_Then_ZeroFeedbacks` | `result.feedbacks.empty()` Expected true, Actual false |
| DEF-003 | `FiltersTest.Given_QualityMainKeywordOnlyText_When_FilterKeywordQuality_Then_IncludedInResult` | `filtered.size()=0`, Expected 1 |

> **H-3 참고:** `FiltersTest.Given_TaekbaeMainKeywordText_When_FilterKeywordBaeseong_Then_IncludedInResult`("택배가 빨라요.")는 **Pass** — `"택배"`가 `CATEGORY_KEYWORDS["배송"]["type"]` 서브 키워드에도 포함되어 `main` skip을 우회함. H-3 재현은 **품질 main-only** 케이스(DEF-003)가 정확한 failing test이다.

> **H-1 참고:** T-03 canonical 단일 텍스트(`"그냥 그래요…"`)는 `Given_CanonicalNeutralText_When_FilterSentimentNeutral_Then_MatchesSingleAnalyzerNeutral`에서 **Pass**. M-7 이중 사전 불일치는 혼합 입력에서만 드러남(DEF-001).

---

## 2. P1 결함 (Medium — 재현 가능)

| ID | Severity | 기능/레이어 | test_plan ID | 재현 절차 | 기대값 | 실제값 | 근본 원인 (파일:줄) | 수정 요약 | 상태 |
|----|----------|-------------|--------------|-----------|--------|--------|---------------------|-----------|------|
| DEF-005 | Medium (P1) | main.cpp / HTTP·UI | M-1 | 서버 기동 → analyze·filter·upload·download 시나리오 | 라우팅·HTML·도메인 로직 분리 | `main.cpp` 372줄 — `renderPage` 130줄+, CSV 파싱·오케스트레이션 혼재 | `main.cpp:80-86` (`renderPage`), 라우트 핸들러 전반 | View/Controller 분리, HTML 템플릿 추출 (Phase 4) | **Open** |
| DEF-006 | Medium (P1) | Session / 전역 상태 | M-2, EX-04 | 필터 없이 GET `/download` | 전체 Session 또는 마지막 분석 결과 | BOM + `text\n` 헤더만, 데이터 0건 | `main.cpp:16` (`fil_data`) · `Session` · `TextAnalyzer` static 캐시 3곳 분산 | `FeedbackRepository` 단일 저장소; download 소스 통일 | **Open** |
| DEF-007 | Medium (P1) | Logger / UI | M-4 | warning·error 발생 후 GET `/` | 페이지에 level별 로그 표시 | 콘솔(`cout`/`cerr`)만 출력; UI 미표시 | `Logger` 구현 · `Filters.h:68-70` (`std::cout` 부작용) | Logger UI 연동; Filters 디버그 출력 제거 | **Open** |
| DEF-008 | Medium (P1) | GET `/download` | M-6, EX-04, TC-A-05 | Session에 N건 → 필터 없이 `/download` | PRD: 필터 성공 직후 집합 또는 warning | `fil_data` 비어 있으면 빈 CSV | `main.cpp:356-365` (`fil_data` 순회만) | 필터 결과·Session 정책 통일; CSV 이스케이프 | **Open** |
| DEF-009 | Medium (P1) | Constants / Filters | M-7, EX-01 | `"괜찮아요"` analyze → filter 중립 / `"배송이 늦었어요"` analyze vs filter 부정 | TextAnalyzer·Filters 동일 감성·카테고리 판정 | 통계 중립 vs 필터 긍정/부정 불일치 | `Constants.cpp:7-21` · `Filters.cpp:5-20` · `UIComponents.cpp:3-5` | 키워드 단일 레지스트리; `initFilterKeywords` 제거 | **Open** |

---

## 3. 결함 ↔ AC 매핑

| AC | 결함 ID | Green 조건 |
|----|---------|------------|
| AC-1 | DEF-001, DEF-009 | T-03 ctest Pass |
| AC-2 | DEF-002 | T-05 ctest Pass |
| AC-3 | DEF-003 | T-06 ctest Pass |
| AC-4 | DEF-004 | ctest 34/34 Green + TA·Filters·CsvParser 커버리지 ≥90% |

---

## 4. 변경 이력

| 일자 | Phase | 변경 내용 |
|------|-------|-----------|
| 2026-05-22 | RED | 초版 작성 — H-1~H-4 필수 4건 + M-1/M-2/M-4/M-6/M-7 선택 5건; ctest 3 failing 스냅샷 연결 |
