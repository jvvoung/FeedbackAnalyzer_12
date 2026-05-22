# Feedback Analyzer — Product Requirements Document (PRD)

| 항목 | 내용 |
|------|------|
| 문서 버전 | 1.1 |
| 작성일 | 2026-05-21 |
| 최종 갱신 | 2026-05-22 (`docs/analysis.md` v2.0 정합) |
| 워크스페이스 | `c:\DEV\FeedbackAnalyzer_12` |
| 근거 문서 | `project_purpose.md`, `docs/analysis.md` (v2.0), `docs/test_plan.md`, `README.md`, `.cursorrules` |

---

## 1. 프로젝트 개요

### 1.1 한 줄 목적문 (What / Who / Why)

| 구분 | 내용 |
|------|------|
| **What** | 자연어 고객 피드백을 수집·분류·시각화하는 C++17 웹 애플리케이션 |
| **Who** | (1) 웹 UI로 피드백을 분석하는 운영/CS 담당자, (2) TDD·Clean Code로 리팩토링을 학습하는 중급 C++ 개발자 |
| **Why** | 의도적 코드 스멜이 포함된 레거시 코드를 테스트 기반으로 점진 개선하며, 유지보수 가능한 계층 구조를 학습한다 (`project_purpose.md` §1.2, §1.3) |

### 1.2 배경 및 문제 정의

**현재 상태** (`docs/analysis.md` §1~§3):

| 문제 영역 | 구체적 내용 | 근거 |
|-----------|-------------|------|
| God Function | `main.cpp`에 HTTP 라우팅·HTML 렌더링·CSV 파싱·오케스트레이션이 혼재 (372줄) | `docs/analysis.md` §3.6 (M-1), `project_purpose.md` §4.2 |
| 전역 상태 | `Session::currentFeedbacks`, `fil_data`, `TextAnalyzer::globalSent/globalKw` | `docs/analysis.md` §2.2.3, §3.4 (M-2), 부록 B.5 |
| 중복 로직 | `containsAny`가 TextAnalyzer·Filters에 각각 존재 | `docs/analysis.md` §3.7 (M-3), `project_purpose.md` §4.1 |
| 감정 키워드 이중 관리 | `Constants::SENTIMENT_KEYWORDS` vs `Filters::S_KEYWORDS` | `docs/analysis.md` §2.1.2 (H-1, M-7) |
| P0 기능 버그 | H-1 중립 필터 불일치, H-2 CSV `text` 컬럼 미사용, H-3 키워드 필터 `main` 누락 | `docs/analysis.md` §2.1, §1.1 |
| 테스트 부재 | Google Test·CMake 테스트 타깃 0건 | `docs/analysis.md` §2.1.4 (H-4), 부록 B.6 |

**PRD가 해결하는 범위:** 위 P0 버그 수정, 테스트 인프라 구축, 계층 분리·상태 캡슐화를 단계(Phase)별로 완료하여 `project_purpose.md` §6.1 학습 목표를 충족한다.

### 1.3 목표 (측정 가능)

| ID | 목표 | 측정 기준 | 근거 |
|----|------|-----------|------|
| G-1 | HTTP API 계약 유지 | 5개 엔드포인트(path·method) 변경 0건; 부록 체크리스트 5항목 통과 | `docs/analysis.md` 부록 A, `.cursorrules` §3 |
| G-2 | P0 버그 수정 및 회귀 방지 | H-1/H-2/H-3 각각 재현 단위 테스트 1건 이상 + Green | `docs/analysis.md` §2.1, 부록 C Phase 1 |
| G-3 | Service 계층 테스트 커버리지 | TextAnalyzer·Filters·CsvParser **≥ 90%** (gcov/lcov 또는 동등) | `project_purpose.md` §6.1-2, `.cursorrules` §5 |
| G-4 | main.cpp 책임 축소 | HTML·핸들러 분리 후 `main.cpp` **≤ 200줄** | `docs/analysis.md` 부록 C Phase 2 |
| G-5 | Logger level별 UI 표시 | warning·error가 HTML alert에 반영; success는 타임스탬프 포함 | `project_purpose.md` §6.1-3, `docs/analysis.md` §2.2.4 (M-4) |

### 1.4 비목표

| ID | 비목표 | 사유 |
|----|--------|------|
| NG-1 | ML/NLP 기반 감정 분석 도입 | 본 프로젝트는 키워드 매칭 기반 분류 유지 (`project_purpose.md` §2.1) |
| NG-2 | 다중 사용자 인증·권한 시스템 | 로컬 실습·리팩토링 챌린지 범위 (`docs/analysis.md` §2.3 L-5) |
| NG-3 | 프로덕션급 RDBMS 도입 | 인메모리 Session 유지; File DB는 선택 과제로만 허용 (`project_purpose.md` §6.1-7, `.cursorrules` §7) |

---

## 2. 사용자 및 이해관계자

### 2.1 타깃 사용자 (페르소나)

**페르소나: 김리팩 (32세, 중급 C++ 개발자 / CS 운영 겸임)**

| 항목 | 내용 |
|------|------|
| 역할 | 고객 피드백 CSV를 업로드해 감정·키워드별 건수를 확인하고, 필터 결과를 다운로드 |
| 학습 목표 | God Function·전역 상태·중복 로직을 TDD로 안전하게 리팩토링 |
| Pain Point | 필터 결과가 분석 집계와 불일치(H-1/H-3), CSV `text` 컬럼이 무시됨(H-2), 테스트 없어 변경 시 불안 |
| 성공 기준 | `ctest` Green + 브라우저에서 5개 엔드포인트 시나리오 통과 |

근거: `project_purpose.md` §1.4, §2.2

### 2.2 주요 시나리오

#### 시나리오 A — 텍스트 입력 → 분석 집계 확인

1. 브라우저에서 `http://localhost:8080` 접속 (`README.md` 사용 방법 §1)
2. textarea에 피드백 입력 후 POST `/analyze` 제출 (`project_purpose.md` §2.2)
3. 감정 분포(긍정/중립/부정 건수) 및 키워드 분포(배송·품질·가격·서비스·사용성 건수) 확인 (`project_purpose.md` §2.3)
4. success alert에 타임스탬프 + 입력 완료 메시지 표시

#### 시나리오 B — CSV 업로드 → 필터 → 다운로드

1. `text` 컬럼을 포함한 CSV 업로드 (POST `/upload`, multipart `file`) (`README.md` CSV 파일 형식)
2. POST `/filter`로 sentiment=`중립`, keyword=`배송` 등 조합 필터
3. 필터 결과 집계 및 피드백 목록(화면 stat) 확인
4. GET `/download`로 UTF-8 BOM CSV 수신; 파일명 `filtered_feedback.csv`

#### 시나리오 C — 리팩토링 학습자: 테스트 → 버그 수정 → 구조 분리

1. Google Test 타깃 추가 후 TextAnalyzer·Filters·CsvParser 단위 테스트 작성 (`docs/analysis.md` 부록 C Phase 1, `docs/test_plan.md` §8)
2. H-1/H-2/H-3 failing test → 수정 → Green (`docs/analysis.md` §1.1 P0, §5.1)
3. Green 상태 유지하며 HtmlRenderer·RouteHandlers 분리 (`docs/analysis.md` 부록 C Phase 2)
4. 전역 상태 캡슐화·네이밍 개선 (`docs/analysis.md` 부록 C Phase 3)

---

## 3. 기능 요구사항

### 3.1 기능 목록

| 우선순위 | 기능 ID | 기능 | 설명 | 근거 |
|----------|---------|------|------|------|
| **필수** | F-01 | 대시보드 | GET `/` — UTF-8 한국어 HTML 대시보드 | `docs/analysis.md` 부록 B.4 |
| **필수** | F-02 | 텍스트 분석 | POST `/analyze` — 피드백 추가 + 감정·키워드 집계 | `project_purpose.md` §2.1 |
| **필수** | F-03 | CSV 업로드 | POST `/upload` — 헤더 `text` 컬럼 파싱 | `README.md` CSV 형식, `docs/analysis.md` §2.2.1 (H-2) |
| **필수** | F-04 | 필터 | POST `/filter` — 감정·키워드 필터 + 집계 | `project_purpose.md` §2.1 |
| **필수** | F-05 | CSV 다운로드 | GET `/download` — UTF-8 BOM CSV | `.cursorrules` §3 |
| **필수** | F-06 | 분류 규칙 단일화 | TextAnalyzer·Filters가 동일 감정·키워드 매칭 규칙 사용 | `docs/analysis.md` §2.1 (H-1, H-3, M-7) |
| **권장** | F-07 | 단위 테스트 | Google Test + CMake `ctest` 타깃 | `docs/analysis.md` §2.1.4 (H-4) |
| **권장** | F-08 | 관심사 분리 | HtmlRenderer·RouteHandlers·CsvParser 분리 | `docs/analysis.md` 부록 C Phase 2 |
| **권장** | F-09 | 상태 캡슐화 | 전역 상태 → AppState/Repository | `docs/analysis.md` 부록 C Phase 3 |
| **권장** | F-10 | Logger UI 연동 | level별(warning/error/success) alert 표시 | `project_purpose.md` §6.1-3 |
| **권장** | F-11 | 멀티라인 입력 검증 | textarea 개행 포함 입력·저장·분석 E2E 검증 | `project_purpose.md` §6.1-3, `docs/analysis.md` §2.3 (L-6) |
| **선택** | F-12 | Trend 시각화 | `test_feedback_trend.csv` 기반 추이 표시 | `project_purpose.md` §6.1-7 |
| **선택** | F-13 | File DB | 감정 필터 키워드 File DB CRUD | `project_purpose.md` §6.1-7 |
| **선택** | F-14 | 업로드 검증 | localhost 바인딩 옵션, CSV MIME·크기 제한 | `docs/analysis.md` §2.3 (L-5) |

### 3.2 기능별 입출력 계약

#### F-01 GET `/`

| 항목 | 계약 |
|------|------|
| Request | Method: `GET`, Path: `/`, Body: 없음 |
| Response Status | `200 OK` |
| Response Content-Type | `text/html; charset=UTF-8` |
| Response Body | HTML 대시보드: 제목 "Feedback Analyzer", 섹션(피드백 입력·CSV 업로드·필터·분석 결과), 한국어 UI (`docs/analysis.md` 부록 A) |
| Side Effect | 세션 상태 초기화 호출 (`docs/analysis.md` 부록 B.4) |
| Alert | success=`피드백 분석기 시작` (타임스탬프 prefix 포함) |

#### F-02 POST `/analyze`

| 항목 | 계약 |
|------|------|
| Request | Method: `POST`, Path: `/analyze`, Content-Type: `application/x-www-form-urlencoded` |
| Request Fields | `text` (string): 피드백 본문; 빈 문자열·공백만인 경우 추가하지 않음 |
| Response Status | `200 OK` (예외 시에도 HTML error alert와 함께 200) |
| Response Content-Type | `text/html; charset=UTF-8` |
| Response Body | 입력 후 감정 집계 map + 키워드 집계 map을 stat 영역에 표시 |
| 감정 집계 키 | `긍정`, `중립`, `부정` (UTF-8, 값=건수 int) |
| 키워드 집계 키 | `배송`, `품질`, `가격`, `서비스`, `사용성` (UIComponents 카테고리, 값=건수 int) |
| Alert — success | `{N}개의 피드백이 입력되었습니다.` + 타임스탬프 |
| Alert — error | `처리 중 오류가 발생했습니다.` |
| 분류 규칙 | F-06과 동일 (`docs/analysis.md` §2.1.2 (H-1)) |

**입출력 예** (`project_purpose.md` §2.3):

| 입력 | 출력 |
|------|------|
| `"배송이 너무 늦어요. 화가 납니다."` | 감정: 부정, 카테고리: 배송 (집계 stat에 반영) |

#### F-03 POST `/upload`

| 항목 | 계약 |
|------|------|
| Request | Method: `POST`, Path: `/upload`, Content-Type: `multipart/form-data` |
| Request Fields | `file` (file): CSV 파일, 확장자 `.csv` |
| CSV 입력 규칙 | 1행=헤더; **`text` 컬럼 필수**; 데이터 행의 `text` 값을 Feedback으로 적재 (`README.md` CSV 형식) |
| `text` 컬럼 없음 | 업로드 실패 처리: error alert `파일 업로드 중 오류가 발생했습니다.` 또는 warning `CSV에 text 컬럼이 없습니다.` (구현 시 error 우선) |
| Response Status | `200 OK` |
| Response Content-Type | `text/html; charset=UTF-8` |
| Alert — success | `{N}개의 피드백이 입력되었습니다.` |
| Alert — error | `파일 업로드 중 오류가 발생했습니다.` |

**입출력 예** (`project_purpose.md` §2.3):

| 입력 | 출력 |
|------|------|
| `feedbacks.csv` (`text` 컬럼 포함) | 전체 피드백 분석 결과 테이블·집계 stat (후속 `/analyze` 또는 `/filter`와 동일 Session에 누적) |

#### F-04 POST `/filter`

| 항목 | 계약 |
|------|------|
| Request | Method: `POST`, Path: `/filter`, Content-Type: `application/x-www-form-urlencoded` |
| Request Fields | `sentiment` (string), `keyword` (string) |
| sentiment 허용값 | `전체`, `긍정`, `중립`, `부정` |
| keyword 허용값 | `전체`, `배송`, `품질`, `가격`, `서비스`, `사용성` |
| sentinel | `전체` = 해당 축 필터 미적용 (`.cursorrules` §3) |
| Response Status | `200 OK` |
| Response Content-Type | `text/html; charset=UTF-8` |
| 필터 성공 (≥1건) | 필터된 subset에 대한 감정·키워드 집계 stat + 다운로드 버튼 표시 |
| Alert — warning (0건) | `필터링 결과가 없습니다.` |
| Alert — warning (피드백 없음) | `분석할 피드백이 없습니다.` |
| Alert — error | `처리 중 오류가 발생했습니다.` |
| H-1 수정 후 계약 | sentiment=`중립` 필터 결과 집합 = TextAnalyzer가 동일 입력에 대해 `중립`으로 분류한 Feedback 집합과 **완전 일치** |
| H-3 수정 후 계약 | keyword=`배송` 필터 시 `Constants::CATEGORY_KEYWORDS["배송"]["main"]` 키워드만 포함된 피드백 **포함** |

**입출력 예** (`project_purpose.md` §2.3):

| 입력 | 출력 |
|------|------|
| keyword=`배송` | 배송 관련 피드백 N건 stat 및 목록 |

#### F-05 GET `/download`

| 항목 | 계약 |
|------|------|
| Request | Method: `GET`, Path: `/download`, Body: 없음 |
| Response Status | `200 OK` |
| Response Content-Type | `text/csv; charset=UTF-8` |
| Response Header | `Content-Disposition: attachment; filename="filtered_feedback.csv"` |
| Response Body 형식 | Byte 1–3: UTF-8 BOM `\xEF\xBB\xBF`; 4행째: 헤더 `text\n`; 이후: 피드백 텍스트 1행 1건 (`.cursorrules` §3) |
| **다운로드 대상 (PRD 확정)** | **마지막 POST `/filter` 성공(≥1건 반환) 직후의 필터 결과 집합** (`docs/analysis.md` §2.2.2 M-6, 부록 B.5 `fil_data`) |
| `/filter` 미실행 또는 마지막 필터 0건 | BOM + `text\n` 헤더만, 데이터 행 0건; UI에서 warning `다운로드할 필터 결과가 없습니다.` (Phase 4 F-10과 함께 구현) |

#### F-06 분류 규칙 단일화 (교차 기능)

| 규칙 | 내용 |
|------|------|
| 감정 단일 소스 | `Constants::SENTIMENT_KEYWORDS`만 사용; `Filters::S_KEYWORDS`·`Filters::initFilterKeywords()` 제거 | `docs/analysis.md` §3.2 (M-7) |
| 감정 판정 순서 | 긍정 키워드 매칭 → 부정 키워드 매칭 → 그 외 **중립** (TextAnalyzer·Filters 동일) | `docs/analysis.md` §2.1.2 (H-1) |
| 키워드 카테고리 매칭 | `Constants::CATEGORY_KEYWORDS[카테고리]["main"]` 키워드 포함 시 해당 카테고리 매칭 (집계·필터 동일 규칙) | `docs/analysis.md` §2.1.1 (H-3) |
| quality 혼동 금지 | `서비스`/`품질` 카테고리 키워드와 감정 3분류를 혼동하지 않음 (`.cursorrules` §3) |

### 3.3 제약 사항

| ID | 제약 | 근거 |
|----|------|------|
| C-01 | 감정 라벨 고정: `긍정`, `중립`, `부정` (UTF-8, `u8"..."`) | `.cursorrules` §3 |
| C-02 | 필터 sentinel: `전체` | `.cursorrules` §3 |
| C-03 | CSV 입력: 1행 헤더, `text` 컬럼 필수 | `README.md` CSV 형식 |
| C-04 | CSV 출력: BOM + `text\n` + 본문 | `.cursorrules` §3, `docs/analysis.md` 부록 A |
| C-05 | 서버 URL: `http://localhost:8080`; 바인딩: `0.0.0.0:8080` | `README.md`, `docs/analysis.md` 부록 B.6 |
| C-06 | HTTP 5엔드포인트 path·method 변경 금지 (PRD 개정 없이) | `.cursorrules` §3, §7.2 |
| C-07 | `src/cpp/httplib.h` 수정 금지 | `.cursorrules` §1 |
| C-08 | 빈 피드백·필터 0건·업로드 실패 시 warning/error alert 계약 준수 | §3.2 각 F-xx |
| C-09 | 리팩토링은 failing test 0건(Green) 상태에서만 진행 | `.cursorrules` §5, §6 |

---

## 4. 비기능 요구사항

### 4.1 기술 스택

| 항목 | 요구 | 근거 |
|------|------|------|
| 언어 | C++17 | `README.md` 요구사항, `docs/analysis.md` 부록 B.2 |
| 빌드 | CMake 3.14+ | `README.md`, `docs/analysis.md` 부록 B.2 |
| HTTP | cpp-httplib (헤더 전용) | `docs/analysis.md` 부록 B.2 |
| 테스트 | Google Test + CMake `enable_testing()` | `.cursorrules` §5 |
| 커버리지 | gcov/lcov 또는 동등 도구 | `.cursorrules` §5 |
| 컴파일러 | MinGW GCC / MSVC / Clang | `README.md` |
| Windows | `_WIN32_WINNT=0x0A00`, `ws2_32` 링크 | `.cursorrules` §1 |

### 4.2 아키텍처 원칙

| 계층 | 책임 | 의존 방향 |
|------|------|-----------|
| Controller | HTTP 라우트·요청 파싱·응답 조립 | → Service, View, State |
| Service | TextAnalyzer(감정·키워드), Filters | → Domain, Constants |
| View | HtmlRenderer (HTML 문자열 생성) | → Domain (표시용) |
| State | Session / AppState (피드백·필터 결과) | → Domain |
| Domain | Feedback | 독립 |
| Config | Constants (키워드 단일 소스) | 독립 |

- **SRP:** `main.cpp`에 HTML·파싱·비즈니스 로직 추가 금지 (`.cursorrules` §2)
- **의존성 방향:** main → handlers → services → domain(Feedback) → Constants
- **리팩토링 원칙:** 테스트 Green → 최소 diff → Extract Function/Class (`docs/analysis.md` 부록 C, `.cursorrules` §6)

### 4.3 테스트 커버리지 목표

| 대상 | 목표 | 스타일 | 근거 |
|------|------|--------|------|
| TextAnalyzer | ≥ 90% | Given-When-Then, TEST/TEST_F | `.cursorrules` §5 |
| Filters | ≥ 90% | 동일 | `.cursorrules` §5 |
| CsvParser | ≥ 90% | 동일 | `.cursorrules` §5 |
| Feedback | ≥ 90% | 동일 | `.cursorrules` §5 |
| HTTP 핸들러 | 통합/스모크 또는 부록 체크리스트 수동 검증 | 5항목 | `docs/analysis.md` 부록 A |

**필수 경계값 테스트 케이스:**

| # | Given | When | Then |
|---|-------|------|------|
| T-01 | 빈 Feedback 목록 | analyzeSentiment | 긍정=0, 중립=0, 부정=0 |
| T-02 | 단일 피드백 1건 | analyze + filter | 집계·필터 건수 1 |
| T-03 | 긍정/부정 키워드 없는 텍스트 | filter sentiment=중립 | TextAnalyzer 중립 분류와 동일 집합 |
| T-04 | sentiment=전체, keyword=전체 | filter | 입력 전체 반환 |
| T-05 | CSV 헤더에 `text` 없음 | parse | 오류 또는 빈 적재 (0건) |
| T-06 | `배송` main 키워드만 포함 | filter keyword=배송 | 해당 Feedback 포함 |
| T-07 | 품질 키워드만 포함, 감정 키워드 없음 | analyzeSentiment | 감정=중립 (품질 카테고리와 분리) |

### 4.4 확장성 원칙

| 원칙 | 내용 | 근거 |
|------|------|------|
| 키워드 단일 소스 | 카테고리 추가 시 `Constants::CATEGORY_KEYWORDS` + `UIComponents::CATS`만 변경 | `project_purpose.md` §4.2 Shotgun Surgery |
| 알고리즘 교체 | 감정 분석 Strategy를 Service 계층에 한정; Controller·View 무변경 | `project_purpose.md` §5.2 |
| 선택 기능 격리 | Trend·File DB는 Phase 5, 명시 요청 시에만 | `.cursorrules` §7 |

---

## 5. 데이터 요구사항

### 5.1 도메인 데이터

| 엔티티 | 필드 | 필수 | 설명 | 근거 |
|--------|------|------|------|------|
| **Feedback** | `text` (string) | Y | 피드백 본문 | `Feedback.h`, `project_purpose.md` §3.1 |
| Feedback (권장) | `sentiment`, `category` | N | 분석 결과 필드 — Phase 3 데이터 클래스 개선 시 추가 | `project_purpose.md` §5.2 |
| **Constants::SENTIMENT_KEYWORDS** | map: `긍정`/`중립`/`부정` → vector\<string\> | Y | 감정 키워드 **단일 소스** | `.cursorrules` §3, `docs/analysis.md` §3.2 (M-7) |
| **Constants::CATEGORY_KEYWORDS** | map: 카테고리 → map: `main`/`sub` → vector\<string\> | Y | 키워드 카테고리 | `Constants.cpp`, `docs/analysis.md` 부록 B.5 |

**카테고리 목록 (고정):** `배송`, `품질`, `가격`, `서비스`, `사용성` (`UIComponents.cpp`, `project_purpose.md` §2.3)

### 5.2 세션·상태

| 상태 | 저장소 | 수명 | 용도 |
|------|--------|------|------|
| 전체 피드백 | `Session::currentFeedbacks` | 프로세스 수명, 인메모리 | `/analyze`, `/upload` 누적 (`docs/analysis.md` 부록 B.5) |
| 마지막 필터 결과 | `fil_data` → Phase 3 `AppState::lastFilteredFeedbacks` | `/filter` 성공 시 갱신 | GET `/download` 소스 (§3.2 F-05) |
| 분석 캐시 (제거 대상) | `TextAnalyzer::globalSent/globalKw` | — | Phase 3에서 제거 (`docs/analysis.md` 부록 C Phase 3) |
| 미사용 (제거 대상) | `Session::internalData`, `filterOptions` | — | Phase 3 정리 (`docs/analysis.md` §3.3 (L-2)) |

### 5.3 CSV 계약

| 방향 | 스키마 | 규칙 |
|------|--------|------|
| **입력** | 헤더: `text` (필수); 추가 컬럼 허용·무시 | `README.md` CSV 형식, `docs/analysis.md` §2.2.1 (H-2) |
| **출력** | BOM + `text\n` + `{피드백텍스트}\n` × N | `.cursorrules` §3, §3.2 F-05 |

### 5.4 (선택) File DB 계약 — Phase 5

| 항목 | 계약 (선택 과제 F-13) |
|------|----------------------|
| 저장 대상 | `Constants::SENTIMENT_KEYWORDS` (긍정/중립/부정 키워드 목록) |
| 저장 위치 | 프로젝트 루트 `data/sentiment_keywords.json` (또는 `.csv`) — 구현 시 PRD 부록 개정 |
| 기동 시 | 파일 존재 → 로드; 없음 → `Constants::init()` 기본값 + 파일 생성 |
| CRUD | 웹 UI 또는 CLI로 키워드 추가·삭제; 변경 후 `/filter`·집계에 즉시 반영 |
| 근거 | `project_purpose.md` §6.1-7 |

---

## 6. 출력 요구사항

### 6.1 HTML 대시보드 (필수)

| 섹션 | UI 요소 | 계약 |
|------|---------|------|
| 피드백 입력 | `<textarea name="text">`, POST `/analyze` | 멀티라인 입력 지원 (`project_purpose.md` §6.1-3) |
| CSV 업로드 | `<input type="file" name="file">`, POST `/upload` | accept `.csv` |
| 필터 | `<select name="sentiment">`, `<select name="keyword">`, POST `/filter` | 허용값 §3.2 F-04 |
| 분석 결과 | 감정 stat + 키워드 stat (`.stat-number`, `.stat-label`) | 건수 int 표시 |
| 다운로드 | `<a href="/download">` 버튼 | F-05 |
| Alert — success | `.alert-success` | 타임스탬프 + 메시지 (`YYYY-MM-DD HH:MM:SS : {message}`) |
| Alert — warning | `.alert-warning` | warning 메시지 (필터 0건 등) |
| Alert — error | `.alert-danger` | error 메시지 |
| Content-Type | `text/html; charset=UTF-8` | `docs/analysis.md` 부록 A |

### 6.2 CSV 다운로드 스키마 (필수)

```
[EF BB BF]text\n
{line1}\n
{line2}\n
...
```

| 항목 | 값 |
|------|-----|
| BOM | `\xEF\xBB\xBF` |
| 헤더 | `text` |
| Content-Disposition | `attachment; filename="filtered_feedback.csv"` |
| 인코딩 | UTF-8 |

### 6.3 (선택) Trend 시각화 출력 — F-12

| 항목 | 계약 |
|------|------|
| 입력 파일 | `test_feedback_trend.csv` (프로젝트 루트 또는 `data/`) |
| 차트 타입 | 시계열 선 그래프 (일별 또는 행 순서별) |
| X축 | 날짜 또는 순번 |
| Y축 | 감정별 건수(긍정/중립/부정) 또는 카테고리별 건수 |
| 출력 위치 | HTML 대시보드 "Trend" 섹션 |
| 근거 | `project_purpose.md` §6.1-7 |

### 6.4 로그 UI 출력 (권장) — F-10

| Level | 콘솔 | HTML alert | 정책 |
|-------|------|------------|------|
| INFO | stdout | 표시 안 함 (기본) | |
| WARNING | stdout | `.alert-warning` 표시 | `project_purpose.md` §6.1-3 |
| ERROR | stderr | `.alert-danger` 표시 | `project_purpose.md` §6.1-3 |
| SUCCESS (UI 전용) | — | `.alert-success` + 타임스탬프 | §6.1 |
| DEBUG | stdout (debugMode=true) | 표시 안 함 | `Logger.h` |

---

## 7. 성공 지표

### 7.1 인수 기준 (Acceptance Criteria)

- [ ] **AC-1:** POST `/filter` sentiment=`중립` 결과 집합 = TextAnalyzer 감정 분류 `중립` 집합과 **100% 일치** (H-1, `docs/analysis.md` §2.1.2)
- [ ] **AC-2:** POST `/upload` 시 CSV 헤더 `text` 컬럼 값만 Feedback으로 적재; `text` 없으면 0건 + error/warning (`docs/analysis.md` §2.2.1, H-2)
- [ ] **AC-3:** POST `/filter` keyword=`배송` 시 `main` 키워드("배송","택배" 등)만 포함한 피드백 **포함** (H-3, `docs/analysis.md` §2.1.1)
- [ ] **AC-4:** `ctest --test-dir build --output-on-failure` 전체 Green; TextAnalyzer·Filters·CsvParser 커버리지 **≥ 90%** (`project_purpose.md` §6.1-2)
- [ ] **AC-5:** `docs/analysis.md` 부록 A 엔드포인트 체크리스트 5항목 통과
- [ ] **AC-6 (권장):** warning·error Logger 호출 시 HTML alert에 동일 메시지 표시 (`project_purpose.md` §6.1-3)
- [ ] **AC-7 (권장):** textarea 개행 포함 입력 → Session 저장 → CSV 다운로드 시 개행 유지 (`docs/analysis.md` §2.3 (L-6))

### 7.2 회귀 보호 규칙

| 규칙 | 내용 |
|------|------|
| R-1 | HTTP 5엔드포인트 path·method 변경 금지 (`/`, `/analyze`, `/upload`, `/filter`, `/download`) |
| R-2 | 감정 3분류 한국어 라벨(`긍정`,`중립`,`부정`) 변경 금지 |
| R-3 | CSV 출력 BOM·`text\n` 헤더 형식 변경 금지 |
| R-4 | 리팩토링 PR마다 `ctest` Green 유지; Red 상태 merge 금지 |
| R-5 | `httplib.h` diff 0 |

---

## 8. 용어 정의 (Glossary)

| 용어 | 정의 |
|------|------|
| **Feedback** | 사용자가 입력하거나 CSV에서 적재한 피드백 텍스트 1건을 나타내는 도메인 객체 (`Feedback.h`) |
| **Sentiment (감정)** | 피드백 텍스트를 `긍정`·`중립`·`부정` 중 하나로 분류한 결과 (`.cursorrules` §3) |
| **Category Keyword (키워드 카테고리)** | `배송`·`품질`·`가격`·`서비스`·`사용성` 등 Constants 기반 주제 분류 |
| **God Function** | `main.cpp`처럼 라우팅·HTML·파싱·오케스트레이션이 한 함수/파일에 집중된 안티패턴 (`project_purpose.md` §4.2) |
| **Session** | `Session::currentFeedbacks` 등 인메모리 전역 상태를 보관하는 클래스 (`docs/analysis.md` 부록 B.5) |
| **fil_data** | POST `/filter` 성공 시 저장되는 필터 결과 벡터; GET `/download`의 현재 소스 (`main.cpp`, §5.2) |
| **UTF-8 BOM** | CSV 다운로드 파일 선두 3바이트 `\xEF\xBB\xBF`; Excel UTF-8 인식용 (`.cursorrules` §3) |
| **Given-When-Then** | 테스트 시나리오 서술 형식: 전제·행동·기대 결과 (`.cursorrules` §5) |
| **sentinel `전체`** | 필터에서 해당 축(감정 또는 키워드) 조건을 적용하지 않음을 의미하는 특수 값 |

---

## 9. 학습 Phase 매핑

`project_purpose.md` §6.1 8단계와 `docs/analysis.md` 부록 C Phase 1~5 대응표.

| 학습 단계 | project_purpose §6.1 | analysis Phase | 목표 | 산출물 | 인수 기준 (1줄) |
|-----------|---------------------|----------------|------|--------|-----------------|
| L-0 | 1. 개요·실습 준비 (1h) | — | 프로젝트·PRD·analysis 숙지 | `docs/PRD.md`, `docs/analysis.md` | 팀원이 5엔드포인트·P0 버그 3건 설명 가능 |
| L-1 | 2. 테스트 구조 (2h) | **Phase 1** | GTest + P0 버그 수정 | `tests/*`, CMake test 타깃 | AC-1~AC-4 중 AC-4 Green + H-1~H-3 테스트 통과 |
| L-2 | 3. 오류 개선 (1.5h) | **Phase 1·4** | 중립 필터·Logger UI·textarea | TextAnalyzer, Filters, Logger | AC-1, AC-6, AC-7 충족 |
| L-3 | 4. 네이밍·상수·전역 (1h) | **Phase 3** | Rename·Constants 단일화·AppState | Session, TextAnalyzer, Filters | `sent/kw/fil` 제거; globalSent/Kw 제거 |
| L-4 | 5. 긴 함수·중복 (1.5h) | **Phase 2** | Extract Function/Class | HtmlRenderer, CsvParser, RouteHandlers | main.cpp ≤ 200줄; containsAny 단일화 |
| L-5 | 6. 리팩토링 1건 추가 (1h) | **Phase 3·4** | FileHandler 정리·download 대상 명확화 | FileHandler 또는 제거 | fil_data/AppState 단일 경로; M-5·M-6 해소 |
| L-6 | 7. 추가 요구 (3h) | **Phase 5** | Trend·File DB | Trend UI, sentiment_keywords 파일 | F-12 또는 F-13 사용자 요청 시 AC 정의 |
| L-7 | 8. 팀 리뷰·발표 (2h) | — | 상호 리뷰 | 리뷰 노트 | AC-5 체크리스트 + 커버리지 리포트 제출 |

### Phase별 상세 (analysis.md 부록 C 기준)

| Phase | 목표 | 핵심 산출물 | 검증 |
|-------|------|-------------|------|
| **Phase 1** | 테스트 기반 + H-1/H-2/H-3 수정 | GTest 타깃, TextAnalyzerTest, FiltersTest, CsvParserTest | `ctest` Green; AC-1~AC-3 |
| **Phase 2** | 관심사 분리 | HtmlRenderer, RouteHandlers, CsvParser | Phase 1 Green + HTTP 스모크 |
| **Phase 3** | 상태·네이밍 | AppState, Constants 단일 소스, Rename | 전체 unit test Green |
| **Phase 4** | Logger UI·download·gitignore | LogBuffer, download warning, `.gitignore` | AC-5, AC-6; M-6 해소 |
| **Phase 5** | 선택 과제 | Trend chart, File DB | 사용자 명시 요청 시 |

---

## 부록 A — 엔드포인트 회귀 체크리스트

출처: `docs/analysis.md` 부록 A

- [ ] GET `/` → 200, HTML, UTF-8 한국어 UI
- [ ] POST `/analyze` → 텍스트 추가, 감정 3분류 집계 표시
- [ ] POST `/upload` → CSV `text` 컬럼 파싱
- [ ] POST `/filter` → sentiment=`전체|긍정|중립|부정`, keyword=`전체|배송|품질|가격|서비스|사용성`
- [ ] GET `/download` → UTF-8 BOM + `text\n` 헤더 CSV

## 부록 B — 문서 변경 이력

| 버전 | 일자 | 변경 |
|------|------|------|
| 1.0 | 2026-05-21 | 초안 작성 (`project_purpose.md`, `docs/analysis.md`, `README.md`, `.cursorrules` 기반) |
| 1.1 | 2026-05-22 | `docs/analysis.md` v2.0(통합판) 섹션 참조 정합; `docs/test_plan.md` 시나리오 C 연동 |
