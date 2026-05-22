# Golden Master (Approval) 회귀 테스트

Feedback Analyzer는 **Domain Service 직렬화 기반** 출력 회귀 테스트(Golden Master / Approval)를 사용합니다. `TextAnalyzer`·`Filters`·`CsvParser`의 결정적 출력을 `tests/golden/` baseline과 Google Test에서 섹션별로 비교합니다. HTML 응답 본문·타임스탬프·포트 번호는 baseline에 넣지 않습니다.

> **현재 Phase:** Domain Golden Master **구현 완료** (GM-D-01~04, 4/4 Pass)  
> **예정 Phase:** HTTP·CSV 다운로드 Golden (`tests/golden/http/`, `tests/golden/csv/`) — Phase 2+

근거: `docs/test_plan.md` §2·§4, `docs/PRD.md` §3.2·§6.2, `docs/refactoring.md` §5.2, `README.md` §Golden Master 회귀 안전장치

---

## 1. expected 출력 파일 생성·보관 전략

| 항목 | 정책 |
|------|------|
| **저장 위치 (현재)** | `tests/golden/domain_golden_expected.txt` — 소스 트리에 **커밋 필수** |
| **저장 위치 (예정)** | `tests/golden/{csv,http}/` — Phase 2+ HTTP stat·CSV 다운로드 baseline |
| **경로 해석** | CMake가 `GOLDEN_DIR` compile definition으로 **소스 절대 경로** 주입 (빌드 디렉터리와 무관) |
| **파일 형식 (Domain)** | 단일 `.txt`, `[GM-D-xx: Title]` 섹션 + `---` 구분자, UTF-8 |
| **파일 형식 (예정 HTTP/CSV)** | `.html`, `.csv`, `.txt` — 실제 출력 그대로 (타임스탬프·alert 시각 제외) |
| **정규화** | 비교 시 CRLF→LF, 파일 끝 개행 1개로 통일 (`golden::normalizeLineEndings`, `trimTrailingNewlines`) |
| **Git 줄바꿈** | `.gitattributes`: `tests/golden/*.txt text eol=lf` |
| **갱신** | 의도된 Domain 동작 변경 시에만 baseline 갱신; PR에 diff 포함 |
| **금지** | `build/` 산출물, 타임스탬프, 포트 번호, HTML alert 시각 문자열을 golden에 넣지 않음 |

### 디렉터리 (As-Is / To-Be)

**현재 (Phase 1 — Domain):**

```
tests/
├── GoldenMasterTest.cpp
├── support/
│   └── GoldenMaster.h              # 직렬화·섹션 파서·diff·갱신
└── golden/
    └── domain_golden_expected.txt  # GM-D-01~04 (Git 추적)
```

**예정 (Phase 2+ — HTTP·CSV):**

```
tests/golden/
├── domain_golden_expected.txt      # ✅ 구현됨
├── csv/                            # ⏳ 예정
│   ├── R04_empty_download.csv
│   └── H03_filtered_download.csv
└── http/                           # ⏳ 예정
    ├── H01_analyze_positive.html   # stat 영역만 (body 전체 golden 금지)
    ├── H02_filter_no_data.html
    └── H04_upload_stats.html
```

### Domain baseline 섹션 형식

```
[GM-D-01: AnalyzeNegativeDelivery]
sentiment|긍정|0
sentiment|부정|1
keyword|배송|1
---
[GM-D-02: FilterNeutral]
count|1
text|그냥 그래요. 특별한 감정 없음.
---
```

| 직렬화 키 | 의미 | 예시 |
|-----------|------|------|
| `sentiment\|{키}\|{건수}` | `TextAnalyzer::analyzeSentiment` map | `sentiment\|부정\|1` |
| `keyword\|{키}\|{건수}` | `TextAnalyzer::analyzeKeywords` map | `keyword\|배송\|1` |
| `count\|{N}` | 필터/CSV 파싱 결과 건수 | `count\|3` |
| `text\|{본문}` | Feedback text (입력 순서 유지) | `text\|택배가 빨라요.` |

키 순서는 `std::map` 삽입 순서(UTF-8 바이트) 기준 — 결정적 출력.

### Baseline 갱신 절차

1. Domain Service 또는 `GoldenMaster.h` 캡처 로직 변경
2. **의도된** 동작 변경인지 PRD·test_plan과 대조
3. `GOLDEN_UPDATE=1`로 Capture 테스트만 실행
4. `tests/golden/domain_golden_expected.txt` diff 리뷰 후 커밋

```powershell
# 권장: scripts 또는 CMake 타깃
.\scripts\update_golden.ps1 -Verify
# 또는
cmake --build build --target update_golden

# 수동 (동일 동작)
$env:GOLDEN_UPDATE = "1"
ctest --test-dir build -R GoldenMasterCapture --output-on-failure
Remove-Item Env:GOLDEN_UPDATE
```

| 스크립트 | 역할 |
|----------|------|
| `scripts/update_golden.ps1` | Domain baseline 갱신 (`GOLDEN_UPDATE=1` + Capture 테스트) |
| `scripts/run_golden.ps1` | GM-D-01~04 회귀 검증 (`ctest -R GoldenMaster`) |
| `scripts/run_coverage.ps1` | MinGW 커버리지 빌드·lcov·HTML 리포트 |
| `scripts/capture_http_golden.ps1` | HTTP/CSV baseline 캡처 (Phase 2+, ctest 미연동) |

CMake: `update_golden` 커스텀 타깃 · `-DENABLE_COVERAGE=ON` (MinGW/GCC 전용, `scripts/run_coverage.ps1`과 동일).

---

## 2. Google Test 파일 비교 구현

본 프로젝트는 **Catch2가 아닌 Google Test**를 사용한다. ApprovalTests 등 외부 의존 없이 자체 헬퍼로 구현한다.

| 구성요소 | 경로 | 역할 |
|----------|------|------|
| **헬퍼** | `tests/support/GoldenMaster.h` | 직렬화, `loadGoldenSection()`, `compareGoldenSection()`, `printUnifiedDiff()` |
| **테스트** | `tests/GoldenMasterTest.cpp` | `GoldenMasterTest` 픽스처 + GM-D-01~04 개별 `TEST_F` |
| **Capture** | `GoldenMasterCapture.UpdateGoldenFile_WhenGoldenUpdateEnvSet` | `GOLDEN_UPDATE=1` 시 baseline 덮어쓰기 |
| **기준 파일** | `tests/golden/domain_golden_expected.txt` | 4섹션 baseline |

### 핵심 API (`golden` namespace)

| 함수 | 역할 |
|------|------|
| `serializeSentimentMap()` / `serializeKeywordMap()` | analyze map → 파이프 직렬화 |
| `serializeFilterResult()` | 필터/CSV 결과 → `count\|N` + `text\|…` |
| `loadGoldenSection(fileContent, "GM-D-01")` | `[GM-D-xx:` ~ `---` body 추출 |
| `compareGoldenSection()` | 정규화 후 바이트 비교 |
| `printUnifiedDiff()` | 실패 시 unified diff 생성 |
| `captureGmD01Body()` ~ `captureGmD04Body()` | 시나리오별 actual 생성 |
| `suppressStdout()` | `Filters` 내부 `std::cout` 억제 (stdout golden 아님) |

### 테스트 등록

```cpp
class GoldenMasterTest : public ::testing::Test {
protected:
    void SetUp() override { Constants::init(); }
};

TEST_F(GoldenMasterTest, GM_D01_AnalyzeNegativeDelivery_MatchesGolden)
TEST_F(GoldenMasterTest, GM_D02_FilterNeutral_MatchesGolden)
TEST_F(GoldenMasterTest, GM_D03_CsvParseTextColumn_MatchesGolden)
TEST_F(GoldenMasterTest, GM_D04_FilterAll_MatchesGolden)
```

### 환경 변수

| 변수 | 값 | 동작 |
|------|-----|------|
| `GOLDEN_UPDATE` | `1` | `domain_golden_expected.txt` 덮어쓰기 (`GoldenMasterCapture`) |
| (미설정) | — | strict 비교; 불일치 시 `ADD_FAILURE()` + unified diff |

### 실패 시 출력 예시

```
--- expected (GM-D-02)
+++ actual
@@ -1,2 +1,2 @@
 count|1
-text|그냥 그래요. 특별한 감정 없음.
+text|다른 텍스트
```

---

## 3. CMake / ctest 통합

루트 `CMakeLists.txt` (테스트 타깃 `feedback_analyzer_tests`):

| 항목 | 내용 |
|------|------|
| 소스 | `tests/GoldenMasterTest.cpp` |
| compile definition | `GOLDEN_DIR="${CMAKE_SOURCE_DIR}/tests/golden"` |
| discover | `gtest_discover_tests(feedback_analyzer_tests)` — GM-D별 개별 ctest 항목 |
| **집계 타깃** | `add_test(NAME GoldenMaster COMMAND feedback_analyzer_tests --gtest_filter=GoldenMasterTest.*)` |
| LABELS | `golden`, `domain` |

```powershell
cmake -S . -B build
cmake --build build

# 전체 (golden 포함)
ctest --test-dir build --output-on-failure

# Golden Master만 (권장 게이트)
ctest --test-dir build -R GoldenMaster -V --output-on-failure

# 개별 GM-D
ctest --test-dir build -R "GoldenMasterTest.GM_D02" --output-on-failure

# gtest 직접 실행
build\feedback_analyzer_tests.exe --gtest_filter=GoldenMasterTest.*
```

**기대 결과 (Domain):**

| 테스트 | 결과 |
|--------|------|
| `GoldenMasterTest.GM_D01` ~ `GM_D04` | ✅ Pass |
| `GoldenMaster` (aggregate) | ✅ **4/4 Pass** |
| `GoldenMasterCapture.UpdateGoldenFile_...` | ⏭ Skip (`GOLDEN_UPDATE` 미설정 시 정상) |

---

## 4. CI 자동 실행

| 항목 | 상태 |
|------|------|
| `.github/workflows/golden_master.yml` | ⏳ **미구현** (README GM-07) |
| GitHub required status check | ⏳ **미설정** (README GM-08) |

**권장 CI job (예정):**

```yaml
# .github/workflows/golden_master.yml (안)
- run: cmake -S . -B build && cmake --build build
- run: ctest --test-dir build -R GoldenMaster --output-on-failure
```

로컬·PR 검증 시 **Refactoring 후에도** 아래 명령으로 회귀 확인 (README GM-09):

```powershell
ctest --test-dir build -R GoldenMaster --output-on-failure
# 기대: 4/4 Pass
```

> HTTP 경계는 현재 `RouteHandlersBoundaryTest`가 in-process httplib + **부분 문자열 검증**으로 커버한다. HTTP Golden baseline은 Phase 2에서 stat HTML만 추가 예정.

---

## 5. test_plan / defect_list 매핑

| Golden ID | TEST_F | test_plan / README | 검증 출력 | AC / 결함 |
|-----------|--------|-------------------|-----------|-----------|
| **GM-D-01** | `GM_D01_AnalyzeNegativeDelivery_MatchesGolden` | T-02, TC-A-01 | `TextAnalyzer` sent/kw map | — |
| **GM-D-02** | `GM_D02_FilterNeutral_MatchesGolden` | T-03, TC-B-02 | `Filters::filter(중립, 전체)` 결과 | **AC-1**, H-1 |
| **GM-D-03** | `GM_D03_CsvParseTextColumn_MatchesGolden` | T-06, TC-B-05 | `CsvParser::parse` text 컬럼 | **AC-3** 연계 |
| **GM-D-04** | `GM_D04_FilterAll_MatchesGolden` | T-04, TC-B-03 | `Filters::filter(전체, 전체)` 3건 | — |

**단위 테스트와의 관계:**

| test_plan ID | 단위 테스트 (Given-When-Then) | Golden Master |
|--------------|-------------------------------|---------------|
| T-03 / H-1 | `FiltersTest.Given_NeutralText_...` | GM-D-02 (출력 스냅샷) |
| T-05 / H-2 | `CsvParserTest.Given_CsvWithoutTextColumn_...` | GM-D-03 (정상 CSV 파싱) |
| T-06 / H-3 | `FiltersTest.Given_MainKeywordOnly_...` | GM-D-03 입력(`택배`) 연계 |

리팩토링 Phase(2~3, Feature) 후에도 **Domain baseline은 불변**이 원칙이다. 의도적 변경 시에만 `GOLDEN_UPDATE=1`로 갱신하고, 단위 테스트 ID(T-01~T-07)는 그대로 유지한다.

---

## 6. 이중 Golden Master 로드맵

| 레이어 | Phase | baseline | 상태 |
|--------|-------|----------|------|
| **Domain Service** | Phase 1 | `tests/golden/domain_golden_expected.txt` | ✅ **4/4 Pass** |
| **CSV 다운로드** | Phase 2+ | `tests/golden/csv/*.csv` | ⏳ 예정 |
| **HTTP stat HTML** | Phase 2+ | `tests/golden/http/*.html` | ⏳ 예정 (`RouteHandlersBoundaryTest` 보완) |

Domain Golden은 stdout 리디렉션 대신 **Service 직렬화**를 사용한다. HTTP Golden은 alert 타임스탬프(`2026-05-21 14:30:00 : …`)를 제외한 **stat·건수 영역**만 고정하는 것을 권장한다 (`docs/analysis.md` §6.2).

---

## 7. README 체크리스트 (GM-01~09)

| ID | 항목 | 구현 |
|----|------|------|
| GM-01 | `domain_golden_expected.txt` (GM-D-01) | ✅ |
| GM-02 | GM-D-02~04 시나리오 | ✅ |
| GM-03 | Git 추적 | ✅ |
| GM-04 | `GoldenMasterTest.cpp` + `GoldenMaster.h` | ✅ |
| GM-05 | `GOLDEN_UPDATE=1` + 섹션 diff | ✅ |
| GM-06 | `add_test(NAME GoldenMaster)` 4/4 Pass | ✅ |
| GM-07 | `.github/workflows/golden_master.yml` | ⏳ |
| GM-08 | PR required status check | ⏳ |
| GM-09 | Refactoring 후 `ctest -R GoldenMaster` | ✅ (로컬); CI 미연동 |

---

## 8. 금지·주의 사항

| 금지 | 이유 |
|------|------|
| `src/cpp/httplib.h` 수정 | 외부 라이브러리 정책 (`.cursorrules` §1) |
| CI에서 `GOLDEN_UPDATE=1` | baseline 무분별 갱신 방지 |
| HTML body 전체 golden | 타임스탬프·alert 시각 불안정 |
| `Filters` stdout을 golden에 직접 저장 | `suppressStdout()` + 직렬화 결과만 비교 |
| Catch2 / ApprovalTests 외부 의존 | Google Test + 자체 헬퍼만 사용 |

---

## 9. 참고 문서

| 문서 | 용도 |
|------|------|
| [`docs/test_plan.md`](test_plan.md) | T-01~T-11, AC-1~AC-5, Given-When-Then |
| [`docs/refactoring.md`](refactoring.md) | Phase별 ctest·GM 검증 스냅샷 |
| [`docs/defect_list.md`](defect_list.md) | H-1~H-3 ↔ Gate Test 매핑 |
| [`docs/bug_fix.md`](bug_fix.md) | CsvParser 통합 후 GM-D-03 회귀 |
| [`README.md`](../README.md) §Golden Master | GM-01~09 운영 체크리스트 |

---

*최종 갱신: Domain Golden Master GM-D-01~04 (Google Test, `GOLDEN_DIR`, `GOLDEN_UPDATE=1`). HTTP·CSV Golden은 Phase 2+ 예정.*
