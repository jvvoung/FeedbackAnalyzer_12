# 코드 리뷰 및 개선 보고서

| 항목 | 내용 |
|------|------|
| **프로젝트** | Feedback Analyzer (C++17 리팩토링 챌린지) |
| **워크스페이스** | `c:\DEV\FeedbackAnalyzer_12` |
| **분석 일자** | 2026-05-22 |
| **Cursor AI 세션 범위** | Spec → RED → GREEN → Refactoring (Dual-Track) → Feature (연습 #4) → Boundary GTest 자동화 → **Src 모듈 레이아웃 · UML** |

---

## 1. 분석 배경 (프로젝트·목적)

### 1.1 프로젝트 목적

Feedback Analyzer는 **자연어 고객 피드백을 수집·분류·시각화하는 C++17 웹 애플리케이션**이다 ([`docs/PRD.md`](PRD.md) §1.1).

| 구분 | 내용 |
|------|------|
| **What** | 웹 UI로 피드백 입력·CSV 업로드 → 감정(긍정/중립/부정)·키워드(배송·품질 등) 집계·필터·CSV 다운로드 |
| **Who** | CS/운영 담당자 + TDD·Clean Code 학습 중급 C++ 개발자 |
| **Why** | 의도적 코드 스멜이 포함된 레거시를 테스트 기반으로 점진 개선하며 ECB/Dual-Track 계층 구조를 학습 |

### 1.2 분석 범위

| 구분 | As-Is (레거시) | To-Be (현재) |
|------|----------------|--------------|
| **구조** | God Module `main.cpp` (~372줄) — HTTP·HTML·CSV·오케스트레이션 혼재 | ECB 분리 + **모듈 디렉터리**: `app/`(http·ui·state·usecase), `csv/`, `feedback/` |
| **상태** | `Session`, `fil_data`, `TextAnalyzer::globalSent/globalKw` 전역 분산 | `AppState` + `FeedbackSession` 단일 캡슐화 |
| **감성 분류** | `TextAnalyzer`·`Filters` 이중 키워드 사전, first-match if-else | `Constants::SENTIMENT_KEYWORDS` 단일 소스 + `SentimentClassifier::classifyWeighted()` |
| **테스트** | GTest 0건 | ctest **43 passed / 0 failed / 1 skipped** (총 44), Golden Master **4/4 Pass** |
| **UML** | Legacy 다이어그램만 | `docs/diagrams/FeedbackAnalyzer_class_diagram_Refactoring.drawio` |

### 1.3 Cursor AI 세션 Phase

| Phase | 산출 | 참조 보고서 |
|-------|------|-------------|
| **RED** | GTest 인프ra, failing test 3건, `defect_list.md` | [`Report/Red/`](../Report/Red/) |
| **GREEN** | H-1~H-3 수정, ctest 14/14 Pass | [`Report/Green/`](../Report/Green/) |
| **Refactoring** | Dual-Track R-L1~L5 + R-U1~U4, GM baseline | [`Report/Refactoring/15~19`](../Report/Refactoring/) |
| **Feature (연습 #4)** | KeywordUtils·FeedbackSession·FileHandler·가중치 감성 | [`Report/Feature/20~21`](../Report/Feature/) |
| **Boundary GTest** | T-08~T-11·EX-04~EX-08 in-process HTTP UT | [`Report/Feature/22`](../Report/Feature/22.Green_BoundaryTestAutomation_진행완료.md) |
| **Src 모듈·UML** | `app/`·`csv/`·`feedback/` 재배치, Refactoring 클래스 다이어그램 | [`Report/Feature/23`](../Report/Feature/23.Feature_SrcModuleLayout_ClassDiagram_진행완료.md) |

---

## 2. Cursor AI 분석 요약 (항목별 표)

| # | 분석 영역 | Cursor AI 진단 요약 | 심각도 | 근거 문서/파일 |
|---|-----------|---------------------|--------|----------------|
| 1 | 코드 스멜 (SRP, Long Method, OCP…) | God Module `main.cpp`, 축약 메서드명(`sent`/`kw`/`fil`), 중복 키워드·`containsAny`, 전역 상태, FileHandler cout 스텁 | **높음~중** (해소) | [`docs/analysis.md`](analysis.md) §3 |
| 2 | ECB (Entity/Control/Boundary) | Domain: `feedback/` · Control: `app/usecase/` · Boundary: `app/http`·`app/ui`·`csv/` / `app/main.cpp` bootstrap **20줄** | **중** (개선 완료) | `src/cpp/app/main.cpp`, `src/cpp/app/http/RouteHandlers.h` |
| 3 | P0 결함 (H-1~H-3) | H-1 중립 필터≠통계, H-2 CSV `text` 컬럼 미사용, H-3 `main` 키워드 스킵 — **전건 Fixed** | **높음** (해소) | [`docs/defect_list.md`](defect_list.md) |
| 4 | 테스트·Golden Master | Domain UT + Boundary in-process HTTP UT + GM-D-01~04 Approval — **4/4 Pass**, baseline 미갱신 | **중** (Gate Green) | `tests/*Test.cpp`, `tests/GoldenMasterTest.cpp` |
| 5 | Dual-Track (R-L/R-U) | Domain 5커밋 + Boundary 3커밋, PRD §7.2 HTTP/CSV/HTML **계약 유지**, `httplib.h` diff 0 | **중** (완료) | [`Report/Refactoring/15~18`](../Report/Refactoring/) |
| 6 | 연습 #4 (중복·명명·FileHandler·가중치) | `KeywordUtils` 공통화, Rename, `FeedbackSession`, `FileHandler::saveToCsv`, `classifyWeighted` | **중** (완료) | [`Report/Feature/20~22`](../Report/Feature/) |
| 7 | 커버리지·Invariant | ctest Green; AC-4 **Partial** — CsvParser **71.2%**, Domain **78.3%**, Boundary **89.1%** (≥85% **달성**) | **중** (CsvParser·Domain stretch 미달) | [`docs/test_plan.md`](test_plan.md) §6~7, [`Report/Refactoring/19`](../Report/Refactoring/19.Refactoring_CoverageInvariant_검증완료.md) |
| 8 | Src 모듈 레이아웃 | flat `src/cpp/` 39파일 → `app/`·`csv/`·`feedback/` + `FA_INCLUDE_DIRS` | **낮음** (완료) | [`Report/Feature/23`](../Report/Feature/23.Feature_SrcModuleLayout_ClassDiagram_진행완료.md) |

---

## 3. 개선/처리 내역

| 피드백사항 | 수정 조치 | 결과 |
|-----------|----------|------|
| H-1 중립 필터 불일치 | `Constants::SENTIMENT_KEYWORDS` 단일 소스 + `SentimentClassifier`/`FeedbackClassifier` 경유 | **Fixed** / ctest Pass (`FiltersTest.Given_NeutralText_...`) |
| H-2 CSV `text` 컬럼 | `CsvParser` 헤더 `text` 인덱스 탐색; `textIndex == -1` → 0건 | **Fixed** / ctest Pass (`CsvParserTest.Given_CsvWithoutTextColumn_...`) |
| H-3 `main` 키워드 스킵 | `Filters::filter()` — `CATEGORY_KEYWORDS[cat]["main"]` 매칭, skip 제거 | **Fixed** / ctest Pass (`FiltersTest.Given_MainKeywordOnly_...`) |
| H-4 테스트 인프ra 0건 | FetchContent GTest, `feedback_analyzer_tests`, `gtest_discover_tests` | **Fixed** / ctest 44건 등록 |
| God Module `main.cpp` | `RouteHandlers`, `HtmlPageRenderer`, UseCase 분리 | **Fixed** / `src/cpp/app/main.cpp` **20줄** (G-4 ≤200줄 달성) |
| `containsAny` 중복 | `feedback/KeywordUtils.h` — `namespace KeywordUtils` + `std::any_of` | **Fixed** / [`Report/Refactoring/17`](../Report/Refactoring/17.Refactoring_Exercise4_NamingDesign_진행완료.md) |
| 축약 메서드명 (`sent`/`kw`/`fil`) | `analyzeSentiment()` / `analyzeKeywords()` / `filter()` Rename | **Fixed** / Grep 0건 |
| `fil_data`·`globalSent/Kw` 전역 | `AppState::lastFilteredFeedbacks()` + static 캐시 제거 | **Fixed** / Phase 2~3 |
| `Session` / `getOldDataFromSession` | `FeedbackSession::getCurrent()` / `update()` / `clear()` | **Fixed** / [`Report/Refactoring/17`](../Report/Refactoring/17.Refactoring_Exercise4_NamingDesign_진행완료.md) |
| FileHandler cout 스텁 | Phase 3 삭제 → `FileHandler::saveToCsv(data, path)` RAII (`std::ofstream`) | **Fixed** / [`Report/Feature/20`](../Report/Feature/20.Feature_FileHandler_SaveToCsv_진행완료.md) |
| first-match → 가중치 감성 | `SentimentScore` + `classifyWeighted()`; `classify()` 위임 | **Fixed** / [`Report/Feature/21`](../Report/Feature/21.Feature_SentimentClassifier_WeightedScoring_진행완료.md) |
| T-08~T-11·EX-04~EX-08 수동만 | `BoundaryUseCaseTest` + `RouteHandlersBoundaryTest` in-process HTTP | **Fixed** / [`Report/Feature/22`](../Report/Feature/22.Green_BoundaryTestAutomation_진행완료.md) |
| flat `src/cpp/` 혼잡 | `app/`·`csv/`·`feedback/` 모듈화, CMake `FA_INCLUDE_DIRS` | **Fixed** / [`Report/Feature/23`](../Report/Feature/23.Feature_SrcModuleLayout_ClassDiagram_진행완료.md) |
| Boundary lcov 0% | Boundary 소스를 `feedback_analyzer_tests`에 링크 + in-process HTTP UT | **Fixed** / Boundary **89.1%** (목표 ≥85%) |
| AC-4 CsvParser ≥90% | CsvParser **71.2%** 미달 — 따옴표·CRLF·unquote 분기 미커버 | **Open** / △ Partial |
| Domain stretch ≥95% | TA+Filters+CsvParser 합산 **78.3%** (목표 95% 미달) | **Open** / △ Partial |
| Logger UI (AC-6) | 콘솔만 출력, HTML alert 미연동 | **Open** / Phase 4 |
| DEF-008 download 정책 | `AppState::lastFilteredFeedbacks` 경로 개선, PRD 완전 통일 미완 | **Open** / △ Partial |
| CI Golden Master gate (GM-07~08) | GitHub Actions workflow 미착수 | **Open** / ⏳ |

> **PRD §7.2 회귀 보호 규칙(R-1~R-5) 유지:** HTTP 5엔드포인트 path·method, 감정 3분류 라벨, CSV BOM+`text\n` 형식, ctest Green, `httplib.h` diff 0 — 전 Phase에서 **준수** 확인.

---

## 4. 전후 코드 비교 (C++ 코드 블록)

### 4.1 SentimentClassifier — first-match vs classifyWeighted

**Before (As-Is):** `TextAnalyzer.h` / `Filters.h` — 긍정 선매칭 if-else

```cpp
// TextAnalyzer.h (레거시)
std::string s = u8"중립";
if (containsAny(txt, Constants::SENTIMENT_KEYWORDS[u8"긍정"])) {
    s = u8"긍정";
} else if (containsAny(txt, Constants::SENTIMENT_KEYWORDS[u8"부정"])) {
    s = u8"부정";
}
// Filters.h — 별도 S_KEYWORDS + initFilterKeywords() (H-1 원인)
```

**After (To-Be):** `src/cpp/feedback/SentimentClassifier.h` — 가중치 스코어링

```cpp
static std::string classifyWeighted(const std::string& text) {
    const SentimentScore score = scoreWeighted(text);
    if (score.positive > score.negative) return u8"긍정";
    if (score.negative > score.positive) return u8"부정";
    return u8"중립";
}
static std::string classify(const std::string& text) {
    return classifyWeighted(text);  // 단일 진입점
}
```

---

### 4.2 main.cpp — God Module vs bootstrap

**Before (As-Is):** `main.cpp` (~372줄) — `renderPage` 130줄+, 라우트·CSV·전역 `fil_data`

```cpp
// main.cpp (레거시 발췌)
std::vector<Feedback> fil_data;
std::string renderPage(...) { /* HTML 130줄+ */ }
server.Post("/analyze", [&](...) { /* 파싱·분석·렌더 혼재 */ });
server.Post("/filter", [&](...) { fil_data = ...; });
server.Get("/download", [&](...) { /* fil_data 순회 */ });
```

**After (To-Be):** `src/cpp/app/main.cpp` — 20줄 bootstrap

```cpp
int main() {
    Constants::init();
    httplib::Server server;
    RouteHandlers routeHandlers;
    routeHandlers.registerRoutes(server);
    Logger::logInfo(u8"서버가 " + ServerConfig::publicUrl() + u8" 에서 시작됩니다.");
    server.listen(ServerConfig::kBindHost, ServerConfig::kPort);
    return 0;
}
```

---

### 4.3 containsAny 중복 vs KeywordUtils

**Before (As-Is):** `TextAnalyzer.h`·`Filters.h` 각각 private `containsAny` for-loop

```cpp
// TextAnalyzer.h / Filters.h (레거시 — 동일 패턴 2벌)
bool containsAny(const std::string& text, const std::vector<std::string>& kws) {
    for (const auto& kw : kws) {
        if (text.find(kw) != std::string::npos) return true;
    }
    return false;
}
```

**After (To-Be):** `src/cpp/feedback/KeywordUtils.h` — 공통 inline + `std::any_of`

```cpp
namespace KeywordUtils {
inline bool containsAny(const std::string& text, const std::vector<std::string>& keywords) {
    return std::any_of(keywords.begin(), keywords.end(), [&text](const auto& kw) {
        return text.find(kw) != std::string::npos;
    });
}
}
```

---

### 4.4 FileHandler cout 스텁 vs saveToCsv

**Before (As-Is):** `FileHandler.h` — cout 스텁 (Phase 3에서 삭제됨)

```cpp
// FileHandler.h (레거시)
void saveResult(const std::vector<Feedback>& data) {
    std::cout << "saveResult" << std::endl;
    for (const auto& fb : data) std::cout << fb.getText() << std::endl;
}
```

**After (To-Be):** `src/cpp/csv/FileHandler.cpp` — RAII 디스크 저장

```cpp
bool FileHandler::saveToCsv(const std::vector<Feedback>& data, const std::string& path) {
    std::ofstream ofs(path, std::ios::binary);
    if (!ofs.is_open()) return false;
    ofs << CsvExporter::exportFilteredFeedbacks(data);
    return true;
}
```

> GET `/download`는 `RouteHandlers` → `CsvExporter` (메모리) 경로 **유지** — PRD §7.2 R-3.

---

### 4.5 Session getOldDataFromSession vs FeedbackSession

**Before (As-Is):** `Session.h` / `main.cpp` — key 미사용, `clear()` 없음

```cpp
// Session.h (레거시)
class Session {
public:
    static std::vector<Feedback> currentFeedbacks;
    static std::vector<Feedback>& getOldDataFromSession(const std::string& key) {
        return currentFeedbacks;  // key 미사용
    }
};
```

**After (To-Be):** `src/cpp/app/state/FeedbackSession.h` — `AppState` 위임, 명시적 API

```cpp
class FeedbackSession {
public:
    static std::vector<Feedback>& getCurrent() {
        return AppState::currentFeedbacks();
    }
    static void update(const std::vector<Feedback>& feedbacks) {
        AppState::currentFeedbacks() = feedbacks;
    }
    static void clear() {
        AppState::currentFeedbacks().clear();
        AppState::lastFilteredFeedbacks().clear();
    }
};
```

---

## 5. 효과 및 향후 과제

### 5.1 효과

| 지표 | As-Is (레거시) | To-Be (2026-05-22, Report 23·lcov 재측정) |
|------|----------------|------------------------------------------|
| **ctest** | 0건 (H-4) | **43 passed, 0 failed, 1 skipped** (총 44) |
| **Golden Master** | 없음 | **GM-D-01~04: 4/4 Pass** (baseline 미갱신) |
| **P0 Gate (H-1~H-3)** | 3 failing | **전건 Pass** |
| **main.cpp 줄 수** | ~372줄 | **20줄** (`src/cpp/app/main.cpp`, G-4 ≤200줄 **달성**) |
| **소스 구조** | flat `src/cpp/` 39파일 | `app/`·`csv/`·`feedback/` 모듈 디렉터리 |
| **Dead Code 제거** | `Session`, `FileHandler` cout 스텁, `S_KEYWORDS`, `globalSent/Kw` | Phase 3 Legacy Cleanup 완료 |
| **Domain 커버리지** (TA+Filters+CsvParser) | 측정 불가 | **78.3%** (90/115, stretch 95% **미달**) |
| **CsvParser 커버리지** | — | **71.2%** (57/80, ≥90% **미달**) |
| **Boundary lcov** (FormParser+Html+CsvExporter+AppMessages) | — | **89.1%** (106/119, ≥85% **달성**) |
| **전체 `src/cpp/` lcov** | — | **87.3%** (413/473 lines) |
| **PRD §7.2 계약** | — | HTTP 5엔드포인트·CSV BOM+`text\n`·감정 라벨 **유지** |

**ctest 스냅샷 (2026-05-22, `build_cov`):** `ctest --test-dir build_cov --output-on-failure` → 100% tests passed, 0 tests failed out of 44. Skipped: `GoldenMasterCapture.UpdateGoldenFile_WhenGoldenUpdateEnvSet` (의도적).

### 5.2 커버리지 상세 (lcov 재측정)

> **측정 환경:** `build_cov/` · `-DENABLE_COVERAGE=ON` · MinGW g++ 15.2.0 · `lcov --capture` → `coverage.filtered.info` · 제외: `tests/*`, `googletest/*`, `httplib.h`

#### 파일별 (프로젝트 소스)

| 파일 | Lines | Cover | ≥90% (AC-4) | 비고 |
|------|------:|------:|:-----------:|------|
| `feedback/TextAnalyzer.h` | 20 | **90.0%** | ✅ (경계) | `analyzeKeywords` return 미히트 |
| `feedback/Filters.h` | 15 | **100%** | ✅ | — |
| `csv/CsvParser.cpp` | 80 | **71.2%** | ❌ | 따옴표·CRLF·unquote·짧은 행 미커버 |
| `feedback/SentimentClassifier.h` | 18 | **100%** | ✅ | 가중치 스코어링 |
| `feedback/FeedbackClassifier.h` | 9 | **77.8%** | ❌ | 미등록 카테고리·`main` 키 없음 분기 |
| `feedback/KeywordUtils.h` | 4 | **100%** | ✅ | — |
| `feedback/Constants.cpp` | 34 | **100%** | ✅ | — |
| `csv/CsvExporter.cpp` | 28 | **96.4%** | ✅ | Boundary UT 반영 |
| `csv/FileHandler.cpp` | 7 | **100%** | ✅ | FH-01~05 |
| `app/http/FormParser.cpp` | 24 | **87.5%** | ❌ | — |
| `app/http/RouteHandlers.cpp` | 82 | **80.5%** | ❌ | in-process HTTP UT |
| `app/ui/HtmlPageRenderer.cpp` | 53 | **90.6%** | ✅ | — |
| `app/ui/AppMessages.cpp` | 14 | **71.4%** | ❌ | — |
| `app/usecase/AnalyzeUseCase.cpp` | 18 | **94.4%** | ✅ | — |
| `app/usecase/FilterUseCase.cpp` | 16 | **93.8%** | ✅ | — |

#### 영역 합산

| 영역 | 범위 | Stmts | Cover | 목표 | Gate |
|------|------|------:|------:|------|:----:|
| **Domain (Service)** | TextAnalyzer + Filters + CsvParser | 115 | **78.3%** (90/115) | ≥95% stretch | ❌ |
| **Boundary** | FormParser + HtmlPageRenderer + CsvExporter + AppMessages | 119 | **89.1%** (106/119) | ≥85% | ✅ |
| **전체 프로덕션** | `src/cpp/**` | 473 | **87.3%** (413/473) | — | — |

#### Baseline 비교

| 시점 | Domain 합산 | CsvParser | Boundary | ctest |
|------|-------------|-----------|----------|-------|
| Phase 1 baseline (Report 11) | 82.7% | 70% (support stub) | — | 14/14 |
| Phase 2 검증 (Report 16) | 81.3% | 71% | 0% | 19/20 |
| Report 19 (Boundary UT 미링크) | **78.3%** | **71.2%** | **0%** | 19/20 |
| **현재 (Report 23 + Boundary UT)** | **78.3%** | **71.2%** | **89.1%** | **43/44** |

> Domain·CsvParser 수치는 Report 19와 **동일** — Boundary GTest 추가로 Boundary만 0%→89.1% 개선. CsvParser 병목(따옴표·CRLF)은 **미해소**.

**lcov 산출물:** `coverage.info`, `coverage.filtered.info`, `coverage.project.info`, `build_cov/coverage_html/` (genhtml 실행 가능)

### 5.3 향후 과제

| 우선순위 | 과제 | 근거 |
|----------|------|------|
| **P1** | CsvParser UT 확장 — 따옴표 이스케이프·CRLF·unquote·짧은 행 (**71.2% → ≥90%**) | [`Report/Refactoring/19`](../Report/Refactoring/19.Refactoring_CoverageInvariant_검증완료.md) §4.4 |
| **P1** | Domain stretch **78.3% → ≥95%** — CsvParser 보강 후 재측정 | `docs/test_plan.md` §6.2 |
| **P2** | CI Golden Master gate (GM-07~08) — GitHub Actions + PR 차단 | [`Report/Refactoring/14`](../Report/Refactoring/14.Refactoring_README_GoldenMasterToDo_진행완료.md) |
| **P2** | Logger UI (AC-6) — warning/error → HTML alert | `docs/PRD.md` §6.4, DEF-007 |
| **P2** | `AppMessages`·`RouteHandlers` 미커버 분기 보강 (80~87%대) | lcov §5.2 |
| **P3** | DEF-008 download 정책 완전 통일 (필터 없을 때 Session vs warning) | `docs/defect_list.md` DEF-008 |

---

## 6. 변경 이력 (선택)

| 일자 | Phase | 내용 |
|------|-------|------|
| 2026-05-21 | Spec | `docs/PRD.md`, `docs/analysis.md`, `docs/test_plan.md` 작성 |
| 2026-05-22 | RED | GTest 인프ra, H-1~H-3 failing test, `defect_list.md` |
| 2026-05-22 | GREEN | H-1~H-3 Fixed, ctest 14/14 Pass |
| 2026-05-22 | Refactoring | Dual-Track 8커밋, `main.cpp` 20줄, GM 4/4, ctest 19/20 Pass |
| 2026-05-22 | Feature #4 | KeywordUtils·FeedbackSession·FileHandler·classifyWeighted |
| 2026-05-22 | Boundary GTest | T-08~T-11·EX-04~EX-08 자동화, ctest 43/44 Pass |
| 2026-05-22 | Src 모듈·UML | `app/`·`csv/`·`feedback/` 재배치, Refactoring 클래스 다이어그램 |
| 2026-05-22 | 본 보고서 | Report 23·lcov 재측정 반영 — 경로·커버리지·Boundary Gate 갱신 |

---

## 참조 보고서

| # | 경로 | 단계 |
|---|------|------|
| 01 | [`Report/Spec/01.Spec_진행완료.md`](../Report/Spec/01.Spec_진행완료.md) | Spec |
| 02~08 | [`Report/Red/`](../Report/Red/) | RED |
| 09~11 | [`Report/Green/`](../Report/Green/) | GREEN |
| 12~19 | [`Report/Refactoring/`](../Report/Refactoring/) | Refactoring |
| 20~23 | [`Report/Feature/`](../Report/Feature/) | Feature |
| — | [`Report/2605221_FeedbackAnalyzer_C++_종합_Report.md`](../Report/2605221_FeedbackAnalyzer_C++_종합_Report.md) | 종합 진행 |
