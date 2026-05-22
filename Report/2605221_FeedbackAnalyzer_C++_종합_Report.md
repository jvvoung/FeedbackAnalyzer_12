# Feedback Analyzer — 프로젝트 종합 진행 보고서

| 항목 | 내용 |
|------|------|
| **프로젝트** | Feedback Analyzer (C++17 리팩토링 챌린지) |
| **워크스페이스** | `c:\DEV\FeedbackAnalyzer_12` |
| **저장소** | https://github.com/jvvoung/FeedbackAnalyzer_12.git |
| **브랜치** | `Refactoring` |
| **보고 일자** | 2026-05-22 |
| **작업자** | KaneUng (`jwadeung98@gmail.com`) · 생성형 AI 보조 (Cursor Agent) |
| **보고 범위** | Spec → RED → GREEN → Refactoring 전 구간 누적 (Report 01~19 + 본 세션 ctest·lcov 재검증) |

---

## 1. 작업 개요

### 1.1 프로젝트·브랜치·일정

| 항목 | 내용 |
|------|------|
| 프로젝트명 | Feedback Analyzer — C++17 웹 피드백 분석기 리팩토링 챌린지 |
| 현재 브랜치 | **`Refactoring`** (Phase 2~3 Dual-Track REFACTOR + Phase 3 Legacy Cleanup 완료) |
| 주요 브랜치 이력 | `Spec` → `Red` → `Green` (PR #3 머지) → `Refactoring` |
| 작업 기간 | 2026-05-21 ~ 2026-05-22 |
| 작업자 | KaneUng |
| AI 보조 | Cursor Agent — 테스트·리팩토링·보고서·프롬프트 Export |

### 1.2 단계별 진행 요약

| 단계 | 브랜치 | 핵심 산출 | 상태 |
|------|--------|-----------|------|
| **Spec** | `Spec` (PR #1) | `docs/analysis.md`, `docs/PRD.md`, `docs/test_plan.md`, `.cursorrules` | ✅ 완료 |
| **RED** | `Red` (PR #2) | GTest 인프ra, 34→14 스켈레톤, failing test 3건, `defect_list.md` | ✅ 완료 |
| **GREEN** | `Green` (PR #3) | H-1/H-2/H-3 수정, ctest **14/14 Pass**, Track A/B 16 TC | ✅ 완료 |
| **Refactoring** | `Refactoring` | Dual-Track 8커밋 + Exercise 4 + Phase 3 Legacy, GM 4/4, ctest **19/20 Pass** | ✅ 기능 Green |
| **검증** | `Refactoring` | lcov·Invariant 분석 (Report 19), AC-4 커버리지 | ⚠️ Partial |

### 1.3 참조 보고서

| # | 경로 | 단계 |
|---|------|------|
| 01 | `Report/Spec/01.Spec_진행완료.md` | Spec |
| 02~08 | `Report/Red/*.md` | RED |
| 09~11 | `Report/Green/*.md` | GREEN |
| 12~19 | `Report/Refactoring/*.md` | Refactoring |

---

## 2. 완료된 To-Do 항목 요약 (README RED·Golden Master To-Do 기준)

> **참고:** `docs/analysis.md` Phase 6(프레젠테이션 분리)은 Dual-Track REFACTOR(R-U2 HtmlPageRenderer 추출)로 **선행 완료**되었으나, `PageViewModel`·Logger UI(AC-6) 등 잔여 항목은 미완이다. 아래는 README 체크리스트 기준이다.

### 2.1 Track A — HTTP / Boundary (8/8 ✅)

| TC | 시나리오 | test_plan | 상태 |
|----|----------|-----------|------|
| TC-A-01 | POST `/analyze` Happy Path (부정+배송) | T-02 | ✅ |
| TC-A-02 | 빈 text → 피드백 미추가 | T-08 | ✅ |
| TC-A-03 | Session 비어 filter → warning | T-09 | ✅ |
| TC-A-04 | 필터 0건 → warning | T-10 | ✅ |
| TC-A-05 | download 필터 미실행 → BOM+헤더만 | EX-04, M-6 | ✅ |
| TC-A-06 | CSV `text` 없음 → error | T-05, EX-06 | ✅ |
| TC-A-07 | textarea 개행 E2E | T-11, AC-7 | ✅ |
| TC-A-08 | 부록 A HTTP-S-01~05 | AC-5 | ✅ (문서 Green) |

### 2.2 Track B — Domain / Service (8/8 ✅)

| TC | 시나리오 | test_plan | 상태 |
|----|----------|-----------|------|
| TC-B-01 | 빈 목록 → 감정 0/0/0 | T-01 | ✅ |
| TC-B-02 | 중립 filter = Analyzer 중립 100% | T-03, H-1 | ✅ |
| TC-B-03 | 전체/전체 → 전체 반환 | T-04 | ✅ |
| TC-B-04 | CSV `text` 컬럼 파싱 | T-05, H-2 | ✅ |
| TC-B-05 | `택배` → filter 배송 | T-06, H-3 | ✅ |
| TC-B-06 | 품질만 → 감정 중립 | T-07 | ✅ |
| TC-B-07 | 긍·부 동시 → 긍정 우선 | EX-09 | ✅ |
| TC-B-08 | SENTIMENT_KEYWORDS 단일 소스 | EX-01, M-7 | ✅ |

### 2.3 커버리지·결함 목록 To-Do

| 항목 | 상태 |
|------|------|
| `docs/defect_list.md` 생성 (H-1~H-4) | ✅ |
| 결함 ↔ test_plan ID 연결 | ✅ |
| H-1/H-2/H-3 수정 후 ctest Green | ✅ |
| lcov baseline (`coverage_baseline_phase1.info`) | ✅ |
| **TextAnalyzer / Filters / CsvParser / Feedback 각 ≥90%** | ⚠️ **CsvParser 71.2% 미달** |
| **Domain stretch ≥95%** | ❌ **78.3%** |
| Boundary lcov ≥85% | ❌ **0%** (UT 타깃 미포함) |

### 2.4 Golden Master To-Do (6/9 ✅, CI 0/3 ⏳)

| 항목 | 상태 |
|------|------|
| GM-01~03: `domain_golden_expected.txt` + git add | ✅ |
| GM-04~06: `GoldenMasterTest.cpp`, Approval, ctest 4/4 | ✅ |
| GM-07~09: CI workflow, PR 차단, Refactor 후 재검증 | ⏳ 미착수 |

### 2.5 Refactoring Phase 로드맵 (`docs/analysis.md` 부록 C)

| Phase | 내용 | 상태 |
|-------|------|------|
| Phase 1 | GTest + H-1/H-2/H-3 | ✅ |
| Phase 2 | HtmlRenderer·RouteHandlers·CsvParser 분리 | ✅ (`main.cpp` **18줄**) |
| Phase 3 | AppState·Constants·Rename·globalSent/Kw 제거 | ✅ |
| Phase 4 | Logger UI·download warning·`.gitignore` | ⏳ 부분 (download·CSV 이스케이프만) |
| Phase 5 | Trend·File DB (선택) | ⏳ 미착수 |
| Phase 6 | PageViewModel·프레젠테이션 완전 분리 | ⏳ HtmlPageRenderer만 추출 |
| Phase 7 | 전역/static 정리 | ⏳ 대부분 완료, Logger static 잔존 |

---

## 3. RED 단계 결과

### 3.1 작성한 테스트 목록

| 파일 | 테스트 수 | 주요 시나리오 |
|------|-----------|---------------|
| `tests/TextAnalyzerTest.cpp` | 4 (스켈레톤) | T-01, T-03, T-07, EX-09 |
| `tests/FiltersTest.cpp` | 4 | T-02~T-04, **T-03/H-1**, **T-06/H-3** |
| `tests/CsvParserTest.cpp` | 3 | **T-05/H-2**, 헤더만, 2행 |
| `tests/FeedbackTest.cpp` | 2 | T-11, 빈 문자열 |
| `tests/KeywordUtilsTest.cpp` | 1 | UTF-8 한글 키워드 |
| **합계 (Phase 1 스켈레톤)** | **14** | README TC-B/A 매핑 |

추가 RED 세션(Report 04): 초기 **34건** 작성 후 스켈레톤 **14건**으로 정리. Refactoring 단계에서 Golden Master **4건** 추가.

### 3.2 실패 확인 (P0 Failing Tests)

| test_plan | Failing Test | 결함 | RED 확인 |
|-----------|--------------|------|----------|
| T-03 / H-1 | `FiltersTest.Given_NeutralText_When_FilterSentimentNeutral_Then_MatchesTextAnalyzerSet` | 중립 필터 ≠ Analyzer | ✅ **Fail 확인** |
| T-05 / H-2 | `CsvParserTest.Given_CsvWithoutTextColumn_When_Parse_Then_ZeroFeedbacks` | `fields[0]` fallback | ✅ **Fail 확인** |
| T-06 / H-3 | `FiltersTest.Given_MainKeywordOnly_When_FilterDelivery_Then_Included` | `main` skip | ✅ **Fail 확인** |

**RED 스냅샷:** ctest **31/34 Pass, 3 Fail** (Report 04) → 스켈레톤 14건 전부 `FAIL() << "RED"` (Report 08).

### 3.3 RED 산출물

- `CMakeLists.txt` — GTest FetchContent, `feedback_analyzer_tests` 타깃
- `docs/defect_list.md` — H-1~H-4 + 재현 절차
- `Report/Red/03~08.*.md` — 단계별 RED 보고서

---

## 4. GREEN 단계 결과

### 4.1 통과한 테스트

**Phase 1 GREEN 최종: ctest 14/14 Pass (0 failures)**

| # | 테스트 | TC / AC |
|---|--------|---------|
| 1 | `KeywordUtilsTest.Given_Utf8KoreanKeyword_...` | 보조 |
| 2~3 | `FeedbackTest.*` | TC-A-02, TC-A-07 |
| 4~6 | `CsvParserTest.*` | TC-B-04, H-2 |
| 7~10 | `FiltersTest.*` | TC-A-01, TC-B-02~05, H-1/H-3 |
| 11~14 | `TextAnalyzerTest.*` | TC-B-01, 06, 07 |

**Refactoring 후 현재: ctest 19 passed / 0 failed / 1 skipped (총 20)**

- Golden Master GM-D-01~04 **4/4 Pass**
- `GoldenMasterCapture.UpdateGoldenFile_WhenGoldenUpdateEnvSet` — Skip (의도적)

### 4.2 GREEN 커밋 메시지 (Phase 1, TC 1건 = 1커밋)

| 순서 | 커밋 (short) | 메시지 |
|------|--------------|--------|
| 1 | `12cdfbd` | `fix: TC-B-02 GREEN — Filters neutral sentiment matches TextAnalyzer (AC-1, H-1)` |
| 2 | `693598c` | `feat(green): include main keywords in category filter (AC-3, H-3)` |
| 3 | `7775197` | `test(green): T-01 empty feedback sentiment counts` |
| 4 | `40197eb` | `feat(green): parse CSV text column by header (AC-2, H-2)` |
| 5 | `4ffe64b` | `feat(green): filter sentinel 전체 returns all feedbacks` |
| 6 | `c09b2e3` | `feat(green): separate category from sentiment (T-07)` |
| 7 | `8d8f243` | `feat(green): positive-first sentiment order (EX-09)` |
| 8 | `e47396e` | `feat(green): unify sentiment keywords in Constants (M-7)` |
| 9 | `4500e89` | `feat(green): skip empty analyze text (T-08)` |
| 10 | `056e400` | `feat(green): warn when filtering empty session (T-09)` |
| 11 | `09198db` | `feat(green): warn when filter returns zero (T-10)` |
| 12 | `584853b` | `feat(green): analyze happy path negative delivery (T-02)` |
| 13 | `5d55e16` | `feat(green): upload error when text column missing` |
| 14 | `3c6bca7` | `feat(green): download boundary and multiline (M-6, AC-7)` |
| 15 | `09ef68c` | `test(green): T-03 TextAnalyzer neutral sentiment classification` |
| 16 | `48afe14` | `test(green): KeywordUtils containsAny utf8 korean` |
| 17 | `e42d093` | `docs(green): verify appendix A endpoint checklist (AC-5)` |

**PR:** `#3 from jvvoung/Green` → main 머지 (`5335141`)

---

## 5. Refactoring 결과

### 5.1 수행한 Refactoring (Dual-Track + Phase 3)

| 커밋 | Track | 변경 요약 |
|------|-------|-----------|
| `a2482e4` | Domain R-L1 | `SentimentClassifier` — if-else → 클래스 |
| `4f58ce3` | Domain R-L2 | `KeywordRegistry` 단일 소스 |
| `89837fa` | Domain R-L3 | `FeedbackClassifier` classify hub |
| `f09d6c3` | Domain R-L4 | `AnalyzeUseCase` / `FilterUseCase` |
| `dbc2fce` | Domain R-L5 | `AppState` → Session 통합 |
| `0253111` | Boundary R-U1 | `FormParser`, `CsvParser` (prod) |
| `8ecca10` | Boundary R-U2 | `AppMessages`, `HtmlPageRenderer`, `CsvExporter` |
| `eda386c` | Boundary R-U3 | `RouteHandlers` — `main.cpp` **18줄** |
| `d59e1bb` | Phase 3 | Exercise 4 Rename (`sent/kw/fil` → 명시적 API), `FeedbackSession` |
| `4e41d6e` | Phase 3 | `ServerConfig`/`CsvConstants`/`FilterConstants`, 레거시 삭제 |

**선택 항목 (Exercise 4·L-5):**

- `FeedbackSession` API (`getCurrent`/`update`/`clear`)
- `KeywordUtils` → `std::any_of` inline 패턴
- `FileHandler.h`, `UIComponents.*`, `TextAnalyzer.cpp` 삭제 (dead code)
- `CsvExporter::escapeCsvField` — CSV 필드 이스케이프

### 5.2 주요 변경 파일

| 구분 | 신규 | 삭제·대체 |
|------|------|-----------|
| Domain | `SentimentClassifier.h`, `FeedbackClassifier.h`, `KeywordRegistry.cpp`, `AnalyzeUseCase.cpp`, `FilterUseCase.cpp`, `AppState.cpp`, `FeedbackSession.cpp` | `TextAnalyzer.cpp`, `Filters.cpp` (header-only화) |
| Boundary | `FormParser.cpp`, `CsvParser.cpp`, `HtmlPageRenderer.cpp`, `CsvExporter.cpp`, `AppMessages.cpp`, `RouteHandlers.cpp` | `main.cpp` 372줄 → **18줄** |
| Config | `ServerConfig.h`, `CsvConstants.h`, `FilterConstants.h` | 매직넘버 인라인 제거 |
| Test | `GoldenMasterTest.cpp`, `tests/golden/domain_golden_expected.txt` | — |

### 5.3 회귀 테스트 통과 여부

| 게이트 | 결과 |
|--------|------|
| ctest 전체 | ✅ **19 passed, 0 failed, 1 skipped** |
| Golden Master GM-D-01~04 | ✅ **4/4 Pass** (baseline 미갱신) |
| P0 Invariant (H-1/H-2/H-3) | ✅ Pass |
| `httplib.h` diff | ✅ **0** |
| HTTP·CSV·HTML 계약 | ✅ 불변 |

---

## 6. 커버리지 현황 (레이어별)

> 측정일: 2026-05-22 · 빌드: `build_cov/` (MinGW `--coverage -O0 -g`) · 도구: lcov  · `coverage.filtered.info`

### 6.1 Domain Service (UT 타깃)

| 파일 | Line Cover | AC-4 (≥90%) |
|------|------------|:-----------:|
| `TextAnalyzer.h` | **90.0%** | ✅ (경계) |
| `Filters.h` | **100%** | ✅ |
| `CsvParser.cpp` | **71.2%** | ❌ |
| `Feedback.h` | **100%** | ✅ |
| `KeywordUtils.h` | **100%** | ✅ |
| `SentimentClassifier.h` | **100%** | ✅ |
| `FeedbackClassifier.h` | **77.8%** | ❌ |
| `Constants.cpp` | **100%** | ✅ |
| `KeywordRegistry.cpp` | **100%** | ✅ |

### 6.2 레이어 합산

| 영역 | 범위 | Cover | 목표 | Gate |
|------|------|------:|------|:----:|
| **Domain (Service)** | TA + Filters + CsvParser | **78.3%** (90/115) | ≥95% stretch | ❌ |
| **Boundary** | FormParser + HtmlPageRenderer + CsvExporter + AppMessages | **0%** (0/117) | ≥85% | ❌ |
| **ctest** | 20건 | **19 Pass / 0 Fail** | 0 failures | ✅ |

### 6.3 CsvParser 미커버 병목 (AC-4 직접 원인)

- 따옴표 필드·`""` 이스케이프 (`splitCsvRecords`, `unquoteField`)
- CRLF 레코드 분리
- 짧은 데이터 행 skip, 빈 CSV, 빈 text 셀 skip

### 6.4 Baseline 비교

| 시점 | Domain 합산 | CsvParser | ctest |
|------|-------------|-----------|-------|
| Phase 1 baseline (Report 11) | 82.7% | 70% (support stub) | 14/14 |
| Phase 2 검증 (Report 16) | 81.3% | 71% (prod) | 19/20 |
| **현재 (Report 19)** | **78.3%** | **71.2%** | **19/20** |

---

## 7. 미완료 항목 및 다음 단계 제안

### 7.1 미완료 항목

| # | 항목 | 현재 | 우선순위 |
|---|------|------|----------|
| 1 | AC-4 CsvParser ≥90% | 71.2% | **P0** |
| 2 | Domain stretch ≥95% | 78.3% | P1 |
| 3 | Boundary lcov ≥85% | 0% | P1 |
| 4 | Golden Master CI (GM-07~09) | 미구현 | P2 |
| 5 | AC-6 Logger → HTML alert | Open (DEF-007) | P2 |
| 6 | DEF-005~009 P1 결함 | Open | P2 |
| 7 | Phase 5 Trend·File DB | 선택 과제 | P3 |
| 8 | `ENABLE_COVERAGE` CMake 옵션 | 미구현 | P3 |
| 9 | PR `feature/refactor-dual-track-complete` | 보류 | P1 |

### 7.2 다음 단계 제안 (우선순위)

1. **CsvParser UT 6건 추가** — 따옴표·CRLF·짧은행·빈 CSV·빈 text (Report 19 §7.1) → AC-4 90% 돌파
2. **Domain stretch 재측정** — CsvParser 보강 후 lcov ≥95% 확인
3. **Boundary 커버리지** — TC-A 수동 스모크 또는 test 타깃에 Boundary 소스 링크 검토
4. **Golden Master CI** — `.github/workflows/golden_master.yml` + required check
5. **Phase 4 Logger UI** — `Logger` → HtmlPageRenderer alert 연동 (AC-6)
6. **PR 생성** — AC-4 Green 확인 후 `Refactoring` → main PR

---

## 8. 발견된 이슈 및 해결 방법

### 8.1 P0 결함 (해결 완료)

| ID | 이슈 | 근본 원인 | 해결 |
|----|------|-----------|------|
| DEF-001 / H-1 | 중립 필터 ≠ TextAnalyzer | `Filters::S_KEYWORDS` vs `Constants` 불일치 | `Constants::SENTIMENT_KEYWORDS` 단일 소스 |
| DEF-002 / H-2 | CSV `text` 컬럼 무시 | `fields[0]` fallback | 헤더 `text` 인덱스 탐색 |
| DEF-003 / H-3 | 배송 키워드 필터 누락 | `main` skip | `main` 키워드 매칭 포함 |
| DEF-004 / H-4 | 테스트 인프라 없음 | CMake GTest 미구성 | FetchContent GTest + `tests/` |

### 8.2 Refactoring 중 이슈

| 이슈 | 해결 |
|------|------|
| `tests/support/CsvParser` stub vs prod `src/cpp/CsvParser.cpp` 이중화 | R-U1에서 prod로 통합, Golden Master로 회귀 고정 |
| `globalSent`/`globalKw` static 캐시 | Phase 3에서 삭제, 반환값만 사용 |
| `Filters` `std::cout` 부작용 | 디버그 루프 제거 |
| `FileHandler` dead code | YAGNI — 삭제 |
| Domain 커버리지 Phase 1 대비 하락 | prod CsvParser 병목; 기능 Green, 커버리지는 UT 보강 필요 |

### 8.3 미해결 P1 결함 (`defect_list.md`)

| ID | 이슈 | 상태 |
|----|------|------|
| DEF-005 | `main.cpp` God Function | ✅ **해소** (18줄) — defect_list 갱신 필요 |
| DEF-006 | download 소스 분산 (`fil_data`) | ⚠️ `AppState::lastFilteredFeedbacks`로 개선, 완전 통일 미완 |
| DEF-007 | Logger UI 미표시 | Open |
| DEF-008 | download 빈 fil_data | ⚠️ warning 추가, 정책 통일 미완 |
| DEF-009 | 감성 키워드 불일치 | ✅ **해소** (Constants 단일화) — defect_list 갱신 필요 |

### 8.4 인프라 이슈

| 이슈 | 해결 방안 |
|------|-----------|
| `-DENABLE_COVERAGE=ON` CMake 미구현 | `CMakeLists.txt`에 옵션 추가 |
| MSVC UTF-8 + 커버리지 | MinGW `build_cov/` 별도 빌드 사용 |
| lcov 미설치 환경 | gcovr `--lcov` 대체 (Phase 1) |

---

## 9. 생성형 AI 활용 회고

### 9.1 도움이 된 순간

| 영역 | AI 기여 |
|------|---------|
| **Spec·문서** | `analysis.md`·`test_plan.md`·PRD 정합성 대조, README RED To-Do 24항목 자동 매핑 |
| **RED** | GTest 스켈레톤 14건 + Given-When-Then 네이밍, H-1~H-3 failing test 시나리오 즉시 작성 |
| **GREEN** | TC 1건 = 1커밋 순서표 준수, 최소 diff로 H-1~H-3 수정 |
| **Refactoring** | Dual-Track 8커밋 순차 추출(SentimentClassifier → RouteHandlers), 매 커밋 후 ctest·GM 검증 |
| **Golden Master** | Approval 패턴·`domain_golden_expected.txt` baseline 생성 |
| **커버리지 분석** | lcov 미커버 줄 ↔ test_plan ID 매핑, UT 제안 6건 (코드 수정 없이) |
| **보고서 Export** | Report/Prompt 쌍 19세트 — 재현 가능한 작업 이력 |

### 9.2 한계

| 한계 | 설명 |
|------|------|
| **커버리지 착시** | AI가 “Green”이라고 해도 CsvParser 71% 병목은 lcov 수치로 별도 확인 필요 |
| **HTTP E2E** | Track A는 수동 스모크 의존 — AI가 실제 브라우저·curl 검증을 대체하지 못함 |
| **과잉 리팩토링 유혹** | GREEN 직후 REFACTOR 금지 규칙 없으면 범위 초과 위험 — `.cursorrules`·프롬프트 제약 필수 |
| **defect_list stale** | 리팩토링 후 DEF-005/009 등 상태 미갱신 — 수동 동기화 필요 |
| **Windows 셸** | PowerShell `&&` 미지원 등 환경 차이로 명령 재시도 필요 |

### 9.3 TC 작성 팁 (Cursor + GTest)

1. **test_plan ID를 테스트명에 포함** — `Given_NeutralText_When_FilterSentimentNeutral_Then_MatchesTextAnalyzerSet` ↔ T-03/H-1 추적 용이
2. **P0 failing test 먼저** — RED 3건 고정 후 GREEN; AI에게 “src/cpp 수정 금지” 명시
3. **Golden Master는 리팩토링 전** — Domain 직렬화 baseline 없이 REFACTOR하면 회귀 검출 불가
4. **커버리지 Gate는 UT 추가로** — Boundary 0%는 “테스트 부재”이지 “코드 미구현”이 아님 — CMake 링크 또는 TC-A 수동 구분
5. **1 TC = 1 커밋** — AI에게 커밋 순서표 제공 시 Green 이력이 리뷰 가능
6. **프롬프트에 금지어** — `httplib.h` 수정 금지, baseline 갱신 금지, 테스트 skip 추가 금지를 매 세션 반복

---

## 부록 A — 현재 ctest 스냅샷 (2026-05-22)

```
100% tests passed, 0 tests failed out of 20
Skipped: GoldenMasterCapture.UpdateGoldenFile_WhenGoldenUpdateEnvSet
Total Test time: ~0.83s
```

## 부록 B — 변경 이력

| 일자 | 변경 |
|------|------|
| 2026-05-22 | 본 종합 보고서 작성 — Spec~Refactoring 전 구간 통합, ctest·lcov 재검증 반영 |

---

**한 줄 결론:** Spec → RED → GREEN → Dual-Track Refactoring까지 **기능·회귀 게이트는 Green**(ctest 19/20, GM 4/4)이나, **AC-4 커버리지(CsvParser 71%)·Boundary lcov(0%)·CI·Logger UI**가 잔여 과제이다. 다음 우선 작업은 **CsvParser UT 6건 추가**로 AC-4 90% 돌파이다.
