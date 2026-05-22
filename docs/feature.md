# 추가 요구사항 구현 결과

| 항목 | 내용 |
|------|------|
| **작성일** | 2026-05-22 |
| **기준** | `docs/PRD.md` §3.2 F-05·F-06, 교육 슬라이드 **연습 #4** (FileHandler · 감성 분석 정확도 개선) |
| **테스트** | 44 cases Green (`ctest --test-dir build`, 1 Skipped) |
| **상세 보고** | [`Report/Feature/20.Feature_FileHandler_SaveToCsv_진행완료.md`](../Report/Feature/20.Feature_FileHandler_SaveToCsv_진행완료.md), [`Report/Feature/21.Feature_SentimentClassifier_WeightedScoring_진행완료.md`](../Report/Feature/21.Feature_SentimentClassifier_WeightedScoring_진행완료.md) |

---

## 1. 구현 요약

| 기능 | 설명 | 주요 파일 |
|------|------|-----------|
| **FileHandler CSV 디스크 저장** | `FileHandler::saveToCsv(data, path)` — UTF-8 BOM + `text\n` 헤더 CSV를 로컬 파일로 저장 (`CsvExporter`와 동일 계약) | `src/cpp/csv/FileHandler.h`, `FileHandler.cpp`, `CsvExporter.cpp` |
| **감성 분석 가중치 스코어링** | first-match(긍→부→중립) → `SentimentScore` 집계 후 건수 비교; `TextAnalyzer`·`Filters`는 `FeedbackClassifier` 단일 소스 유지 | `src/cpp/feedback/SentimentClassifier.h`, `FeedbackClassifier.h` |

> 두 기능 모두 **Domain/Infra 레이어** 변경이며, HTTP 5엔드포인트 path·method 및 `httplib.h`는 **변경하지 않았다** (PRD §7.2).

---

## 2. FileHandler CSV 디스크 저장

### 2.1 API

```cpp
// src/cpp/csv/FileHandler.h
class FileHandler {
public:
    static bool saveToCsv(const std::vector<Feedback>& data, const std::string& path);
};
```

| 항목 | 내용 |
|------|------|
| 성공 | `return true` — BOM + `text\n` + 본문(`{text}\n` × N) 기록 |
| 실패 | `return false` — `ofstream` open 실패 (쓰기 불가 path 등) |
| CSV 형식 | `CsvExporter::exportFilteredFeedbacks()` 결과와 **바이트 동일** (`buildCsvBody()` 공유) |

### 2.2 동작

1. `std::ofstream ofs(path, std::ios::binary)` 로 파일 오픈 (Windows `\n`→`\r\n` 변환 방지)
2. `CsvExporter::exportFilteredFeedbacks(data)` 문자열을 그대로 기록
3. GET `/download`는 기존대로 **메모리 응답** (`RouteHandlers` → `CsvExporter`) — 디스크 경유 없음

### 2.3 HTTP 라우트

| 변경 | 설명 |
|------|------|
| **없음** | 디스크 저장은 독립 Infra 유틸. Phase 5 File DB 연동 전까지 tests·로컬 스크립트용 |

기존 GET `/download` 계약 (PRD F-05):

| 메서드 | 경로 | Content-Type | Body |
|--------|------|--------------|------|
| GET | `/download` | `text/csv; charset=UTF-8` | BOM + `text\n` + 필터 결과 |

### 2.4 테스트

| ID | 설명 |
|----|------|
| FH-01 | Feedback 1건 저장 — 파일 존재, BOM+`text\n`+본문, `return true` |
| FH-02 | 빈 vector — BOM+`text\n`만, `return true` |
| FH-03 | 개행 포함 text — `\n` 유지, `CsvExporter` 출력과 바이트 일치 |
| FH-04 | 쓰기 불가 path — `return false` |
| FH-05 | 선두 3바이트 = `EF BB BF` (UTF-8 BOM) |

```powershell
ctest --test-dir build -R "FileHandlerTest" --output-on-failure
# 기대: 5/5 Pass
```

---

## 3. 감성 분석 가중치 스코어링

### 3.1 API

```cpp
// src/cpp/feedback/SentimentClassifier.h
struct SentimentScore { int positive{0}; int negative{0}; };

class SentimentClassifier {
public:
    static SentimentScore scoreWeighted(const std::string& text);
    static std::string classifyWeighted(const std::string& text);
    static std::string classify(const std::string& text);  // classifyWeighted() 위임
};
```

### 3.2 분류 규칙 (PRD F-06 개정)

키워드 소스: `Constants::SENTIMENT_KEYWORDS[u8"긍정"]`, `[u8"부정"]` **ONLY**

| 단계 | 규칙 |
|------|------|
| 1 | 긍정 키워드 목록 순회 — 텍스트에 포함 시 `score.positive++` |
| 2 | 부정 키워드 목록 순회 — 텍스트에 포함 시 `score.negative++` |
| 3 | `positive > negative` → **긍정** |
| 4 | `negative > positive` → **부정** |
| 5 | 동점(0 포함) → **중립** |

**Before (first-match):** 긍정 키워드 1건이라도 매칭되면 즉시 `"긍정"` 반환 → 혼합 감성 오분류.

**After (weighted):** 전체 키워드 집계 후 건수 비교 → 혼합 텍스트 정확도 개선.

### 3.3 호출 경로 (단일 소스)

```
TextAnalyzer / Filters
        ↓
FeedbackClassifier::classifySentiment()
        ↓
SentimentClassifier::classify() → classifyWeighted()
        ↓
Constants::SENTIMENT_KEYWORDS[u8"긍정"|u8"부정"]
```

### 3.4 HTTP 라우트

| 변경 | 설명 |
|------|------|
| **없음** | Domain 교체만. POST `/analyze`, POST `/filter` 응답 HTML은 동일 path·method |

### 3.5 테스트

| ID | Given | Then |
|----|-------|------|
| SC-W01 | `"좋아요 만족"` | `"긍정"`, positive>negative |
| SC-W02 | `"나쁘 불만"` | `"부정"` |
| SC-W03 | `"그냥 그래요"` | `"중립"` (T-03) |
| SC-W04 | 긍정2 + 부정1 혼합 | `"긍정"` |
| SC-W05 | 긍정1 + 부정2 혼합 | `"부정"` |
| SC-W06 | 긍정1 + 부정1 동점 | `"중립"` |
| SC-W01b | `classify()` | `classifyWeighted()`와 동일 |
| EX-09 | `"좋아요 but 나쁘기도 해요"` | **중립=1** (동점, first-match「긍정 우선」에서 교체) |

```powershell
ctest --test-dir build -R "SentimentClassifier|TextAnalyzerTest" --output-on-failure
# 기대: SentimentClassifier 7/7 + TextAnalyzer 4/4 Pass
```

### 3.6 EX-09 / Golden Master 영향

| 케이스 | Before (first-match) | After (weighted) | GM baseline |
|--------|---------------------|------------------|-------------|
| EX-09 `"좋아요 but 나쁘기도 해요"` | **긍정** | **중립** | N/A |
| SC-W05 `"좋아요 but 나쁘 불만"` | **긍정** | **부정** | N/A |
| GM-D-01~04 | Pass | Pass | ✅ 변경 없음 |

---

## 4. 변경된 모델·설정

### SentimentScore (신규)

```cpp
struct SentimentScore {
    int positive{0};
    int negative{0};
};
```

### CsvConstants (기존 재사용)

| 상수 | 값 |
|------|-----|
| `kUtf8Bom` | `\xEF\xBB\xBF` |
| `kTextColumnHeader` | `text\n` |
| `kAttachmentFilename` | `filtered_feedback.csv` |
| `kContentType` | `text/csv; charset=UTF-8` |

### PRD F-06 (개정)

- **Before:** 긍정 키워드 매칭 → 부정 → 중립 fallback
- **After:** `SentimentClassifier::classifyWeighted` 가중치 스코어링 (§3.2 표 참조)

### Feedback

- **변경 없음** — 트렌드용 `date` 필드·File DB 연동은 **미구현** (본 프로젝트 범위 외)

---

## 5. 사용 방법

### 5.1 웹 앱 (기존 HTTP)

```powershell
cmake -S . -B build
cmake --build build
build\feedback_analyzer.exe
```

1. 브라우저 `http://localhost:8080` 접속
2. 피드백 입력·CSV 업로드 → **감정 분포**·키워드 통계 확인 (가중치 규칙 자동 적용)
3. 필터 후 GET `/download` — BOM CSV 다운로드 (변경 없음)

### 5.2 FileHandler (프로그램/API 호출)

```cpp
#include "FileHandler.h"

std::vector<Feedback> data = { Feedback(u8"좋아요 만족합니다.") };
const bool ok = FileHandler::saveToCsv(data, "output/filtered.csv");
// ok == true → output/filtered.csv (BOM + text\n + 본문)
```

### 5.3 SentimentClassifier (Domain 직접 호출)

```cpp
#include "SentimentClassifier.h"

const auto label = SentimentClassifier::classifyWeighted(u8"좋아요 만족 but 나쁘");
// label == u8"중립" (positive=2, negative=1 → 긍정)

const auto score = SentimentClassifier::scoreWeighted(u8"좋아요 but 나쁘");
// score.positive == 1, score.negative == 1
```

### 5.4 테스트 실행

```powershell
ctest --test-dir build --output-on-failure
# 기대: 44 passed, 0 failed, 1 skipped

# Feature 신규 테스트만
ctest --test-dir build -R "FileHandlerTest|SentimentClassifier" -V
# 기대: 12/12 Pass (FH-01~05 + SC-W01~06 + SC-W01b)
```

---

## 6. 회귀·호환성

| 항목 | 상태 |
|------|------|
| Golden Master GM-D-01~04 | ✅ 4/4 Pass (baseline 미갱신) |
| HTTP 5엔드포인트 | ✅ path·method 불변 |
| `httplib.h` | ✅ diff 0 |
| CSV 입력 (`text` 단독) | ✅ 기존과 동일 |
| GET `/download` | ✅ `CsvExporter` 메모리 응답 유지 |
| Filters AND 로직 | ✅ 변경 없음 |
| 카테고리(품질/배송) vs 감성 분리 | ✅ T-07 유지 |
| ctest 전체 | ✅ **44** test cases Green (1 Skipped: `GoldenMasterCapture`) |

### 미구현 (참고 템플릿 대비)

본 프로젝트 **연습 #4** 범위에는 아래 항목이 **포함되지 않는다**.

| 기능 | 상태 |
|------|------|
| 감정 트렌드 시각화 (`date` 컬럼·`TrendAnalyzer`) | ❌ 미구현 |
| 감정 키워드 File DB (`SentimentKeywordDb`) | ❌ 미구현 |
| POST `/trend/load-sample`, POST `/admin/sentiment` | ❌ 미구현 |

감성 키워드는 `Constants::SENTIMENT_KEYWORDS` (코드 내 단일 소스)로 관리한다.

---

## 7. 관련 문서

| 문서 | 내용 |
|------|------|
| `docs/PRD.md` §3.2 F-05, F-06 | CSV 다운로드·감성 분류 규칙 |
| `docs/test_plan.md` EX-09 | 혼합 감성 동점 → 중립 |
| `Report/Feature/20.*` | FileHandler TDD 상세 |
| `Report/Feature/21.*` | SentimentClassifier TDD 상세 |
| `Prompting/Feature/20.*`, `21.*` | 대화형 프롬프트 Transcript |

---

*본 문서는 Feedback Analyzer **연습 #4 추가 요구사항 2건**(FileHandler saveToCsv · 감성 가중치 스코어링)의 구현 결과 요약이다.*
