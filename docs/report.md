# 코드 리뷰 및 개선 보고서

| 항목 | 내용 |
|------|------|
| **프로젝트** | Feedback Analyzer (C++17 리팩토링 챌린지) |
| **워크스페이스** | `c:\DEV\FeedbackAnalyzer_12` |
| **분석 일자** | 2026-05-22 |
| **Cursor AI 세션 범위** | Spec → RED → GREEN → Refactoring (Dual-Track) → Feature (연습 #4) → Boundary GTest 자동화 |

---

## 1. 분석 배경 (프로젝트·목적)

### 1.1 프로젝트 목적

Feedback Analyzer는 **자연어 고객 피드백을 수집·분류·시각화하는 C++17 웹 애플리케이션**이다 ([`docs/PRD.md`](../docs/PRD.md) §1.1).

| 구분 | 내용 |
|------|------|
| **What** | 웹 UI로 피드백 입력·CSV 업로드 → 감정(긍정/중립/부정)·키워드(배송·품질 등) 집계·필터·CSV 다운로드 |
| **Who** | CS/운영 담당자 + TDD·Clean Code 학습 중급 C++ 개발자 |
| **Why** | 의도적 코드 스멜이 포함된 레거시를 테스트 기반으로 점진 개선하며 ECB/Dual-Track 계층 구조를 학습 |

### 1.2 분석 범위

| 구분 | As-Is (레거시) | To-Be (현재) |
|------|----------------|--------------|
| **구조** | God Module `main.cpp` (~372줄) — HTTP·HTML·CSV·오케스트레이션 혼재 | ECB 분리: Domain(UseCase·Classifier) + Boundary(RouteHandlers·HtmlPageRenderer) |
| **상태** | `Session`, `fil_data`, `TextAnalyzer::globalSent/globalKw` 전역 분산 | `AppState` + `FeedbackSession` 단일 캡슐화 |
| **감성 분류** | `TextAnalyzer`·`Filters` 이중 키워드 사전, first-match if-else | `Constants::SENTIMENT_KEYWORDS` 단일 소스 + `SentimentClassifier::classifyWeighted()` |
| **테스트** | GTest 0건 | ctest **43 passed / 0 failed / 1 skipped** (총 44), Golden Master **4/4 Pass** |

### 1.3 Cursor AI 세션 Phase

| Phase | 산출 | 참조 보고서 |
|-------|------|-------------|
| **RED** | GTest 인프ra, failing test 3건, `defect_list.md` | [`Report/Red/`](../Report/Red/) |
| **GREEN** | H-1~H-3 수정, ctest 14/14 Pass | [`Report/Green/`](../Report/Green/) |
| **Refactoring** | Dual-Track R-L1~L5 + R-U1~U4, GM baseline | [`Report/Refactoring/15~19`](../Report/Refactoring/) |
| **Feature (연습 #4)** | KeywordUtils·FeedbackSession·FileHandler·가중치 감성 | [`Report/Feature/20~21`](../Report/Feature/) |
| **Boundary GTest** | T-08~T-11·EX-04~EX-08 in-process HTTP UT | [`Report/Feature/22`](../Report/Feature/22.Green_BoundaryTestAutomation_진행완료.md) |

---

## 2. Cursor AI 분석 요약 (항목별 표)

| # | 분석 영역 | Cursor AI 진단 요약 | 심각도 | 근거 문서/파일 |
|---|-----------|---------------------|--------|----------------|
| 1 | 코드 스멜 (SRP, Long Method, OCP…) | God Module `main.cpp`, 축약 메서드명(`sent`/`kw`/`fil`), 중복 키워드·`containsAny`, 전역 상태, FileHandler cout 스텁 | **높음~중** | [`docs/analysis.md`](../docs/analysis.md) §3 |
| 2 | ECB (Entity/Control/Boundary) | Domain: `TextAnalyzer`·`Filters`·UseCase·`SentimentClassifier` / Boundary: `RouteHandlers`·`HtmlPageRenderer`·`CsvExporter` / `main.cpp` bootstrap 18줄 | **중** (개선 완료) | `src/cpp/main.cpp`, `RouteHandlers.h` |
| 3 | P0 결함 (H-1~H-3) | H-1 중립 필터≠통계, H-2 CSV `text` 컬럼 미사용, H-3 `main` 키워드 스킵 — **전건 Fixed** | **높음** (해소) | [`docs/defect_list.md`](../docs/defect_list.md) |
| 4 | 테스트·Golden Master | Domain UT + Boundary in-process HTTP UT + GM-D-01~04 Approval — **4/4 Pass**, baseline 미갱신 | **중** (Gate Green) | `tests/*Test.cpp`, `tests/GoldenMasterTest.cpp` |
| 5 | Dual-Track (R-L/R-U) | Domain 5커밋 + Boundary 3커밋, PRD §7.2 HTTP/CSV/HTML **계약 유지**, `httplib.h` diff 0 | **중** (완료) | [`Report/Refactoring/15~18`](../Report/Refactoring/) |
| 6 | 연습 #4 (중복·명명·FileHandler·가중치) | `KeywordUtils` 공통화, Rename, `FeedbackSession`, `FileHandler::saveToCsv`, `classifyWeighted` | **중** (완료) | [`Report/Feature/20~22`](../Report/Feature/) |
| 7 | 커버리지·Invariant | ctest Green; AC-4 **Partial** — CsvParser 71.2%, Domain 78.3%, Boundary lcov 0% | **중** (미달) | [`docs/test_plan.md`](../docs/test_plan.md) §6~7, [`Report/Refactoring/19`](../Report/Refactoring/19.Refactoring_CoverageInvariant_검증완료.md) |

---

## 3. 개선/처리 내역

| 피드백사항 | 수정 조치 | 결과 |
|-----------|----------|------|
| H-1 중립 필터 불일치 | `Constants::SENTIMENT_KEYWORDS` 단일 소스 + `SentimentClassifier`/`FeedbackClassifier` 경유 | **Fixed** / ctest Pass (`FiltersTest.Given_NeutralText_...`) |
| H-2 CSV `text` 컬럼 | `CsvParser` 헤더 `text` 인덱스 탐색; `textIndex == -1` → 0건 | **Fixed** / ctest Pass (`CsvParserTest.Given_CsvWithoutTextColumn_...`) |
| H-3 `main` 키워드 스킵 | `Filters::filter()` — `CATEGORY_KEYWORDS[cat]["main"]` 매칭, skip 제거 | **Fixed** / ctest Pass (`FiltersTest.Given_MainKeywordOnly_...`) |
| H-4 테스트 인프ra 0건 | FetchContent GTest, `feedback_analyzer_tests`, `gtest_discover_tests` | **Fixed** / ctest 44건 등록 |
| God Module `main.cpp` | `RouteHandlers`, `HtmlPageRenderer`, UseCase 분리 | **Fixed** / `main.cpp` **18줄** (G-4 ≤200줄 달성) |
| `containsAny` 중복 | `KeywordUtils.h` — `namespace KeywordUtils` + `std::any_of` | **Fixed** / [`Report/Refactoring/17`](../Report/Refactoring/17.Refactoring_Exercise4_NamingDesign_진행완료.md) |
| 축약 메서드명 (`sent`/`kw`/`fil`) | `analyzeSentiment()` / `analyzeKeywords()` / `filter()` Rename | **Fixed** / Grep 0건 |
| `fil_data`·`globalSent/Kw` 전역 | `AppState::lastFilteredFeedbacks()` + static 캐시 제거 | **Fixed** / Phase 2~3 |
| `Session` / `getOldDataFromSession` | `FeedbackSession::getCurrent()` / `update()` / `clear()` | **Fixed** / [`Report/Refactoring/17`](../Report/Refactoring/17.Refactoring_Exercise4_NamingDesign_진행완료.md) |
| FileHandler cout 스텁 | Phase 3 삭제 → `FileHandler::saveToCsv(data, path)` RAII (`std::ofstream`) | **Fixed** / [`Report/Feature/20`](../Report/Feature/20.Feature_FileHandler_SaveToCsv_진행완료.md) |
| first-match → 가중치 감성 | `SentimentScore` + `classifyWeighted()`; `classify()` 위임 | **Fixed** / [`Report/Feature/21`](../Report/Feature/21.Feature_SentimentClassifier_WeightedScoring_진행완료.md) |
| T-08~T-11·EX-04~EX-08 수동만 | `BoundaryUseCaseTest` + `RouteHandlersBoundaryTest` in-process HTTP | **Fixed** / [`Report/Feature/22`](../Report/Feature/22.Green_BoundaryTestAutomation_진행완료.md) |
| AC-4 커버리지 ≥90% | CsvParser 71.2% 미달 — 따옴표·CRLF·unquote 분기 미커버 | **Open** / △ Partial |
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

**After (To-Be):** `src/cpp/SentimentClassifier.h` — 가중치 스코어링

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

**After (To-Be):** `src/cpp/main.cpp` — 18줄 bootstrap

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

**After (To-Be):** `src/cpp/KeywordUtils.h` — 공통 inline + `std::any_of`

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

**After (To-Be):** `src/cpp/FileHandler.cpp` — RAII 디스크 저장

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

**After (To-Be):** `src/cpp/FeedbackSession.h` — `AppState` 위임, 명시적 API

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

| 지표 | As-Is (레거시) | To-Be (2026-05-22) |
|------|----------------|---------------------|
| **ctest** | 0건 (H-4) | **43 passed, 0 failed, 1 skipped** (총 44) |
| **Golden Master** | 없음 | **GM-D-01~04: 4/4 Pass** (baseline 미갱신) |
| **P0 Gate (H-1~H-3)** | 3 failing | **전건 Pass** |
| **main.cpp 줄 수** | ~372줄 | **18줄** (G-4 ≤200줄 **달성**) |
| **Dead Code 제거** | `Session`, `FileHandler` cout 스텁, `S_KEYWORDS`, `globalSent/Kw` | Phase 3 Legacy Cleanup 완료 |
| **Domain 커버리지** | 측정 불가 | **78.3%** (stretch 95% **미달**) |
| **CsvParser 커버리지** | — | **71.2%** (≥90% **미달**) |
| **Boundary lcov** | — | **0%** (≥85% **미달**, UT 타깃 미포함) |
| **PRD §7.2 계약** | — | HTTP 5엔드포인트·CSV BOM+`text\n`·감정 라벨 **유지** |

**ctest 스냅샷 (2026-05-22):** `ctest --test-dir build --output-on-failure` → 100% tests passed, 0 tests failed out of 44. Skipped: `GoldenMasterCapture.UpdateGoldenFile_WhenGoldenUpdateEnvSet` (의도적).

### 5.2 향후 과제

| 우선순위 | 과제 | 근거 |
|----------|------|------|
| **P1** | CsvParser UT 확장 — 따옴표 이스케이프·CRLF·unquote·짧은 행 (71.2% → ≥90%) | [`Report/Refactoring/19`](../Report/Refactoring/19.Refactoring_CoverageInvariant_검증완료.md) §4.4 |
| **P1** | Boundary UT 타깃에 `RouteHandlers`·`CsvExporter` 등 링크 → lcov ≥85% | `docs/test_plan.md` §6.2 |
| **P2** | CI Golden Master gate (GM-07~08) — GitHub Actions + PR 차단 | [`Report/Refactoring/14`](../Report/Refactoring/14.Refactoring_README_GoldenMasterToDo_진행완료.md) |
| **P2** | Logger UI (AC-6) — warning/error → HTML alert | `docs/PRD.md` §6.4, DEF-007 |
| **P3** | DEF-008 download 정책 완전 통일 (필터 없을 때 Session vs warning) | `docs/defect_list.md` DEF-008 |
| **P3** | EX-05 POST `/analyze` 예외 경로 — 수동·스모크 잔존 | [`Report/Feature/22`](../Report/Feature/22.Green_BoundaryTestAutomation_진행완료.md) |

---

## 6. 변경 이력 (선택)

| 일자 | Phase | 내용 |
|------|-------|------|
| 2026-05-21 | Spec | `docs/PRD.md`, `docs/analysis.md`, `docs/test_plan.md` 작성 |
| 2026-05-22 | RED | GTest 인프ra, H-1~H-3 failing test, `defect_list.md` |
| 2026-05-22 | GREEN | H-1~H-3 Fixed, ctest 14/14 Pass |
| 2026-05-22 | Refactoring | Dual-Track 8커밋, `main.cpp` 18줄, GM 4/4, ctest 19/20 Pass |
| 2026-05-22 | Feature #4 | KeywordUtils·FeedbackSession·FileHandler·classifyWeighted |
| 2026-05-22 | Boundary GTest | T-08~T-11·EX-04~EX-08 자동화, ctest 43/44 Pass |
| 2026-05-22 | 본 보고서 | Cursor AI 분석 종합 — 코드 리뷰 및 개선 보고서 |

---

## 참조 보고서

| # | 경로 | 단계 |
|---|------|------|
| 01 | [`Report/Spec/01.Spec_진행완료.md`](Spec/01.Spec_진행완료.md) | Spec |
| 02~08 | [`Report/Red/`](Red/) | RED |
| 09~11 | [`Report/Green/`](Green/) | GREEN |
| 12~19 | [`Report/Refactoring/`](Refactoring/) | Refactoring |
| 20~22 | [`Report/Feature/`](Feature/) | Feature |
| — | [`Report/2605221_FeedbackAnalyzer_C++_종합_Report.md`](2605221_FeedbackAnalyzer_C++_종합_Report.md) | 종합 진행 |
