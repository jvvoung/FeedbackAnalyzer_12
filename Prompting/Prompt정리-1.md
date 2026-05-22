# Prompt creation for .cursorrules draft
_Exported on 5/22/2026 at 11:42:11 GMT+9 from Cursor (3.4.20)_

---

**User**

현재 폴더를 기준으로 .cursorrules 초안 생성 요청을 할 프롬포트를 만들건데, 프롬포트를 작성해줘. 
아래글은 예시야.

[P] 당신은 레거시 코드 QA/리팩토링을 돕는 시니어 C++ 엔지니어입니다.
[C] Gilded Rose C++17 프로젝트에서 Cursor AI가 항상 따라야 하는 규칙을
    프로젝트 루트의 .cursorrules로 작성하려고 합니다.
[T] 아래 요구를 만족하는 .cursorrules 내용을 작성해줘.
    - 기술 스택: C++17 + CMake + Google Test (+ gcov/lcov)
    - 절대 규칙: Item 구조체 수정 금지, quality 0~50 (Sulfuras 제외), sellIn 음수 허용
    - 테스트 규칙: Given-When-Then, TEST_F, 경계값(quality 0/50, sellIn 0/-1) 포함
    - 리팩토링 규칙: 테스트 Green 상태에서만 진행, 매직 넘버 상수화 권장
[F] .cursorrules 파일에 그대로 붙여넣을 수 있는 텍스트

---

**Cursor**

현재 프로젝트 구조와 기술 스택을 파악한 뒤, `.cursorrules` 초안 생성용 프롬프트를 작성하겠습니다.


요청하신 **P-C-T-F** 형식으로, 현재 `FeedbackAnalyzer_12` 프로젝트에 맞춘 `.cursorrules` 초안 생성 프롬프트입니다.

---

```
[P] 당신은 레거시 C++ 웹 애플리케이션의 QA/리팩토링을 돕는 시니어 C++ 엔지니어입니다.
    Clean Code, Refactoring, TDD 관점에서 코드 스멜을 식별하고 점진적으로 개선하는 것을 우선합니다.

[C] 현재 작업 디렉터리는 Feedback Analyzer(고객 피드백 분석 시스템) 리팩토링 챌린지 프로젝트입니다.
    Cursor AI가 이 프로젝트에서 항상 따라야 하는 규칙을 프로젝트 루트의 `.cursorrules`로 작성하려고 합니다.
    프로젝트 개요는 다음과 같습니다.
    - 목적: 자연어 피드백 수집·분류·시각화 C++ 웹 앱을 리팩토링하며 클린 아키텍처 학습
    - 기술 스택: C++17, CMake 3.14+, cpp-httplib(헤더 전용), MinGW/MSVC/Clang
    - 실행: `http://localhost:8080`, CSV 업로드(`text` 컬럼 필수), 감정/키워드 필터, CSV 다운로드
    - 주요 소스: `src/cpp/` (main, TextAnalyzer, Filters, Session, Constants, UIComponents, Logger, Feedback, FileHandler, httplib.h)
    - 의도적 코드 스멜: God Function(main.cpp), 전역 상태(Session/fil_data/globalSent/globalKw), 중복 containsAny, 부적절한 네이밍(sent/kw/fil), Lava Flow(FileHandler), HTML·라우팅·비즈니스 로직 혼재
    - 참고 문서: `README.md`, `project_purpose.md` (학습 단계·리팩토링 포인트·버그 수정 항목 포함)

[T] 아래 요구를 만족하는 `.cursorrules` 내용을 작성해줘.

    1) 기술 스택·빌드 규칙
    - C++17, CMake 기준 빌드/실행 안내 (Windows MinGW 예시 포함)
    - `httplib.h`는 외부 라이브러리로 취급하고 불필요하게 수정하지 않음
    - Windows 빌드 시 `_WIN32_WINNT`, `ws2_32` 링크 등 CMakeLists.txt 설정 존중

    2) 아키텍처·책임 분리 규칙
    - HTTP 라우팅, HTML 렌더링, 비즈니스 로직(감정/키워드 분석·필터), 상태 관리, 파일 I/O를 분리하도록 유도
    - `main.cpp`에 모든 로직을 추가하지 말고, Extract Function/Extract Class 방향으로 작은 단위 리팩토링
    - 전역 변수·정적 전역 상태(Session, fil_data, globalSent/globalKw)를 점진적으로 상태 객체/의존성 주입으로 대체
    - `FileHandler`는 죽은 코드 제거 또는 실제 역할 부여 중 하나로 정리

    3) 도메인·동작 불변 규칙 (리팩토링 중에도 깨지면 안 되는 것)
    - 감정 분류: 긍정 / 중립 / 부정 (한국어 라벨, `u8"..."` UTF-8 문자열 유지)
    - 키워드 카테고리는 `Constants` 기반으로 동작
    - CSV 입력: 헤더 `text` 컬럼, 다운로드 시 UTF-8 BOM 포함
    - 알려진 버그 수정 시 회귀 방지: "중립" 필터 오류, 로그 level별 UI 표시, textarea 멀티라인 입력
    - 기존 HTTP 엔드포인트(`/`, `/analyze`, `/upload`, `/filter`, `/download`) 계약 유지

    4) 코딩 컨벤션
    - 의미 있는 이름 사용 (sent/kw/fil 같은 축약 지양)
    - 매직 넘버·하드코딩 키워드는 Constants 또는 enum/구조체로 상수화
    - 중복 로직(containsAny 등)은 한 곳으로 통합
    - UI/HTML 문자열은 UTF-8(`u8"..."`) 일관 유지
    - 불필요한 대규모 재작성보다, 기존 파일/클래스 구조를 존중하는 최소 diff 우선

    5) 테스트 규칙
    - Google Test + CMake 테스트 타깃 추가를 전제로 규칙 작성
    - Given-When-Then 스타일, TEST/TEST_F 사용
    - 우선 테스트 대상: TextAnalyzer(감정/키워드), Filters(감정·키워드·"전체"), CSV 파싱, Feedback 모델
    - 경계값 포함: 빈 입력, 단일 피드백, "중립"만 매칭, "전체" 필터, quality/sentiment 혼동 없이 감정 3분류
    - 커버리지 목표 90% 이상 (gcov/lcov 또는 동등 도구 언급)
    - 리팩토링은 테스트 Green 상태에서만 진행

    6) 리팩토링·AI 작업 방식
    - 한 번에 God Class 분해하지 말고, 테스트 추가 → 작은 리팩토링 → 재실행 순서
    - 요청 범위 밖 파일/기능은 수정하지 않음
    - git commit/push는 사용자가 명시적으로 요청할 때만
    - 응답/주석/커밋 메시지는 한국어 사용 (코드 식별자·HTTP 프로토콜·표준 API명은 영문 유지)

    7) 향후 확장(규칙에 '선택 과제'로만 언급)
    - Trend 시각화 (`test_feedback_trend.csv`)
    - 감정 분석 필터 설정 File DB화

[F] `.cursorrules` 파일에 그대로 붙여넣을 수 있는 plain text.
    - 마크다운 코드블록 없이, 섹션 제목과 bullet 위주의 실행 규칙 형태
    - "~해줘" 같은 대화체가 아니라 Cursor가 따를 명령형/제약형 문장으로 작성
    - 80~150줄 내외, 중복 없이 간결하게
```

---

**사용 팁**

1. Cursor에서 **현재 프로젝트 폴더(`FeedbackAnalyzer_12`)를 워크스페이스로 연 상태**에서 위 프롬프트를 붙여넣으면, AI가 `README.md`, `project_purpose.md`, 소스 구조를 함께 참고합니다.
2. `[C]`의 프로젝트 설명은 필요하면 본인 학습 목표(예: “테스트 먼저”, “main.cpp 분리 우선”)에 맞게 한두 줄 수정하면 됩니다.
3. 생성된 초안을 받은 뒤 `.cursorrules`로 저장하고, 실제 리팩토링 작업 한두 건을 시험해 보며 규칙을 보완하는 흐름이 좋습니다.

원하시면 이 프롬프트를 바로 실행해서 `.cursorrules` 초안까지 생성해 드릴 수도 있습니다.

---

**User**

Spring Boot 프로젝트의전체구조와문제점을분석해줘" →docs폴더 생성후 analysis.md 저장후단계별진행.

위글을 프롬포트 짜줘.

---

**Cursor**

요청하신 **P-C-T-F** 형식의 프롬프트입니다. Spring Boot 프로젝트 구조·문제점 분석 → `docs/analysis.md` 저장 → 단계별 진행까지 한 번에 지시할 수 있습니다.

---

```
[P] 당신은 Spring Boot 레거시 코드베이스를 진단하고 점진적으로 개선하는 시니어 백엔드 엔지니어입니다.
    아키텍처, 의존성, 테스트, 운영 관점에서 문제를 식별하고, 실행 가능한 개선 로드맵을 제시합니다.

[C] 현재 작업 디렉터리의 Spring Boot 프로젝트 전체를 분석 대상으로 합니다.
    Cursor AI는 먼저 코드베이스를 탐색한 뒤, 분석 결과를 문서화하고 이후 작업을 단계별로 진행해야 합니다.
    분석 시 참고할 항목:
    - 빌드/의존성: pom.xml 또는 build.gradle(.kts), Spring Boot 버전, Java/Kotlin 버전
    - 패키지 구조: controller / service / repository / domain / config / dto 등 계층 분리 여부
    - 설정: application.yml(.properties), profile(dev/test/prod), 보안·DB·로깅 설정
    - API: REST 엔드포인트, 요청/응답 DTO, 예외 처리, validation
    - 데이터 계층: JPA/MyBatis/JDBC, 엔티티 설계, N+1, 트랜잭션 경계
    - 테스트: 단위/통합 테스트 존재 여부, 커버리지, @SpringBootTest 남용 여부
    - 운영: Actuator, 로깅, 예외·에러 응답 일관성, 하드코딩/시크릿 노출
    - 코드 스멜: God Class, 순환 의존, 중복 로직, dead code, 매직 넘버/문자열

[T] 아래 순서대로 작업해줘.

    ■ 1단계: 프로젝트 전체 구조 분석
    - 루트 디렉터리 트리와 핵심 모듈/패키지 역할을 파악
    - 기술 스택 요약 (Spring Boot 버전, DB, 빌드 도구, 주요 라이브러리)
    - 요청 흐름 요약 (Controller → Service → Repository → DB)
    - 설정 파일·환경(profile) 구조 정리
    - 테스트/빌드/실행 방법 확인

    ■ 2단계: 문제점 진단
    - 아키텍처 문제 (계층 혼재, 책임 분산 실패, 패키지 구조 불일치)
    - 코드 품질 문제 (중복, 네이밍, 예외 처리, validation 부재)
    - 데이터/성능 문제 (N+1, 트랜잭션, 인덱스/쿼리 이슈 가능성)
    - 테스트/품질 문제 (테스트 부재, flaky test, mock 남용)
    - 보안/운영 문제 (시크릿 하드코딩, CORS/인증 설정, 로깅 부족)
    - 각 문제는 반드시 다음 형식으로 작성:
      · 문제 요약 (1줄)
      · 근거 (파일 경로 + 클래스/메서드 또는 설정 키)
      · 영향 (유지보수/성능/보안/테스트 중 해당 항목)
      · 심각도 (High / Medium / Low)
      · 개선 방향 (1~2줄)

    ■ 3단계: docs/analysis.md 생성
    - 프로젝트 루트에 `docs/` 폴더가 없으면 생성
    - `docs/analysis.md` 파일을 새로 작성 (기존 파일이 있으면 덮어쓰기 전 변경 요약 먼저 제시)
    - 문서 구조:
      1. 프로젝트 개요
      2. 기술 스택
      3. 디렉터리/패키지 구조
      4. 핵심 컴포넌트 및 데이터 흐름
      5. 현재 동작 방식 (빌드·실행·테스트)
      6. 발견된 문제점 (심각도별 표 또는 목록)
      7. 리스크 및 우선순위
      8. 단계별 개선 로드맵 (Phase 1~N)
      9. 즉시 착수 가능한 Quick Wins (3~5개)

    ■ 4단계: 단계별 후속 진행 제안
    - analysis.md 저장 후, 개선 작업을 Phase 1부터 순서대로 제안
    - 각 Phase마다 목표, 수정 대상 파일, 예상 리스크, 검증 방법(테스트/실행) 명시
    - 한 번에 대규모 리팩토링하지 말고, Phase 1만 구체 실행 계획까지 작성
    - Phase 1 착수 전 사용자 확인을 요청

    ■ 공통 제약
    - 분석은 실제 코드/설정 파일을 읽고 근거를 제시 (추측 금지)
    - 코드 수정은 analysis.md 작성 단계에서는 하지 않음 (문서화만)
    - git commit/push는 사용자가 명시적으로 요청할 때만
    - 응답은 한국어, 파일/클래스/설정 키는 원문 경로 그대로 표기

[F] 출력 형식
    1) 먼저 채팅에 분석 요약 (5~10줄)
    2) `docs/analysis.md` 전체 내용을 파일로 저장
    3) 마지막에 Phase 1 실행 계획만 간단히 제시하고 사용자 확인 요청
    - analysis.md는 마크다운 문서로, 표·체크리스트·mermaid 다이어gram(필요 시) 포함
    - "다음에 무엇을 할까요?" 같은 막연한 질문 대신, Phase 1 시작 여부를 명확히 질문
```

---

**짧은 버전** (한 줄 붙여넣기용):

```
Spring Boot 프로젝트 전체 구조와 문제점을 코드/설정 근거로 분석하고, docs/analysis.md(개요·스택·구조·데이터흐름·문제점·심각도·로드맵)를 작성해줘. 분석 단계에서는 코드 수정 없이 문서화만 하고, 저장 후 Phase 1 개선 계획을 제시한 뒤 내 확인을 받고 진행해줘.
```

---

**참고:** 현재 워크스페이스(`FeedbackAnalyzer_12`)는 **C++ + CMake** 프로젝트입니다. 위 프롬프트는 Spring Boot 저장소를 열었을 때 사용하세요. C++ 프로젝트용으로 바꾼 버전이 필요하면 말씀해 주세요.

---

**User**

내가 지금 요청한게 PRD파일과 다른건가?

---

**Cursor**

**네, 다릅니다.** 지금 요청하신 것들은 PRD와 **목적·시점·독자**가 다릅니다.

## 한 줄로 정리

| 구분 | PRD | 지금 요청하신 것 |
|------|-----|------------------|
| **무엇을** | “무엇을 만들/제공할 것인가” (기능·요구사항) | “AI/개발자가 **어떻게** 일할 것인가” (규칙·분석·진행 방식) |
| **시점** | 기획·설계 단계 | 코드가 이미 있을 때 (리팩토링·분석 단계) |
| **독자** | PM, 기획자, 개발팀 전체 | Cursor AI, 실무 개발자 |

---

## 각각이 무엇인지

### 1. PRD (Product Requirements Document)
- **제품/서비스 요구사항** 문서
- 포함 내용: 목적, 사용자, 기능 목록, 입·출력, 우선순위, 완료 기준
- 예: “피드백 CSV 업로드”, “감정 3분류”, “필터링 후 다운로드”

### 2. `.cursorrules` 생성 프롬프트
- **AI 코딩 규칙**을 만드는 지시문
- PRD가 아니라 **개발 행동 규칙** (C++17, 테스트 Green 후 리팩토링, 전역 상태 제거 등)
- “무엇을 만들지”보다 “코드 수정할 때 **지켜야 할 것**”

### 3. `docs/analysis.md` 생성 프롬프트
- **현재 코드베이스 진단서**를 만드는 지시문
- PRD가 아니라 **As-Is 분석** (구조, 문제점, 심각도, 개선 로드맵)
- “원래 요구사항”이 아니라 “**지금 코드가 어떤 상태인지**”

---

## 이 프로젝트에서의 관계

현재 repo에는 PRD라는 이름의 파일은 없고, **`project_purpose.md`가 PRD에 가장 가깝습니다.**

```
project_purpose.md  ≈  PRD / 프로젝트 명세 (학습용 리팩토링 챌린지)
        ↓
docs/analysis.md    =  실제 코드 기준 현황·문제 진단 (As-Is)
        ↓
.cursorrules        =  Cursor AI 작업 규칙 (How to work)
```

- **`project_purpose.md`**: “이 프로젝트는 무엇을 배우고, 어떤 기능·스멜이 있는가” → **기획/교육 명세**
- **`docs/analysis.md`**: “실제 코드를 읽어보니 구조·버그·리스크가 이렇다” → **기술 분석**
- **`.cursorrules` 프롬프트**: “앞으로 AI가 이렇게 코딩해라” → **작업 가이드**

---

## Spring Boot 프롬프트와의 차이

Spring Boot 분석 프롬프트도 PRD가 **아닙니다**.  
다만 그 프로젝트에 PRD가 있다면, `analysis.md`는 **PRD(목표) vs 현재 구현(실제)** 을 비교할 때 참고 자료로 쓸 수 있습니다.

---

**정리:** PRD = “무엇을 만들 것인가”, 요청하신 프롬프트들 = “코드를 어떻게 분석·개선·AI가 다룰 것인가”.  
`project_purpose.md`는 PRD 역할에 가깝고, `.cursorrules`와 `analysis.md`는 그 다음 단계 문서입니다.

PRD 템플릿이나 `project_purpose.md` → `analysis.md` 연결 프롬프트가 필요하면 말씀해 주세요.

---

**User**


아래 프롬포트를 현 프로젝트에 맞게 바꿔줘.


당신은 PRD 전문가입니다. 워크스페이스: c:\DEV\UnitConverter_Cpp
⚠️ 코드·클래스 설계·빌드 스크립트는 작성하지 마십시오.
전제: Phase 4 Epic/Story/Gherkin을 기반으로 작성하라.

# 1. 프로젝트 개요
  1.1 한 줄 목적문 (What/Who/Why)
  1.2 배경 및 문제 정의 (현재 코드: 단일 main, if-else 체인)
  1.3 목표 3~5개 (측정 가능)
  1.4 비목표 2~3개

# 2. 사용자 및 이해관계자
  2.1 타깃 사용자 (페르소나 1개)
  2.2 주요 시나리오 2~3개 (Phase 4 Journey 기반)

# 3. 기능 요구사항
  3.1 기능 목록 (필수/권장/선택 구분)
      - 필수: 입력 검증, meter 허브 변환, 콘솔 출력
      - 권장: OCP 레지스트리, 동적 단위 등록
      - 선택: JSON/CSV/표 출력, 설정 외부화
  3.2 기능별 입출력 계약 (문자열 계약 수준, 코드 X)
  3.3 제약 사항 (입력 형식, 허용 단위, 음수 정책, 미지원 단위 처리)
  → Phase 4 Gherkin의 Background Given과 반드시 일치

# 4. 비기능 요구사항
  4.1 기술 스택: C++17, CMake, Catch2
  4.2 아키텍처 원칙 (OCP/SRP, BCE 레이어, 의존성 방향)
  4.3 테스트 커버리지 목표 (Domain/Boundary/Data 수치)
  4.4 확장성 원칙 (새 단위 추가 시 기존 코드 변경 최소화)

# 5. 데이터 요구사항
  5.1 단위 비율 상수 (meter 허브: feet=3.28084, yard=1.09361)
  5.2 설정 외부화 방식 (JSON/YAML 로드)
  5.3 동적 단위 등록 계약 ("1 cubit = 0.4572 meter" 형식 명시)

# 6. 출력 요구사항
  6.1 콘솔 기본 포맷 ("2.5 meter = 8.202100 feet")
  6.2 JSON 출력 스키마  6.3 CSV 출력 스키마  6.4 표 출력 스키마

# 7. 성공 지표
  7.1 인수 기준 3~5개 (체크박스, 테스트 가능)
  7.2 회귀 보호 규칙 (계약 변경 금지 정책)

# 8. 용어 정의 (Glossary) — 5개 이상

출력: 구조화된 Markdown만. 모호어("적절히") 금지.


---

**Cursor**

현 프로젝트 문서와 구조를 확인한 뒤 PRD 프롬프트를 맞춰 작성하겠습니다.


`UnitConverter_Cpp`용 PRD 프롬프트를 **Feedback Analyzer** 프로젝트 기준으로 맞춘 버전입니다. 이 프로젝트에는 Phase 4 Gherkin 파일이 없으므로, 실제로 있는 `project_purpose.md` · `docs/analysis.md` · `README.md`를 기준 문서로 지정했습니다.

---

```
당신은 PRD 전문가입니다. 워크스페이스: c:\DEV\FeedbackAnalyzer_12

⚠️ 코드·클래스 설계·빌드 스크립트는 작성하지 마십시오.
전제: 아래 문서를 기반으로 작성하라.
  - `project_purpose.md` (기능 명세·학습 단계·시나리오)
  - `docs/analysis.md` (현황 분석·버그·Phase 로드맵·엔드포인트 계약)
  - `README.md` (빌드·실행·CSV 형식)
  - `.cursorrules` (있을 경우, 도메인 불변 규칙 참고)

# 1. 프로젝트 개요
  1.1 한 줄 목적문 (What/Who/Why)
      - What: 자연어 고객 피드백 수집·분류·시각화 C++ 웹 앱
      - Who: 피드백을 분석하는 운영/CS 담당자 + 리팩토링을 학습하는 C++ 개발자
      - Why: 의도적 코드 스멜을 TDD·Clean Code로 점진 개선하며 유지보수 가능한 구조 학습
  1.2 배경 및 문제 정의
      - 현재 코드: `main.cpp` God Function(라우팅+HTML+CSV 파싱+오케스트레이션)
      - 전역 상태(`Session`, `fil_data`, `globalSent/globalKw`), 중복 로직(`containsAny`), 감정 키워드 이중 관리
      - 알려진 버그: "중립" 필터 불일치, CSV `text` 컬럼 미사용, 키워드 필터 `main` 누락
      - 테스트 인프라 없음 (Google Test 미구성)
  1.3 목표 3~5개 (측정 가능)
      - 예: HTTP 5개 엔드포인트 계약 유지, 감정 3분류·키워드 필터 정확도 회귀 0건
      - 예: Domain/Service 계층 단위 테스트 커버리지 90% 이상
      - 예: `main.cpp` 200줄 이하(HTML·핸들러 분리 후)
      - 예: H-1/H-2/H-3(P0 버그) 수정 및 재현 테스트 추가
      - 예: Logger level별(warning/error) UI 표시 요구 충족
  1.4 비목표 2~3개
      - 예: 실제 ML/NLP 기반 감정 분석 도입
      - 예: 다중 사용자 인증·권한 시스템
      - 예: 프로덕션급 RDBMS 도입 (File DB는 선택 과제로만)

# 2. 사용자 및 이해관계자
  2.1 타깃 사용자 (페르소나 1개)
      - 중급 C++ 개발자(리팩토링·TDD 실습자) 또는 CS 담당자(웹 UI로 피드백 분석)
  2.2 주요 시나리오 2~3개
      - `project_purpose.md` §2.2 및 `docs/analysis.md` §4.1 Journey 기반:
        ① 대시보드 접속 → 텍스트 입력 → 감정/키워드 집계 확인
        ② CSV 업로드(`text` 컬럼) → 필터(감정·키워드) → 결과 CSV 다운로드
        ③ 리팩토링 학습자: 테스트 추가 → P0 버그 수정 → Green 유지하며 구조 분리

# 3. 기능 요구사항
  3.1 기능 목록 (필수/권장/선택 구분)
      - 필수:
        · GET `/` 대시보드 HTML (UTF-8 한국어 UI)
        · POST `/analyze` 텍스트 피드백 입력 + 감정(긍정/중립/부정)·키워드 집계
        · POST `/upload` CSV 업로드 (헤더 `text` 컬럼 파싱)
        · POST `/filter` 감정·키워드 필터 (`전체|긍정|중립|부정`, `전체|카테고리`)
        · GET `/download` 필터 결과 CSV (UTF-8 BOM, `text\n` 헤더)
        · 감정/키워드 분류 규칙 단일화 (TextAnalyzer·Filters 동일 기준)
      - 권장:
        · Google Test + CMake 테스트 타깃
        · HTML 렌더링·라우트 핸들러·CSV 파서 분리 (Extract Function/Class)
        · 전역 상태 → `AppState`/Repository 캡슐화
        · Logger level별 UI 표시 (warning/error/success)
        · textarea 멀티라인 입력 E2E 검증
      - 선택:
        · Trend 시각화 (`test_feedback_trend.csv`)
        · 감정 필터 키워드 File DB 관리
        · localhost 바인딩·업로드 크기/MIME 검증
  3.2 기능별 입출력 계약 (문자열 계약 수준, 코드 X)
      - 각 HTTP 엔드포인트별:
        · Request: method, path, form field / multipart field 이름
        · Response: status, Content-Type, 본문 형식(HTML/CSV), 한국어 라벨 값
      - `project_purpose.md` §2.3 표와 `docs/analysis.md` 부록 체크리스트와 일치시킬 것
  3.3 제약 사항
      - 감정 라벨: `u8"긍정"`, `u8"중립"`, `u8"부정"` (UTF-8 고정)
      - 필터 sentinel: `u8"전체"` (미선택 = 전체)
      - CSV 입력: 첫 행 헤더, `text` 컬럼 필수
      - CSV 출력: `\xEF\xBB\xBF` BOM + `text\n` + 본문
      - 서버: `http://localhost:8080` (바인딩 `0.0.0.0:8080`)
      - 빈 피드백·필터 결과 없음·업로드 실패 시 warning/error UI 메시지 계약
      - `httplib.h` 외부 라이브러리 — 수정 금지
      → `docs/analysis.md` §6(H-1~H-3) 및 부록 엔드포인트 체크리스트와 반드시 일치

# 4. 비기능 요구사항
  4.1 기술 스택: C++17, CMake 3.14+, cpp-httplib, Google Test (+ gcov/lcov 권장)
  4.2 아키텍처 원칙
      - SRP: Controller(라우트) / Service(TextAnalyzer·Filters) / View(HtmlRenderer) / State(Session·AppState) 분리
      - 의존성 방향: main → handlers → services → domain(Feedback) → Constants
      - 리팩토링은 테스트 Green 상태에서만 진행; 최소 diff 우선
  4.3 테스트 커버리지 목표
      - Domain/Service(TextAnalyzer, Filters, CsvParser): **90% 이상**
      - HTTP 핸들러: 통합/스모크 테스트 또는 부록 체크리스트 수동 검증
      - Given-When-Then, TEST/TEST_F 스타일
      - 경계값: 빈 입력, "중립"만 해당, "전체" 필터, `text` 컬럼 없는 CSV, main 키워드만 포함 피드백
  4.4 확장성 원칙
      - 키워드 카테고리 추가 시 `Constants` 단일 소스만 변경 (Shotgun Surgery 방지)
      - 감정 분석 알고리즘 교체 시 Strategy 패턴으로 Service 계층만 교체 가능

# 5. 데이터 요구사항
  5.1 도메인 데이터
      - `Feedback`: 텍스트 본문 (분석 결과 필드는 권장 과제로 명시)
      - `Constants::SENTIMENT_KEYWORDS`: 긍정/중립/부정 키워드 맵 (단일 소스)
      - `Constants::CATEGORY_KEYWORDS`: 카테고리별 `main`/`sub` 키워드 맵
  5.2 세션·상태
      - 현재: `Session::currentFeedbacks` (인메모리)
      - 필터 결과: `/filter` 성공 시 다운로드 대상 — 대상 정의를 PRD에서 명확히 (현재 `fil_data` vs 전체 목록 이슈)
  5.3 CSV 계약
      - 입력: `text` 컬럼 (README 명세)
      - 출력: `text` 단일 컬럼, UTF-8 BOM
  5.4 (선택) File DB 계약
      - 감정 필터 키워드 CRUD 형식, 저장 위치, 앱 기동 시 로드 규칙 (Phase 5 선택 과제)

# 6. 출력 요구사항
  6.1 HTML 대시보드 (기본)
      - 섹션: 피드백 입력(textarea), CSV 업로드, 감정/키워드 필터, 분석 결과(집계 stat), 다운로드 버튼
      - 알림: success / warning / error alert (한국어, 타임스탬프 success 포함)
      - Content-Type: `text/html; charset=UTF-8`
  6.2 CSV 다운로드 스키마
      - BOM + 헤더 `text` + 각 행 피드백 텍스트 1줄
      - Content-Disposition: `attachment; filename="filtered_feedback.csv"`
  6.3 (선택) Trend 시각화 출력
      - `test_feedback_trend.csv` 기반 시계열/추이 표시 형식 (차트 타입·축 정의)
  6.4 로그 UI 출력 (권장)
      - level별 표시 정책: warning/error를 페이지 alert에 반영 가능해야 함

# 7. 성공 지표
  7.1 인수 기준 3~5개 (체크박스, 테스트 가능)
      - [ ] POST `/filter` sentiment=`중립` 시 TextAnalyzer 집계와 동일 집합 반환
      - [ ] CSV 업로드 시 `text` 컬럼 값만 Feedback으로 적재
      - [ ] 키워드=`배송`(main 키워드) 필터 시 해당 피드백 포함
      - [ ] `ctest` 전체 Green + Service 계층 커버리지 ≥ 90%
      - [ ] 부록 엔드포인트 체크리스트 5항목 통과
  7.2 회귀 보호 규칙
      - HTTP 5개 엔드포인트 path/method 변경 금지 (명시적 PRD 개정 없이)
      - 감정 3분류 한국어 라벨 변경 금지
      - CSV 출력 BOM·헤더 형식 변경 금지
      - 리팩토링 전 failing test 0건 유지

# 8. 용어 정의 (Glossary) — 5개 이상
  - Feedback, Sentiment(감정), Category Keyword(키워드 카테고리)
  - God Function, Session, fil_data, UTF-8 BOM, Given-When-Then 등 프로젝트 맥락 용어

# 9. (선택) 학습 Phase 매핑
  - `project_purpose.md` §6.1 8단계와 `docs/analysis.md` §8 Phase 1~5를 PRD 로드맵 표로 매핑
  - 각 Phase: 목표, 산출물, 인수 기준 1줄

출력: `docs/PRD.md`에 저장할 구조화된 Markdown만. 모호어("적절히", "가능하면") 금지.
      모든 기능·제약은 위 기준 문서의 파일 경로·섹션 번호를 근거로 명시할 것.
```

---

**원본과의 주요 변경점**

| 원본 (UnitConverter) | 현 프로젝트 (Feedback Analyzer) |
|----------------------|----------------------------------|
| Phase 4 Epic/Story/Gherkin | `project_purpose.md` + `docs/analysis.md` |
| Catch2 | Google Test |
| 콘솔 단위 변환 | HTTP 웹 + HTML/CSV |
| meter 허브·단위 비율 | 감정 키워드·카테고리 키워드 |
| JSON/CSV/표 출력 | HTML 대시보드 + CSV 다운로드 |

Gherkin(`.feature`) 파일을 나중에 추가할 계획이면, 전제 문장에 `docs/features/*.feature`(Phase 4)를 한 줄 더 넣으면 됩니다.

---

**User**

아래 프롬포트를 현 프로젝트에 맞게 변경해줘.

워크스페이스: c:\DEV\UnitConverter_Cpp
저장소: https://github.com/kznetwork/UnitConverter_Cpp
전제: Phase 5 PRD를 기반으로 작성하라. ⚠️ 코드 구현 추가 금지.

# UnitConverter (C++)
한 줄 설명 — PRD 1.1 목적문 활용

## 목차 (자동 링크 포함)

## 개요 (Overview)
- 이 프로젝트가 해결하는 문제 (PRD 1.2 배경 기반)
- 주요 학습 목표 (OCP/SRP, BCE, TDD)
- 현재 코드의 문제점과 개선 방향

## 빠른 시작 (Quick Start)
- 사전 조건 (C++ 버전, 빌드 도구)
- 빌드 & 실행:  g++ -o UnitConverter UnitConverter.cpp ./UnitConverter
- 예시 입출력: meter:2.5 → 결과

## 지원 단위 및 비율
표: 단위명 / 식별자 / meter 기준 비율 / 출처
(1 meter = 3.28084 feet, 1 meter = 1.09361 yard)

## 입력 형식 계약
- 정상: 단위:값 예시 3개
- 비정상: 음수·잘못된형식·없는단위 케이스 3개 + 에러 메시지

## 아키텍처
- BCE 레이어 다이어그램 (Mermaid 또는 ASCII)
- 의존성 방향 설명
- 새 단위 추가 방법 (단계별, 코드 최소화)

## 테스트 실행
cmake -S . -B build && cmake --build build ctest --test-dir build -V

## 설정 파일 (JSON/YAML)
- 위치 및 형식 예시
- 동적 단위 등록: "1 cubit = 0.4572 meter"

## 출력 포맷
콘솔 / JSON / CSV 각 예시 블록 포함

## 생성형 AI 활용 Activities (6시간)
README의 Activities 섹션 내용을 반영하라.

## 기여 가이드 / ## 라이선스 (MIT)

Markdown만. 표·코드 블록·Mermaid 다이어그램 적극 활용.


---

**Cursor**

현 프로젝트 문서를 확인한 뒤 README 생성 프롬프트를 맞춰 작성합니다.


현 프로젝트(`FeedbackAnalyzer_12`) 기준으로 맞춘 README 생성 프롬프트입니다.

---

```
워크스페이스: c:\DEV\FeedbackAnalyzer_12
저장소: https://github.com/jvvoung/FeedbackAnalyzer_12.git
전제: `docs/PRD.md`를 기반으로 작성하라. 보조 참고: `project_purpose.md`, `docs/analysis.md`, `.cursorrules`
⚠️ 코드 구현 추가 금지. README.md 문서만 작성·갱신.

# Feedback Analyzer (C++)
한 줄 설명 — PRD §1.1 목적문(What/Who/Why)을 1문장으로 요약

## 목차 (자동 링크 포함)

## 개요 (Overview)
- 이 프로젝트가 해결하는 문제 (PRD §1.2 배경: God Function, 전역 상태, P0 버그, 테스트 부재)
- 주요 학습 목표 (SRP, 관심사 분리, TDD, 점진적 리팩토링) — PRD §1.3·§4.2·`project_purpose.md` §1.3 반영
- 현재 코드의 문제점과 개선 방향 (`docs/analysis.md` §6·§8 Phase 1~5 요약)

## 빠른 시작 (Quick Start)
- 사전 조건: C++17, CMake 3.14+, MinGW/MSVC/Clang, 웹 브라우저
- 저장소 클론:
  git clone https://github.com/jvvoung/FeedbackAnalyzer_12.git
  cd FeedbackAnalyzer_12
- 빌드 (Windows MinGW 예시):
  rmdir /q /s build
  cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=C:/mingw64/bin/g++.exe
  cmake --build build
- 실행:
  build\feedback_analyzer.exe
- 접속: http://localhost:8080
- 예시 사용 흐름 1개 (텍스트 입력 → 분석 stat 확인 → 필터 → CSV 다운로드)

## HTTP API 개요
표: Method / Path / 역할 / Content-Type
(PRD §3.2 F-01~F-05, `docs/analysis.md` §4.1 기준)
- GET `/`
- POST `/analyze`
- POST `/upload`
- POST `/filter`
- GET `/download`

## 감정 분류 및 키워드 카테고리
표: 유형 / 라벨(UTF-8) / 판정 규칙 / Constants 출처
- 감정: 긍정 / 중립 / 부정 (키워드 매칭 기준, PRD §3.3·§5.1)
- 키워드 카테고리: 배송, 품질, 가격, 서비스, 사용성 (`main`/`sub` 키워드)
- 필터 sentinel: `전체` = 해당 축 미적용

## 입력·출력 계약

### CSV 입력 (POST `/upload`)
- 정상 예시 1개: `text` 헤더 + 데이터 행
- 비정상 케이스 3개 + UI alert 메시지:
  · `text` 컬럼 없음
  · 빈 파일
  · 헤더만 있고 데이터 없음
(PRD §3.2 F-03, README CSV 형식)

### HTTP Form 입력 (POST `/analyze`, POST `/filter`)
- 정상 예시 3개 (PRD §3.2·§2.2 시나리오 A/B 기반)
- 비정상 케이스 3개 + alert:
  · 빈 text 제출
  · 피드백 없이 `/filter`
  · 필터 결과 0건

### CSV 출력 (GET `/download`)
- UTF-8 BOM + `text\n` 헤더 + 본문 형식 예시 블록 (PRD §6.2)

## 아키텍처
- 계층 다이어그램 (Mermaid 또는 ASCII):
  Browser → Route Handlers(main) → TextAnalyzer / Filters → Constants
           → Session(AppState) → HtmlRenderer
- 현재 As-Is vs To-Be(Phase 2~3) 의존성 방향 설명 (PRD §4.2)
- 새 키워드 카테고리 추가 방법 (단계별, `Constants::CATEGORY_KEYWORDS` 단일 소스 원칙)
- `httplib.h`는 외부 라이브러리 — 수정 금지 명시

## 프로젝트 구조
`docs/analysis.md` §3 디렉터리 트리 반영 (docs/, tests/ 예정 포함)

## 테스트 실행
- (권장/목표) PRD §4.3·§7.1:
  cmake -S . -B build && cmake --build build
  ctest --test-dir build --output-on-failure
- 현재 상태: 테스트 미구성 시 "Phase 1 목표"로 명시 (`docs/analysis.md` §5.3)
- 커버리지 목표: TextAnalyzer·Filters·CsvParser ≥ 90%

## 설정 및 데이터
- 현재: `Constants` 하드코딩 (application.yml 없음) — PRD §5.1
- 세션: 인메모리 `Session::currentFeedbacks` — PRD §5.2
- (선택 Phase 5) File DB: 감정 키워드 CRUD 형식·저장 위치 — PRD §5.4 한 줄 요약

## 출력 포맷

### HTML 대시보드 (기본)
- 섹션 구성, alert(success/warning/error) 예시 — PRD §6.1

### CSV 다운로드
- BOM·헤더·파일명 `filtered_feedback.csv` 예시 블록 — PRD §6.2

### (선택) Trend 시각화
- `test_feedback_trend.csv` 기반 — PRD §6.3, Phase 5

## 알려진 이슈 (Known Issues)
P0 버그 3건 표로 요약 (H-1 중립 필터, H-2 CSV text 컬럼, H-3 main 키워드) — `docs/analysis.md` §6.1
→ Phase 1에서 수정 예정임을 명시

## 생성형 AI 활용 Activities
`project_purpose.md` §6.1 8단계 + PRD §9 Phase 매핑을 README Activities 섹션으로 재구성:
- 각 단계: 소요 시간, 목표, Cursor/AI 활용 팁 1줄, 산출물
- 총 학습 시간 표 (L-0~L-7, Phase 1~5 대응)
- 예: Phase 1 — GTest 추가·P0 버그 failing test → Green
- 예: Phase 2 — HtmlRenderer/RouteHandlers 분리 (AI에게 Extract Function 요청)
- 예: Phase 5 — Trend·File DB (선택)

## 기여 가이드
- 브랜치·PR·커밋 메시지(한국어)·리팩토링 규칙(테스트 Green 후 진행) — `.cursorrules` 요약
- HTTP 5개 엔드포인트·감정 3분류 라벨 변경 시 PRD 개정 필요 (PRD §7.2)

## 라이선스
- 저장소에 LICENSE 파일이 없으면 "LICENSE 파일 추가 예정" 또는 MIT(저장소 확인 후 기재) 명시

---

출력 규칙:
- Markdown만. 표·코드 블록·Mermaid 다이어그램 적극 활용.
- 모호어("적절히") 금지. 수치·라벨·경로는 PRD/analysis 근거와 일치.
- 저장 위치: 프로젝트 루트 `README.md` (기존 README 구조·스크린샷 `feedback_analyzer.png` 참조 유지)
- UnitConverter 전용 내용(단위 변환, meter 허브, Catch2, g++ 단일 파일 빌드, JSON/YAML 단위 등록)은 포함하지 말 것.
```

---

**원본 대비 주요 치환**

| UnitConverter | Feedback Analyzer |
|---------------|-------------------|
| Phase 5 PRD | `docs/PRD.md` |
| g++ 단일 파일 빌드 | CMake + `feedback_analyzer.exe` |
| 지원 단위·meter 비율 | 감정 3분류 + 키워드 카테고리 |
| `단위:값` 입력 계약 | HTTP API + CSV `text` 컬럼 |
| BCE 레이어 | Route → Service → Domain → View |
| Catch2 / ctest | Google Test / ctest (Phase 1 목표) |
| JSON/YAML 설정 | Constants 하드코딩 + (선택) File DB |
| 콘솔/JSON/CSV 출력 | HTML 대시보드 + CSV 다운로드 |

저장소 URL이 바뀌면 프롬프트 상단 `저장소:` 한 줄만 수정하면 됩니다.

---

**User**

완성된 README.md와 Phase 5 PRD를 비교하라. 보고만 (수정 금지):
1) PRD에 있으나 README에 빠진 항목
2) README에 있으나 PRD에 없는 항목 (범위 초과 위험)
3) 입출력 계약 불일치 (특히 비율 3.28084 / 1.09361)
4) 테스트 커버리지 목표 불일치
bullet만.
이것도 프로젝트에 맞게 수정해줘

---

**Cursor**

README.md와 PRD를 읽고 항목별로 대조하겠습니다.


**비교 프롬프트 (Feedback Analyzer용)**

```
완성된 README.md와 docs/PRD.md를 비교하라. 보고만 (수정 금지):

1) PRD에 있으나 README에 빠진 항목
2) README에 있으나 PRD에 없는 항목 (범위 초과 위험)
3) 입출력 계약 불일치 (HTTP 엔드포인트, CSV text/BOM, 감정 3분류, 키워드 main/sub 매칭, 필터 sentinel `전체`, alert 메시지, GET `/download` 대상)
4) 테스트 커버리지 목표 불일치 (Service ≥90% vs HTTP 핸들러 스모크/체크리스트)

bullet만.
```

---

**실제 비교 결과**

### 1) PRD에 있으나 README에 빠진 항목

- §1.4 **비목표** (NG-1 ML/NLP, NG-2 인증, NG-3 RDBMS) — README에 비목표 섹션 없음
- §2.1 **페르소나** (김리팩) — README에 페르소나 정의 없음
- §2.2 **시나리오 C** (테스트 → 버그 수정 → 구조 분리) — README는 사용 흐름 1개만, 시나리오 A/B/C 미구분
- §3.1 **기능 ID·우선순위** (F-01~F-14, 필수/권장/선택) — README HTTP 표만 있고 F-ID·F-14(업로드 MIME/크기 검증) 없음
- §3.2 **F-01 GET `/` 계약** — success alert `피드백 분석기 시작`, 세션 초기화 side effect 미기재
- §3.2 **F-03 대안 alert** — warning `CSV에 text 컬럼이 없습니다.` (error 우선) README에 warning 옵션 없음
- §3.3 **제약 C-06·C-08·C-09** — HTTP path 변경 금지, alert 계약, Green 상태 리팩토링 규칙 README에 명시적 나열 없음
- §4.1 **Windows 빌드 설정** — `_WIN32_WINNT=0x0A00`, `ws2_32` 링크 README Quick Start에 없음
- §4.2 **계층 책임 표** (Controller/Service/View/State/Domain/Config) — Mermaid만 있고 표 없음
- §4.3 **HTTP 핸들러 검증 방식** — 통합/스모크 또는 부록 체크리스트 5항목 (90% 아님) README 테스트 섹션에 없음
- §4.3 **경계값 테스트 ID** T-01~T-7 — README는 케이스 나열만, ID 없음
- §4.4 **알고리즘 교체 Strategy** 원칙 — README에 없음
- §5.1 **Feedback 권장 필드** (`sentiment`, `category`, Phase 3) — README 데이터 섹션에 없음
- §5.2 **제거 대상 상태** (`globalSent/Kw`, `internalData`, `filterOptions`) — 아키텍처에 일부만, 데이터 요구사항으로 미정리
- §5.3 **CSV 입력** — 추가 컬럼 허용·무시 규칙 README에 없음
- §6.4 **Logger level 정책 표** (INFO/WARNING/ERROR/DEBUG → HTML 표시 여부) — README alert 예시만, level별 정책 표 없음
- §7.1 **인수 기준 AC-1~AC-7** 체크리스트 — README에 AC 목록 없음
- §7.2 **회귀 규칙 R-4·R-5** — R-4(`ctest` Green merge 금지) README 기여 가이드에 없음 (R-5 httplib는 있음)
- §8 **Glossary** — README에 용어 정의 섹션 없음
- **부록 A** 엔드포인트 회귀 체크리스트 5항목 — README에 체크리스트 본문 없음 (analysis.md 링크만)

### 2) README에 있으나 PRD에 없는 항목 (범위 초과 위험)

- **`feedback_analyzer.png` 스크린샷** — PRD 출력 요구사항에 없음
- **`tests/` 디렉터리·파일 3개**를 프로젝트 구조에 포함 — PRD는 Phase 1 목표로만 언급; 실제 미구성 시 “이미 존재”로 오해 가능
- **알려진 이슈 H-4**를 P0 표에 포함 — PRD §1.2는 배경으로만, Known Issues 섹션·Medium 이슈 링크는 PRD 본문에 없음
- **As-Is Mermaid** (`S_KEYWORDS 중복` 등) — PRD 아키텍처 원칙(To-Be)만 정의, As-Is 다이어그램 없음
- **Cursor/AI 활용 팁** (L-0~L-7 단계별) — PRD §9 Phase 매핑에 AI 팁 없음 (문서화 범위 초과는 아니나 PRD 외 콘텐츠)
- **총 학습 시간 14시간** — PRD §9 단계 합계는 13시간(1+2+1.5+1+1.5+1+3+2); README 14h와 불일치
- **LICENSE 추가 예정** — PRD에 라이선스 조항 없음
- **키워드 카테고리 매칭에 `sub` 포함** (README §146~150) — PRD F-06은 **`main`만**; README가 PRD보다 넓은 계약을 서술 (범위 초과·불일치)

### 3) 입출력 계약 불일치

- **키워드 매칭 범위:** PRD §3.2 F-06·H-3 → `CATEGORY_KEYWORDS[cat]["main"]`만; README → `main` **및 `sub`** — **불일치**
- **GET `/`:** PRD F-01 success `피드백 분석기 시작` + 타임스탬프; README — GET `/` 입출력 계약 미기재
- **POST `/analyze` HTTP status:** PRD F-02 예외 시에도 **200** + error alert; README — status code 명시 없음
- **CSV `text` 없음:** PRD F-03 error **또는** warning `CSV에 text 컬럼이 없습니다.`; README — error만 기재 (error 우선이면 수용 가능하나 warning 옵션 누락)
- **GET `/download` 0건 warning:** PRD F-05 메시지 `다운로드할 필터 결과가 없습니다.`; README — “Phase 4 warning 추가 예정”만, **정확한 문구 없음**
- **CSV 입력 추가 컬럼:** PRD §5.3 추가 컬럼 허용·무시; README — `text`만 예시, 추가 컬럼 규칙 없음
- **다운로드 대상:** PRD F-05 “마지막 `/filter` 성공(≥1건) 직후 집합” — README §226과 **일치** (양호)
- **감정 3분류·sentinel `전체`·CSV BOM/`text\n`:** README와 PRD **일치**
- *(UnitConverter 항목 대체)* **meter 비율 3.28084/1.09361** — 본 프로젝트 해당 없음

### 4) 테스트 커버리지 목표 불일치

- **Service 4종 ≥90%:** PRD §4.3·G-3 TextAnalyzer·Filters·CsvParser; §4.3에 Feedback ≥90% — README 동일 4종 ≥90% → **수치 일치**
- **HTTP 핸들러:** PRD §4.3 “통합/스모크 **또는** 부록 체크리스트 수동 검증 (90% 아님)” — README 커버리지 표에 **HTTP 핸들러 행 자체 없음** → 검증 방식 누락 (90%로 오해할 여지는 없으나 PRD 요구 미반영)
- **G-3 vs §4.3:** PRD G-3는 Service 3종만 명시, §4.3·README는 Feedback 포함 — G-3와 README/§4.3 간 **PRD 내부** 경미한 차이 (README는 §4.3과 정렬)
- **AC-4:** PRD `ctest` Green + 3종 ≥90%; README Phase 1 목표와 동일 방향 — **불일치 없음** (AC 번호·체크리스트만 README 누락)

---

**User**

이 프로젝트를 기준으로 아래 프롬포트를 변경해줘.

@c:\DEV\UnitConverter_Cpp/README.md 와 PRD 문서를 참고하여
테스트 플랜 작성을 위한 샘플 예제 1개를 선택해줘.

선택 기준:
1. README의 기본 요구사항에 명시된 핵심 변환 기능
2. 비즈니스 로직(1 meter = 3.28084 feet, 1 meter = 1.09361 yard)을 직접 검증
3. 정상 경로(happy path)와 경계값 모두 포함 가능한 기능

선택한 예제에 대해 다음을 명시하라:
- 선택한 기능 이름 (예: meter → feet 변환)
- README 요구사항 번호 (예: 기본 요구사항 4번)
- 입력 예시: "meter:2.5"
- 기대 출력: "2.5 meter = 8.202100 feet"
- 이 예제를 선택한 이유 2줄

코드 작성 금지. 선택 결과 문서만 출력.


---

**Cursor**

README와 PRD의 핵심 요구사항을 확인한 뒤, 테스트 플랜용 프롬프트를 이 프로젝트 기준으로 맞추겠습니다.


**Feedback Analyzer용 프롬프트**

```
@c:\DEV\FeedbackAnalyzer_12\README.md 와 @c:\DEV\FeedbackAnalyzer_12\docs\PRD.md 를 참고하여
테스트 플랜 작성을 위한 샘플 예제 1개를 선택해줘.

선택 기준:
1. README·PRD 필수 기능(F-01~F-06) 중 핵심 비즈니스 로직을 직접 검증하는 기능
   (감정 3분류, 키워드 카테고리 매칭, 필터 sentinel `전체` 중 하나)
2. PRD §3.2 F-06 / §7.1 AC-1~AC-3에 정의된 분류·필터 규칙을 직접 검증
   (예: 긍정→부정→중립 판정 순서, TextAnalyzer·Filters 동일 규칙, `main` 키워드 매칭)
3. 정상 경로(happy path)와 경계값(T-01~T-07)을 모두 포함 가능한 기능

선택한 예제에 대해 다음을 명시하라:
- 선택한 기능 이름 (예: POST `/filter` — sentiment=`중립` 필터)
- PRD 요구사항 ID (예: F-04, F-06, AC-1)
- README·PRD 근거 (섹션 번호, 예: README §HTTP Form 입력, PRD §3.2 F-04)
- 입력 예시:
    · Given: Feedback 1건 `text="그냥 그래요. 특별한 감정 없음."` (긍정/부정 키워드 없음)
    · When: POST `/filter`, `sentiment=중립&keyword=전체`
- 기대 출력:
    · Then: 필터 결과 집합 = TextAnalyzer 감정 분류 `중립` 집합과 100% 일치
    · HTML stat: 중립 ≥1, success/warning alert는 PRD §3.2 F-04 계약 준수
- (경계값 1건 추가) Given: 빈 Session → When: POST `/filter` → Then: warning `분석할 피드백이 없습니다.`
- 이 예제를 선택한 이유 2줄

코드 작성 금지. 선택 결과 문서만 출력.
```

---

**실행 시 출력 예시** (프롬프트 적용 결과 샘플)

### 테스트 플랜 샘플 예제 선택

**선택한 기능 이름**  
POST `/filter` — sentiment=`중립` 필터 (TextAnalyzer·Filters 분류 규칙 일치 검증)

**PRD 요구사항 ID**  
F-04 (필터), F-06 (분류 규칙 단일화), AC-1 (중립 필터 100% 일치)

**README·PRD 근거**  
- README §HTTP Form 입력 — POST `/filter` 정상·비정상 예시  
- PRD §3.2 F-04, F-06 — 중립 필터 계약, H-1 수정 후 TextAnalyzer와 동일 집합  
- PRD §4.3 T-03 — 긍정/부정 키워드 없는 텍스트 + sentiment=중립  
- PRD §7.1 AC-1 — 인수 기준

**입력 예시 (happy path)**  
- **Given:** Session에 Feedback 1건 — `text="그냥 그래요. 특별한 감정 없음."`  
- **When:** `POST /filter`, body `sentiment=중립&keyword=전체`

**기대 출력**  
- **Then:** 반환 subset = TextAnalyzer가 동일 입력에 `중립`으로 분류한 Feedback 집합과 **100% 일치** (AC-1)  
- HTML: 필터 결과 stat에 중립 건수 ≥1, 다운로드 버튼 표시 (≥1건 시 F-04)  
- alert: error 없음; 0건이면 warning `필터링 결과가 없습니다.` (F-04)

**경계값 (1건)**  
- **Given:** Session 비어 있음  
- **When:** `POST /filter`, `sentiment=중립&keyword=전체`  
- **Then:** warning `분석할 피드백이 없습니다.` (README §HTTP Form 비정상, PRD §3.2 F-04)

**선택 이유**  
- README·PRD 필수 기능 중 **H-1(P0 버그)** 을 직접 재현·검증할 수 있어 Phase 1 테스트 플랜의 대표 사례로 적합하다.  
- happy path(중립 1건 필터)와 경계값(빈 Session), sentinel `전체`를 한 기능으로 묶어 Given-When-Then 테스트 설계가 가능하다.

---

**UnitConverter → Feedback Analyzer 치환 요약**

| 원본 | 변경 |
|------|------|
| meter → feet 변환 | 감정·키워드 분석 / `중립` 필터 |
| 3.28084 / 1.09361 | 긍정→부정→중립 판정, TextAnalyzer·Filters 동일 규칙 |
| `meter:2.5` | `sentiment=중립&keyword=전체` + Feedback 텍스트 |
| 기본 요구사항 N번 | PRD F-xx / AC-x ID |

---

**User**

아래 프롬포트를 현 프로젝트에 맞게 변경해줘.

[P] 시니어 QA 리드입니다.
[C] C++17, CMake, Catch2
[T] 선택한 샘플 예제(meter→feet 변환)를 기반으로 테스트 계획서를 작성해줘.

    포함 항목:
    - Catch2 기반 단위 테스트 범위/우선순위
    - 경계값 케이스 목록:
        · value = 0 (영값 변환)
        · value = 매우 큰 수 (오버플로 위험)
        · value < 0 (음수 입력 정책)
        · 소수점 파싱 실패 ("meter:abc")
        · ":"없는 입력 (형식 오류)
        · 없는 단위 ("parsec:1.0")
    - 예외/특이 케이스 목록
    - 커버리지 목표 (Domain 95%+ / Boundary 85%+)
    - # gcov / lcov / gcov UnitConverter.cpp 측정 전략

[F] Markdown 문서. test_plan.md로 저장.


---

**Cursor**

**Feedback Analyzer용 프롬프트 (P-C-T-F)**

```
[P] 시니어 QA 리드입니다. C++ 레거시 웹 앱 리팩토링 챌린지의 테스트 전략을 수립합니다.

[C] 워크스페이스: c:\DEV\FeedbackAnalyzer_12
    기술 스택: C++17, CMake 3.14+, Google Test, cpp-httplib
    근거 문서: README.md, docs/PRD.md, docs/analysis.md, .cursorrules
    샘플 예제(전제): POST `/filter` — sentiment=`중립` 필터
      · PRD: F-04, F-06, AC-1
      · Given: Feedback `text="그냥 그래요. 특별한 감정 없음."` (긍정/부정 키워드 없음)
      · When: `sentiment=중립&keyword=전체`
      · Then: Filters 결과 집합 = TextAnalyzer `중립` 분류 집합 100% 일치

[T] 위 샘플 예제를 중심으로 테스트 계획서를 작성해줘.

    포함 항목:
    - Google Test 기반 단위 테스트 범위·우선순위
      · 1순위: TextAnalyzer, Filters, CsvParser (Phase 1)
      · 2순위: Feedback, KeywordUtils/containsAny 통합
      · 3순위: HTTP 핸들러 스모크 또는 PRD 부록 A 체크리스트(수동/E2E)
    - 경계값 케이스 목록 (PRD §4.3 T-01~T-07 및 README §입력·출력 계약 반영):
        · T-01: 빈 Feedback 목록 → analyzeSentiment → 긍정=0, 중립=0, 부정=0
        · T-02: 단일 피드백 1건 → analyze + filter → 집계·필터 건수 1
        · T-03: 긍정/부정 키워드 없는 텍스트 → filter sentiment=중립 → TextAnalyzer 중립 집합과 동일 (AC-1, H-1)
        · T-04: sentiment=전체, keyword=전체 → 입력 전체 반환
        · T-05: CSV 헤더에 `text` 없음 → parse 오류 또는 0건 적재 (H-2, AC-2)
        · T-06: `배송` main 키워드("배송","택배" 등)만 포함 → filter keyword=배송 포함 (H-3, AC-3)
        · T-07: 품질 카테고리 키워드만, 감정 키워드 없음 → analyzeSentiment=중립 (카테고리·감정 분리)
        · 추가: 빈 text 제출(POST `/analyze`) → 피드백 미추가
        · 추가: Session 비어 있을 때 POST `/filter` → warning `분석할 피드백이 없습니다.`
        · 추가: 필터 결과 0건 → warning `필터링 결과가 없습니다.`
        · 추가: textarea 개행 포함 입력 → 저장·분석·CSV 개행 유지 (AC-7)
    - 예외/특이 케이스 목록:
        · TextAnalyzer vs Filters 감정 키워드 이중 관리(S_KEYWORDS) 불일치 (H-1)
        · CSV `fields[0]`만 사용하여 `text` 컬럼 무시 (H-2)
        · Filters에서 `main` 키워드 skip (H-3)
        · GET `/download` 필터 미실행 시 빈 CSV (M-6)
        · POST `/analyze` 예외 시 HTML error alert + 200 (F-02)
        · CSV `text` 없음: error `파일 업로드 중 오류` 또는 warning `CSV에 text 컬럼이 없습니다.` (error 우선)
    - 커버리지 목표 (PRD §4.3·§7.1 AC-4):
        · TextAnalyzer / Filters / CsvParser / Feedback: 각 ≥ 90%
        · HTTP 핸들러: 90% 목표 아님 — 부록 A 5항목 스모크/체크리스트
        · (팀 내부 stretch goal 명시 시) Domain(Service) 95%+ / Boundary(경계·에러) 85%+
    - gcov / lcov 측정 전략:
        · CMake `--coverage` 또는 `-fprofile-arcs -ftest-coverage` 플래그
        · 측정 대상: TextAnalyzer.cpp, Filters.cpp, CsvParser.cpp (및 추출 후 해당 헤더)
        · 제외: main.cpp(HTTP), httplib.h, HtmlRenderer(Phase 2 이후 분리)
        · 실행: `ctest` → `lcov --capture` → HTML 리포트; Phase 1 Green 후 baseline 수립
    - Phase별 테스트 일정 (PRD §9 / README Activities):
        · Phase 1: failing test → H-1/H-2/H-3 수정 → Green
        · Phase 2~4: 리팩토링 후 회귀(ctest + 부록 A)
        · Phase 5: Trend·File DB — 선택, 별도 AC 정의

[F] Markdown 문서. `docs/test_plan.md`로 저장.
    코드 작성 금지. Given-When-Then 표·체크리스트·우선순위 표 포함.
    UnitConverter 전용 내용(Catch2, meter/feet, parsec, meter:abc)은 포함하지 말 것.
```

---

**원본 대비 치환표**

| UnitConverter | Feedback Analyzer |
|---------------|-------------------|
| Catch2 | Google Test |
| meter→feet 변환 | POST `/filter` sentiment=`중립` (AC-1) |
| value=0, 오버플로, 음수 | T-01~T-07, 빈 Session, 빈 text, 필터 0건 |
| `meter:abc`, `:` 없음, parsec | CSV `text` 없음, H-1/H-2/H-3, alert 계약 |
| Domain 95% / Boundary 85% | Service ≥90% (PRD), stretch goal로 95/85 명시 |
| gcov UnitConverter.cpp | gcov TextAnalyzer/Filters/CsvParser |
| test_plan.md | docs/test_plan.md |

---

**User**

현 코드에 이미 테스트케이스가 작성되어있어? 

---

**Cursor**

**아니요. 현재 코드베이스에는 프로젝트용 테스트 케이스가 없습니다.**

확인 결과:

- `*test*` 패턴 파일 **0건** (`tests/` 폴더도 없음)
- `CMakeLists.txt`에 `enable_testing()`, Google Test, `feedback_analyzer_tests` 타깃 **없음**
- 실행 타깃은 `feedback_analyzer` **1개뿐**

README·PRD·analysis.md에 `TextAnalyzerTest`, `FiltersTest`, `CsvParserTest` 등이 나오지만, 이는 **Phase 1 목표**로만 적혀 있고 아직 구현되지 않았습니다.

참고로 `httplib.h` 안의 `TEST(...)` 언급은 외부 라이브러리 주석이며, 이 프로젝트 테스트가 아닙니다.

---

**User**

현 프로젝트로 변경해줘 아래 프롬포트를

@c:\DEV\UnitConverter_Cpp/README.md 파일에 아래 섹션을 추가해줘.
기존 내용은 유지하고, "## RED 단계 To-Do 리스트" 섹션만 새로 삽입.

추가할 섹션 형식:

## RED 단계 To-Do 리스트

> 이 체크리스트는 test_plan.md 기반으로 생성되었습니다.
> 각 항목은 RED(실패 테스트 작성) 완료 시 체크합니다.

### Track A — UI / Boundary 테스트
- [ ] TC-A-01: 정상 입력 "meter:2.5" → 변환 결과 반환 (Happy Path)
- [ ] TC-A-02: ":" 없는 입력 → std::invalid_argument 발생
- [ ] TC-A-03: 음수 입력 "meter:-1.0" → std::invalid_argument 발생
- [ ] TC-A-04: 없는 단위 "parsec:1.0" → std::invalid_argument 발생
- [ ] TC-A-05: 소수점 파싱 실패 "meter:abc" → std::invalid_argument 발생
- [ ] TC-A-06: 출력 포맷에 원 입력 단위·값 보존 ("2.5 meter = ...")
- [ ] TC-A-07: value=0 경계값 처리 확인

### Track B — Domain / Logic 테스트
- [ ] TC-B-01: convert("meter", 2.5, "feet") == 8.20210 (오차 1e-5)
- [ ] TC-B-02: convert("meter", 1.0, "yard") == 1.09361 (오차 1e-5)
- [ ] TC-B-03: convert("feet", 1.0, "meter") == 0.30480 (역변환)
- [ ] TC-B-04: convertAll("meter", 1.0) → 모든 등록 단위 변환 반환
- [ ] TC-B-05: registerUnit("cubit", 0.4572) 후 변환 가능
- [ ] TC-B-06: loadConfig(유효한 경로) → 비율 정상 로드
- [ ] TC-B-07: loadConfig(없는 경로) → 기본값(3.28084/1.09361) 유지

### 커버리지 목표
- [ ] Domain Logic: 95%+ (# gcov / lcov)
- [ ] Boundary Layer: 85%+
- [ ] 전체 TOTAL: 90%+

### 결함 목록 연결
- [ ] defect_list.md 생성 및 발견 결함 기록
- [ ] 모든 결함 수정 후 회귀 테스트 통과 확인


---

**Cursor**

**Feedback Analyzer용 프롬프트**

```
@c:\DEV\FeedbackAnalyzer_12\README.md 파일에 아래 섹션을 추가해줘.
기존 내용은 유지하고, "## RED 단계 To-Do 리스트" 섹션만 새로 삽입.
삽입 위치: "## 테스트 실행" 섹션 바로 아래 (또는 "## 알려진 이슈" 바로 위).
목차(## 목차)에도 해당 항목 링크를 추가.

근거: `docs/test_plan.md`, `docs/PRD.md` §4.3·§7.1, `docs/analysis.md` §6.1

추가할 섹션 형식:

## RED 단계 To-Do 리스트

> 이 체크리스트는 docs/test_plan.md 기반으로 생성되었습니다.
> 각 항목은 RED(실패 테스트 작성) 완료 시 체크합니다.
> Phase 1: T-03, T-05, T-06 failing test → H-1/H-2/H-3 수정 → Green.

### Track A — HTTP / Boundary 테스트 (수동·스모크)
- [ ] TC-A-01: POST `/analyze` — `text=배송이 너무 늦어요. 화가 납니다.` → 감정 stat 부정 +1, 키워드 stat 배송 +1 (Happy Path, T-02)
- [ ] TC-A-02: POST `/analyze` — `text=` 또는 공백만 → 피드백 미추가, 기존 건수 유지 (T-08)
- [ ] TC-A-03: Session 비어 있을 때 POST `/filter` → warning `분석할 피드백이 없습니다.` (T-09)
- [ ] TC-A-04: 필터 조건 불일치 → warning `필터링 결과가 없습니다.` (T-10)
- [ ] TC-A-05: GET `/download` 필터 미실행 → BOM + `text\n` 헤더만, 데이터 0건 (EX-04, M-6)
- [ ] TC-A-06: POST `/upload` — CSV `text` 컬럼 없음 (`id,comment`) → error `파일 업로드 중 오류가 발생했습니다.` (T-05, EX-06)
- [ ] TC-A-07: textarea 개행 입력 → analyze → filter → download 시 `\n` 유지 (T-11, AC-7)
- [ ] TC-A-08: 부록 A HTTP-S-01~05 엔드포인트 회귀 체크리스트 (AC-5)

### Track B — Domain / Service 단위 테스트 (Google Test)
- [ ] TC-B-01: T-01 — 빈 Feedback 목록 → analyzeSentiment → 긍정=0, 중립=0, 부정=0
- [ ] TC-B-02: T-03 / AC-1 / H-1 — `text="그냥 그래요. 특별한 감정 없음."` → filter sentiment=중립 → TextAnalyzer 중립 집합과 100% 일치
- [ ] TC-B-03: T-04 — sentiment=전체, keyword=전체 → 입력 전체 반환
- [ ] TC-B-04: T-05 / AC-2 / H-2 — CSV `id,comment\n1,hello` → `text` 컬럼 파싱, fields[0] 사용 금지
- [ ] TC-B-05: T-06 / AC-3 / H-3 — `text="택배가 빨라요."` → filter keyword=배송 → 1건 포함 (main 키워드)
- [ ] TC-B-06: T-07 — `text="품질이 별로예요."` → analyzeSentiment=중립 (카테고리≠감정 분리)
- [ ] TC-B-07: EX-09 — 긍정·부정 키워드 동시 포함 → 긍정 우선 (판정 순서 F-06)
- [ ] TC-B-08: Filters·TextAnalyzer — `Constants::SENTIMENT_KEYWORDS` 단일 소스 사용 (EX-01, M-7)

### 커버리지 목표
- [ ] TextAnalyzer / Filters / CsvParser / Feedback: 각 ≥ 90% (PRD §4.3, AC-4)
- [ ] Domain (Service) stretch: ≥ 95% (# gcov / lcov, `docs/test_plan.md` §6.2)
- [ ] Boundary (T-01~T-11, EX-01~EX-10): ≥ 85%+
- [ ] HTTP 핸들러 (main.cpp): 90% 목표 아님 — 부록 A 체크리스트로 검증

### 결함 목록 연결
- [ ] docs/defect_list.md 생성 — H-1(중립 필터), H-2(CSV text), H-3(main 키워드), H-4(테스트 부재) 기록
- [ ] 각 결함에 test_plan ID(T-03/T-05/T-06) 및 failing test명 연결
- [ ] H-1/H-2/H-3 수정 후 `ctest --test-dir build --output-on-failure` Green 확인
- [ ] Phase 1 Green 직후 lcov baseline 수립 (`docs/test_plan.md` §7.4)

코드 작성 금지. README.md Markdown 섹션만 출력·삽입.
UnitConverter 전용 내용(meter/feet, Catch2, parsec, std::invalid_argument)은 포함하지 말 것.
```

---

**원본 대비 치환**

| UnitConverter | Feedback Analyzer |
|---------------|-------------------|
| test_plan.md | docs/test_plan.md |
| Track A UI/Boundary | HTTP Form·CSV·alert·부록 A |
| Track B convert/registerUnit | TextAnalyzer·Filters·CsvParser (T-01~T-07) |
| meter:2.5, parsec, invalid_argument | sentiment=중립, CSV text, main 키워드 |
| Domain 95% / Boundary 85% | Service ≥90% (PRD) + stretch 95/85 |
| defect_list.md | docs/defect_list.md + H-1~H-4 |

---

**User**

아래 프롬포트를 현 프로젝트에 맞게 만들어줘. 그리고 현재 레거시코드는 건들지말라고해줘.

[P] 테스트 설계에 강한 시니어 C++ QA입니다.
[C] C++17, CMake, Catch2
[T] 아이템 타입별 최소 5개 테스트를 작성해줘.

    대상 케이스:
    - 정상 변환: meter→feet, meter→yard, feet→meter (역변환)
    - 경계값: value=0 / 매우 큰 수 / 소수점 6자리 정확도
    - 예외: 잘못된 형식 / 음수 / 없는 단위
    - 동적 등록: registerUnit 후 변환
    - 설정 로드: JSON/YAML 정상·실패 케이스

    형식 규칙:
    - Catch2 사용
    - Given-When-Then 주석 구조
    - 각 테스트에 변환 비율 명시 (1 meter = 3.28084 feet)
    - 테스트 이름: test_[변환타입]_[조건]_[기대결과] 형식

[F] 완성된 테스트 코드. cmake -S . -B build && cmake --build build가 Green이 되게 작성.


---

**Cursor**

**Feedback Analyzer용 프롬프트 (P-C-T-F)**

```
[P] 테스트 설계에 강한 시니어 C++ QA입니다. TDD RED 단계(실패 테스트 선작성)를 주도합니다.

[C] 워크스페이스: c:\DEV\FeedbackAnalyzer_12
    기술 스택: C++17, CMake 3.14+, Google Test, cpp-httplib
    근거 문서: docs/test_plan.md, docs/PRD.md §4.3·§7.1, docs/analysis.md §6.1, README.md §RED 단계 To-Do
    대표 샘플: POST `/filter` sentiment=중립 — AC-1 / H-1 (T-03)

    ⚠️ 레거시 코드 수정 금지:
    - src/cpp/ 아래 기존 파일(main.cpp, TextAnalyzer.*, Filters.*, Session.*,
      Constants.*, Logger.*, UIComponents.*, Feedback.h, FileHandler.h, httplib.h)는
      **한 줄도 수정·삭제·리팩토링하지 말 것**
    - 비즈니스 로직을 맞추기 위한 프로덕션 코드 변경 금지 (GREEN은 별도 Phase에서 진행)
    - 허용: tests/ 신규 추가, CMakeLists.txt에 **테스트 타깃만** 추가(enable_testing, GTest FetchContent, feedback_analyzer_tests)

[T] 아래 대상별 **최소 5개** Google Test를 작성해줘. (총 25개 이상)

    ### 1) TextAnalyzer (감정·키워드 분석)
    - 정상: 긍정/부정/중립 분류 1건씩 (판정 순서: 긍정 → 부정 → 중립)
    - 경계: 빈 Feedback 목록(T-01), 카테고리 키워드만·감정 키워드 없음(T-07)
    - 특이: 긍정·부정 키워드 동시 포함 → 긍정 우선(EX-09)
    - 키워드: `배송` main 키워드("택배" 등) 집계(T-06 연계)
    - Constants 연동: SENTIMENT_KEYWORDS / CATEGORY_KEYWORDS["main"] 기준 명시

    ### 2) Filters (감정·키워드 필터)
    - 정상: sentiment=부정 + keyword=배송 교집합(T-02)
    - 경계: sentiment=전체, keyword=전체 → 입력 전체 반환(T-04)
    - P0 / AC-1: sentiment=중립 → TextAnalyzer 중립 집합 100% 일치(T-03, H-1) ← **failing 예상**
    - P0 / AC-3: keyword=배송 → main 키워드 포함 피드백 포함(T-06, H-3) ← **failing 예상**
    - sentinel: `전체` = 해당 축 미적용

    ### 3) CsvParser (CSV 파싱 — tests 전용 또는 main.cpp에서 추출 전 stub)
    - 정상: `text\n배송이...\n` 2행 파싱
    - 경계: 헤더만(`text\n`) → 0건
    - P0 / AC-2: `text` 컬럼 없음 → 0건 또는 parse 실패(T-05, H-2) ← **failing 예상**
    - 추가 컬럼 허용·무시(PRD §5.3)
    - UTF-8 한글·개행 포함 text(AC-7 연계)
    ※ CsvParser 클래스가 아직 없으면 tests/CsvParser.h/cpp **테스트용 최소 구현**만 tests/ 또는 tests/support/에 추가
       (src/cpp/main.cpp의 parseCsvLine 로직 복사 금지 — 독립 stub; 레거시 미수정)

    ### 4) Feedback (도메인 모델)
    - 정상: 텍스트 생성·getText()
    - 경계: 빈 문자열, 공백만, 멀티라인(\n 포함)
    - 이동/복사 기본 동작
    - (권장) 긴 문자열 1건
    - UTF-8 한글 1건

    ### 5) KeywordUtils / containsAny (tests/support — Phase 2~3 대비)
    - 정상: 키워드 목록에 부분 문자열 포함 → true
    - 경계: 빈 text, 빈 keywords
    - 미포함 → false
    - 다중 키워드 중 하나 매칭
    - UTF-8 한글 키워드 매칭
    ※ src/cpp 중복 containsAny 통합 전 — tests/support/KeywordUtils.h에 **테스트용 헬퍼만** 작성

    형식 규칙:
    - **Google Test** 사용 (`TEST`, `TEST_F`; Catch2 사용 금지)
    - Given-When-Then 주석 블록 필수
    - 각 테스트에 PRD/test_plan ID 주석 (예: // T-03, AC-1, H-1)
    - 분류 규칙·키워드 출처 주석 명시 (예: Constants::SENTIMENT_KEYWORDS["부정"])
    - 테스트 이름: `Given_[전제]_When_[행동]_Then_[기대]` (Google Test 매크로 내)
    - H-1/H-2/H-3 재현 테스트는 **의도적으로 failing(Red)** — 프로덕션 수정 없이 Red 유지

[F] 완성된 테스트 코드 + CMake 테스트 타깃 설정.
    - 산출물: tests/*.cpp, (필요 시) tests/support/*.h, CMakeLists.txt **테스트 블록만** 추가
    - 빌드: cmake -S . -B build && cmake --build build → **feedback_analyzer_tests** 빌드 Green
    - 실행: ctest --test-dir build --output-on-failure
    - **기대:** T-03/T-05/T-06 등 P0 케이스는 **실패(Red)** — 전체 ctest Green 아님을 README/주석에 명시
    - feedback_analyzer(메인 executable) 빌드도 깨지지 않게 유지
    - UnitConverter 전용(meter/feet, 3.28084, Catch2, registerUnit, JSON/YAML) 코드 금지
```

---

**원본 대비 핵심 변경**

| UnitConverter | Feedback Analyzer |
|---------------|-------------------|
| Catch2 | **Google Test** |
| meter→feet 변환 5종 | TextAnalyzer / Filters / CsvParser / Feedback / KeywordUtils 각 5+ |
| 프로덕션 수정 OK | **src/cpp 레거시 수정 금지** (tests + CMake 테스트 타깃만) |
| ctest Green | **빌드 Green**, P0 테스트는 **Red 허용** |
| 3.28084 비율 주석 | Constants 키워드·AC/T/H ID 주석 |

---

**User**

아래 프롬포트를 현재 프로젝트에 맞게 수정해줘.

[P] 디버깅과 결함 분석에 능한 C++ QA 엔지니어입니다.
[T] (여기에 cmake -S . -B build && cmake --build build 실패 로그를 붙여넣을 것)

    1) 테스트 실패의 기대/실제 차이 요약
       예: expected 8.202100 but got 0.000000
    2) 버그 위치 특정 (파일명:줄번호)
       예: UnitConverter.cpp:27
    3) 결함 심각도 분류 및 근거:
       Critical   — 변환 결과가 완전히 틀린 경우
       Major      — 경계값에서 오차 초과
       Minor      — 출력 포맷 불일치
       Info       — 코드 스타일/주석 문제
    4) 최소 변경 수정 방안 (C++ 스타일)
       - if-else 체인 유지 최소 수정 (GREEN 통과 우선)
       - Item/기본 구조체 수정 금지

[F] 수정 diff 제안 + cmake -S . -B build && cmake --build build Green 확인 절차


---

**Cursor**

**Feedback Analyzer용 프롬프트 (P-C-T-F)**

```
[P] 디버깅과 결함 분석에 능한 C++ QA 엔지니어입니다.
    Feedback Analyzer(감정·키워드 분석, cpp-httplib 웹앱)의 Google Test / CMake 빌드 실패를 진단합니다.

[C] 워크스페이스: c:\DEV\FeedbackAnalyzer_12
    기술 스택: C++17, CMake 3.14+, Google Test, cpp-httplib
    근거 문서: docs/test_plan.md, docs/PRD.md §7.1 (AC-1~AC-4), docs/analysis.md §6
    알려진 P0: H-1(중립 필터), H-2(CSV text), H-3(main 키워드), H-4(테스트 부재)
    ⚠️ 수정 시 src/cpp/httplib.h diff 0, HTTP 5엔드포인트 계약·감정 3분류 라벨 변경 금지

[T] (여기에 아래 중 해당하는 실패 로그를 붙여넣을 것)
    - cmake -S . -B build && cmake --build build 실패 로그
    - ctest --test-dir build --output-on-failure 실패 로그
    - (선택) lcov/gcov 관련 오류

    1) 실패 요약 — 기대/실제 차이
       예 (단위 테스트):
         · expected filter result size 1 but got 0  (T-03, AC-1, H-1 — sentiment=중립)
         · expected Feedback.text "택배가 빨라요." in result but missing  (T-06, AC-3, H-3)
         · expected parsed feedbacks size 2 but got 0  (T-05, AC-2, H-2 — CSV text 컬럼)
         · expected sentiment map["중립"] == 1 but got 0  (TextAnalyzer vs Filters 불일치)
       예 (빌드 실패):
         · undefined reference to `CsvParser::parse`
         · FetchContent GoogleTest download failed
         · ws2_32 link error (Windows MinGW)

    2) 버그 위치 특정 (파일명:줄번호 + 함수/심볼)
       예:
         · Filters.cpp:37 — 중립 판정이 S_KEYWORDS["중립"] 키워드 의존 (TextAnalyzer와 불일치)
         · main.cpp:304 — CSV fields[0]만 사용, text 컬럼 미탐색
         · Filters.cpp:56 — main 키워드 skip (continue)
         · TextAnalyzer.h:29-34 — 긍/부정 미매칭 시 무조건 중립
         · CMakeLists.txt — enable_testing / GTest 타깃 누락

    3) 결함 심각도 분류 및 근거 (docs/analysis.md §6·§7 기준)
       High (P0)   — 핵심 기능 오류: H-1 중립 필터 불일치, H-2 CSV text, H-3 main 키워드, H-4 테스트 없음
       Medium (P1) — God Function, 전역 상태, containsAny 중복, Logger UI 미연동, download 대상 불명확
       Low (P2~P3) — 네이밍(sent/kw/fil), dead code, gitignore 잔재, localhost 보안
       Info        — 주석/문서/README·PRD 불일치, 커버리지 미달(90% 미만) 리포트만

       ※ test_plan Red 단계: T-03/T-05/T-06 failing은 **의도적 Red**일 수 있음 — 로그에 [RED-EXPECTED] 표시 시 Green 수정은 Phase 1 GREEN 작업으로 분리

    4) 최소 변경 수정 방안 (C++ 스타일, GREEN 통과 우선)
       - src/cpp/ 레거시는 **최소 diff**만; God Function 한 번에 분해 금지
       - H-1: Filters가 Constants::SENTIMENT_KEYWORDS 단일 소스 + TextAnalyzer와 동일 판정 순서(긍→부→중립)
       - H-2: main.cpp parseCsvLine 호출부에서 헤더 text 인덱스 탐색 (또는 CsvParser 추출 후 tests Green)
       - H-3: Filters.cpp main skip 제거; CATEGORY_KEYWORDS[cat]["main"] 매칭 통일
       - Feedback.h / httplib.h **구조·시그니처 변경 금지** (text getter 유지)
       - Constants 감정·카테고리 **라벨 문자열**(긍정/중립/부정) 변경 금지
       - 테스트 코드(tests/)만 깨진 경우: 프로덕션 수정 없이 테스트 기대값·include·CMake 타깃부터 점검

[F] 수정 diff 제안(변경 파일 목록 + 핵심 diff 요약) + Green 확인 절차

    ```powershell
    cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=C:/mingw64/bin/g++.exe
    cmake --build build
    ctest --test-dir build --output-on-failure
    build\feedback_analyzer.exe   # (선택) HTTP 스모크
    ```

    - 빌드 Green: feedback_analyzer + feedback_analyzer_tests 컴파일·링크 성공
    - 테스트 Green: ctest 전체 통과 (Red 예상 케이스는 수정 후 재실행)
    - 회귀: docs/PRD.md 부록 A 5항목 또는 test_plan HTTP-S-01~05 수동 1회

    UnitConverter 전용(meter/feet, 3.28084, Item 구조체, if-else 체인, Catch2) 언급·수정 금지.
```

---

**원본 대비 치환**

| UnitConverter | Feedback Analyzer |
|---------------|-------------------|
| 8.202100 / 0.000000 | 중립 필터 0건, CSV 0건, 배송 필터 누락 |
| UnitConverter.cpp:27 | Filters.cpp, main.cpp, TextAnalyzer.h 등 |
| Critical/Major/Minor | High/Medium/Low/Info (analysis.md) |
| Item 수정 금지 | Feedback.h·httplib.h·HTTP 계약 변경 금지 |
| if-else 최소 수정 | H-1/H-2/H-3 최소 diff, Constants 단일 소스 |

---

**User**

현 프로젝트에 맞게 아래 프롬포트를 변경해줘.

[P] QA 리드입니다.
[C] 현재까지 발견된 테스트 실패/결함들을 문서화합니다.
[T] 발견된 결함을 defect_list.md에 정리해줘.

    항목 형식:
    | ID | Severity | 변환 타입 | 재현 절차 | 기대값 | 실제값 | 근본 원인 | 수정 요약 |

    예시:
    | DEF-001 | Critical | meter→feet | "meter:2.5" 입력 | 8.202100 | 0.000000 |
    | if-else 체인에서 feet 분기 누락 | meterValue * 3.28084 추가 |

[F] Markdown. defect_list.md로 저장.
    README의 "RED 단계 To-Do 리스트 > 결함 목록 연결" 체크박스도 업데이트.


---

**Cursor**

**Feedback Analyzer용 프롬프트 (P-C-T-F)**

```
[P] QA 리드입니다. Feedback Analyzer 리팩토링 챌린지의 테스트 실패·결함을 추적·문서화합니다.

[C] 워크스페이스: c:\DEV\FeedbackAnalyzer_12
    근거 문서: docs/analysis.md §6·§7, docs/test_plan.md §4~§5, docs/PRD.md §7.1 (AC-1~AC-4), README.md §RED 단계 To-Do
    입력(있을 경우): ctest 실패 로그, Red 단계 failing test 결과, 수동 재현 메모
    ⚠️ defect_list.md 작성 단계에서는 src/cpp 레거시 코드 수정 금지 (문서화만)

[T] 발견된 결함을 docs/defect_list.md에 정리해줘.

    문서 상단 메타:
    - 작성일, 근거 문서, Phase (Phase 1 RED/GREEN), 연결 test_plan ID

    항목 형식 (표):
    | ID | Severity | 기능/레이어 | test_plan ID | 재현 절차 | 기대값 | 실제값 | 근본 원인 (파일:줄) | 수정 요약 | 상태 |

    Severity (docs/analysis.md §6·§7):
    - High (P0)  — H-1~H-4, 핵심 기능·회귀 방지
    - Medium (P1) — M-1~M-7
    - Low (P2~P3) — L-1~L-6
    - Info       — 문서/스타일/커버리지 리포트

    상태: Open | In Progress | Fixed | Won't Fix | RED-Expected

    필수 포함 결함 (analysis.md 기준, 최소 4건):
    - H-1 / AC-1 / T-03 — 중립 필터 vs TextAnalyzer 불일치
    - H-2 / AC-2 / T-05 — CSV text 컬럼 미사용
    - H-3 / AC-3 / T-06 — Filters main 키워드 skip
    - H-4 — Google Test·CMake 테스트 타깃 부재

    선택 포함 (Medium 이상, 재현 가능한 것):
    - M-1 God Function, M-2 전역 상태, M-4 Logger UI, M-6 download 대상, M-7 이중 키워드 사전

    예시 행:
    | DEF-001 | High (P0) | Filters / sentiment=중립 | T-03, AC-1, H-1 |
    | Session에 Feedback("그냥 그래요.") → POST /filter sentiment=중립&keyword=전체 |
    | Filters 결과 1건 = TextAnalyzer 중립 집합 | 0건 (또는 불일치) |
    | TextAnalyzer는 긍/부 미매칭→중립, Filters는 S_KEYWORDS["중립"] 필요 |
    | Filters.cpp:37, TextAnalyzer.h:29 | Constants 단일 소스 + 동일 판정 순서 | Open |

    | DEF-002 | High (P0) | CsvParser / POST /upload | T-05, AC-2, H-2 |
    | CSV `id,comment\n1,hello` 업로드 | text 컬럼 값만 Feedback 적재 | fields[0]="1" 적재 또는 무시 |
    | main.cpp:304 fields[0]만 사용 | main.cpp 또는 CsvParser — text 헤더 인덱스 탐색 | Open |

    | DEF-003 | High (P0) | Filters / keyword=배송 | T-06, AC-3, H-3 |
    | Feedback("택배가 빨라요.") → filter keyword=배송 | 1건 포함 | 0건 |
    | Filters.cpp:56 main skip | main 키워드 매칭 통일 | Open |

    각 P0 행에 failing test명(예정) 연결:
    - Given_NeutralText_When_FilterSentimentNeutral_Then_MatchesTextAnalyzerSet
    - Given_CsvWithoutTextColumn_When_Parse_Then_ZeroFeedbacks
    - Given_MainKeywordOnly_When_FilterDelivery_Then_Included

[F] Markdown. docs/defect_list.md로 저장.
    README.md "RED 단계 To-Do 리스트 > 결함 목록 연결" 체크박스도 함께 업데이트:
    - [x] docs/defect_list.md 생성 — H-1~H-4 기록
    - [x] 각 결함에 test_plan ID(T-03/T-05/T-06) 및 failing test명 연결
    - [ ] H-1/H-2/H-3 수정 후 ctest Green 확인  ← 아직 Open이면 unchecked 유지
    - [ ] Phase 1 Green 직후 lcov baseline 수립  ← 동일

    UnitConverter 전용(meter→feet, 3.28084, if-else 체인, Item 구조체) 항목 금지.
    코드 diff·프로덕션 수정 금지 — 문서·README 체크박스만.
```

---

**원본 대비 치환**

| UnitConverter | Feedback Analyzer |
|---------------|-------------------|
| 변환 타입 | 기능/레이어 (Filters, CsvParser, TextAnalyzer…) |
| Critical | High (P0) — H-1~H-4 |
| meter:2.5 → 8.202100 | sentiment=중립, CSV text, keyword=배송 |
| UnitConverter.cpp:27 | Filters.cpp, main.cpp, TextAnalyzer.h |
| defect_list.md | docs/defect_list.md |
| README 체크박스 1개 | README §결함 목록 연결 4개 항목 |

---

**User**

아래 프롬포트를 현재 프로젝트에 맞게 바꿔줘. 레거시 코드는 건들면안돼.



당신은 Dual-Track UI + Logic TDD 전문가입니다.
프로젝트: c:\DEV\UnitConverter_Cpp
현재 코드 상태: 단일 main() 함수, if-else 체인, 예외 처리 없음

중요: 구현 코드 작성 금지 / GREEN 단계 금지 / REFACTOR 금지
오직 RED 테스트만 작성하십시오.

프로젝트 조건:
- 입력: std::string (단위:값) (예: "meter:2.5")
- 변환: 1 meter = 3.28084 feet, 1 meter = 1.09361 yard
- 출력: "2.5 meter = 8.202100 feet" 형식
- 예외: std::invalid_argument

TRACK A — UI / Boundary RED:
1. "meter:2.5" 정상 입력 → 변환 결과 반환
2. ":" 없는 입력 → 예외 발생 (잘못된 형식)
3. 음수 값 입력 ("meter:-1.0") → 예외 발생
4. 없는 단위 ("parsec:1.0") → 예외 발생
5. 반환 포맷에 원 입력 단위·값 보존 확인
6. JSON 출력 요청 시 올바른 스키마 반환
각 테스트에: 테스트 이름 / Given / When / Then / 보호하는 계약

TRACK B — Domain / Logic RED:
1. convert(fromUnit, value, toUnit) — meter↔feet 정확도 (1e-5 이내)
2. convert(fromUnit, value, toUnit) — meter↔yard 정확도
3. convertAll(fromUnit, value) — 모든 등록 단위로 변환 반환
4. registerUnit(name, ratio_to_meter) — 새 단위 등록 후 변환 가능
5. loadConfig(path) — JSON/YAML 설정 파일 로드 후 비율 적용
6. loadConfig(invalid_path) — 파일 없을 때 기본값 유지
각 테스트: 실패 상태 명확히 설명 / 보호 Invariant 명시

출력 형식:
# UI RED Tests — Test ID / Given/When/Then / Invariant
# Logic RED Tests — Test ID / Scenario / Invariant


---

**Cursor**

**Feedback Analyzer용 프롬프트 (Dual-Track RED only)**

```
당신은 Dual-Track UI + Logic TDD 전문가입니다.
프로젝트: c:\DEV\FeedbackAnalyzer_12
현재 코드 상태: main.cpp God Function, TextAnalyzer/Filters 분리 불완전,
                  감정 키워드 이중 관리(S_KEYWORDS), P0 버그 H-1~H-3 존재, Google Test 0건

중요:
- 구현 코드 작성 금지 / GREEN 단계 금지 / REFACTOR 금지
- 오직 RED 테스트만 작성하십시오
- ⚠️ 레거시 코드 수정 금지:
  src/cpp/ 아래 기존 파일(main.cpp, TextAnalyzer.*, Filters.*, Session.*,
  Constants.*, Logger.*, UIComponents.*, Feedback.h, FileHandler.h, httplib.h)는
  한 줄도 수정·삭제·리팩토링하지 말 것
- 허용: tests/ 신규 추가, tests/support/ 테스트용 stub,
  CMakeLists.txt에 테스트 타깃(feedback_analyzer_tests)만 추가

근거 문서: docs/test_plan.md, docs/PRD.md §3.2·§4.3·§7.1, docs/analysis.md §6

프로젝트 조건:
- 입력: HTTP form / multipart CSV (POST /analyze, /upload, /filter)
- 분류: 감정 3분류(긍정→부정→중립), 키워드 카테고리(main 키워드), sentinel `전체`
- 출력: HTML stat + alert(success/warning/error), CSV 다운로드(UTF-8 BOM + text\n)
- P0 결함: H-1(중립 필터), H-2(CSV text), H-3(main 키워드 skip)

---

TRACK A — HTTP / Boundary RED (수동·스모크 또는 Handler 테스트 스펙):
1. POST /analyze — text="배송이 너무 늦어요. 화가 납니다." → 감정 stat 부정 +1, 키워드 stat 배송 +1 (T-02, Happy Path)
2. POST /analyze — text= 또는 공백만 → 피드백 미추가, 건수 유지 (T-08)
3. Session 비어 있을 때 POST /filter → warning `분석할 피드백이 없습니다.` (T-09)
4. 필터 조건 불일치 → warning `필터링 결과가 없습니다.` (T-10)
5. GET /download — /filter 미실행 → BOM + text\n 헤더만, 데이터 0건 (EX-04, M-6)
6. POST /upload — CSV text 컬럼 없음(id,comment) → error `파일 업로드 중 오류가 발생했습니다.` (T-05, EX-06)
7. textarea 개행 입력 → analyze → filter → download 시 \n 유지 (T-11, AC-7)
8. 부록 A HTTP-S-01~05 엔드포인트 계약 (AC-5)

각 테스트에: Test ID / Given / When / Then / 보호하는 계약(PRD F-xx, AC-x)

---

TRACK B — Domain / Service RED (Google Test, tests/*.cpp):
1. TextAnalyzer — 빈 목록 → 긍정=0, 중립=0, 부정=0 (T-01)
2. TextAnalyzer — "품질이 별로예요." → analyzeSentiment=중립 (T-07, 카테고리≠감정)
3. TextAnalyzer — 긍·부 키워드 동시 → 긍정 우선 (EX-09, F-06)
4. Filters — sentiment=중립 → TextAnalyzer 중립 집합 100% 일치 (T-03, AC-1, H-1) ← failing 예상
5. Filters — keyword=배송, text="택배가 빨라요." → 1건 포함 (T-06, AC-3, H-3) ← failing 예상
6. Filters — sentiment=전체, keyword=전체 → 입력 전체 반환 (T-04)
7. CsvParser — CSV `text\n배송...\n` → 2건 파싱 (정상)
8. CsvParser — `id,comment\n1,hello` → text 컬럼 미사용, 0건 또는 parse 실패 (T-05, AC-2, H-2) ← failing 예상
9. Feedback — 멀티라인 text 생성·getText() (AC-7 연계)
10. KeywordUtils — containsAny UTF-8 한글 키워드 매칭 (M-3 대비, tests/support만)

각 테스트:
- Google Test 매크로명: Given_[전제]_When_[행동]_Then_[기대]
- Given-When-Then 주석 블록
- 실패 상태 명확히 설명 (현재 As-Is vs To-Be)
- 보호 Invariant 명시 (예: AC-1, F-06, Constants 단일 소스)
- test_plan ID 주석 (T-03, H-1 등)

형식 규칙:
- Catch2 사용 금지 → Google Test (TEST / TEST_F)
- meter/feet, 3.28084, registerUnit, loadConfig, JSON 출력 금지

---

출력 형식:

# HTTP / Boundary RED Tests
| Test ID | Given | When | Then | Invariant (PRD) | Expected RED |

# Domain / Service RED Tests
| Test ID | Scenario (Google Test name) | Invariant | Expected RED (As-Is) |

# CMake (테스트 타깃만)
- feedback_analyzer_tests 타깃 추가 요약 (레거시 executable 변경 없음)

# 실행 기대
- cmake --build build → feedback_analyzer_tests 빌드 Green
- ctest → T-03/T-05/T-06 등 P0 케이스 **실패(Red)** — 전체 Green 아님을 명시

구현 코드(GREEN/REFACTOR) 출력 금지. tests/ 코드와 테스트 스펙 문서만 출력.
```

---

**Track 매핑**

| UnitConverter | Feedback Analyzer |
|---------------|-------------------|
| Track A UI (meter:2.5, JSON) | Track A HTTP/Boundary (POST /analyze·/filter·/upload, GET /download) |
| Track B convert/registerUnit | Track B TextAnalyzer·Filters·CsvParser·Feedback |
| std::invalid_argument | warning/error alert, 0건·불일치 집합 |
| 3.28084 / 1.09361 | AC-1 중립 일치, AC-2 text 컬럼, AC-3 main 키워드 |
| 구현 금지 | **src/cpp 레거시 수정 금지** + RED only |

---

**User**

현 프로젝트에 맞게 아래 프롬포트를 변경해줘.

Catch2 테스트 스켈레톤을 작성하라.
파일/클래스 이름만 적고, 메서드 본문은 FAIL("RED") 한 줄만.
RED 원칙으로만 작성하십시오.

예시:
TEST_CASE("convert_meter_to_feet_returns_correct_ratio","[domain]") {   FAIL("RED"); }


---

**Cursor**

**Feedback Analyzer용 프롬프트**

```
Google Test 테스트 스켈레톤을 작성하라.
파일/클래스(TEST_F fixture) 이름만 적고, 테스트 본문은 FAIL() << "RED"; 한 줄만.
RED 원칙으로만 작성하십시오.

⚠️ 레거시 코드 수정 금지:
- src/cpp/ 아래 기존 파일은 한 줄도 수정하지 말 것
- 허용: tests/*.cpp, tests/support/*.h, CMakeLists.txt 테스트 타깃만 추가

근거: docs/test_plan.md §3~§4, docs/PRD.md §4.3 (T-01~T-07), README §RED 단계 To-Do

대상 파일 (예정):
- tests/TextAnalyzerTest.cpp
- tests/FiltersTest.cpp
- tests/CsvParserTest.cpp
- tests/FeedbackTest.cpp
- tests/KeywordUtilsTest.cpp   (tests/support/KeywordUtils.h stub 연동)

예시:
TEST(Given_NeutralText_When_AnalyzeSentiment_Then_ClassifiedAsNeutral, TextAnalyzer) {
    FAIL() << "RED";  // T-03, AC-1, H-1
}

TEST_F(FiltersTest, Given_MainKeywordOnly_When_FilterDelivery_Then_Included) {
    FAIL() << "RED";  // T-06, AC-3, H-3
}

포함할 RED 케이스 (각 파일 최소 스켈레톤 1개, P0는 필수):
- TextAnalyzerTest: T-01, T-03, T-07, EX-09
- FiltersTest: T-02, T-03, T-04, T-06
- CsvParserTest: T-05, 정상 CSV 2행, 헤더만
- FeedbackTest: 멀티라인, 빈 문자열
- KeywordUtilsTest: containsAny 한글 매칭

각 TEST 위에 Given-When-Then 주석 1블록 + test_plan ID 주석.
Catch2(TEST_CASE) 사용 금지. meter/feet, 3.28084, registerUnit 금지.
구현 본문·GREEN 코드·src/cpp 수정 금지.
```

---

**스켈레톤 출력 예시** (프롬프트 실행 시 기대 형태)

```cpp
// tests/TextAnalyzerTest.cpp
// T-03, AC-1, H-1
// Given: Feedback("그냥 그래요. 특별한 감정 없음.")
// When:  analyzeSentiment
// Then:  중립 1건
TEST(Given_NeutralText_When_AnalyzeSentiment_Then_ClassifiedAsNeutral, TextAnalyzer) {
    FAIL() << "RED";
}
```

```cpp
// tests/FiltersTest.cpp
// T-06, AC-3, H-3
TEST(Given_MainKeywordOnly_When_FilterDelivery_Then_Included, Filters) {
    FAIL() << "RED";
}
```

```cpp
// tests/CsvParserTest.cpp
// T-05, AC-2, H-2
TEST(Given_CsvWithoutTextColumn_When_Parse_Then_ZeroFeedbacks, CsvParser) {
    FAIL() << "RED";
}
```

---

| UnitConverter | Feedback Analyzer |
|---------------|-------------------|
| Catch2 `TEST_CASE` | Google Test `TEST` / `TEST_F` |
| `FAIL("RED")` | `FAIL() << "RED";` |
| convert_meter_to_feet | Given_…_When_…_Then_… (T-xx ID) |

---

**User**

현 프로젝트에 맞게 아래 프롬포트를 수정해줘.

[단계] TDD GREEN만 수행한다. REFACTOR는 하지 않는다.
프로젝트: c:\DEV\UnitConverter_Cpp

대상 테스트: TC-B-01
  파일: tests/test_converter.cpp
  내용: REQUIRE( convert("meter", 2.5, "feet") == Approx(8.20210).epsilon(1e-5) );

PRD §3.2 비즈니스 로직:
  1 meter = 3.28084 feet
  1 meter = 1.09361 yard
  (비율은 상수로 추출 — 인라인 하드코딩 금지)

할 일:
1. cmake -S . -B build && cmake --build build 실행 → TC-B-01 FAIL 확인
2. convert() 함수 최소 구현 (meter→feet 분기만)
3. cmake -S . -B build && cmake --build build 재실행 → TC-B-01 PASS 확인

금지:
- 다른 TC (TC-B-02~07, TC-A-01~07) 동시 구현
- REFACTOR (이름 정리, 구조 분리) 이번 커밋에서 금지
- 비율 3.28084 인라인 사용 금지 (상수 정의 후 사용)

산출물:
- TC-B-01이 통과하는 최소 프로덕션 코드
- 커밋 메시지 제안: feat(green): implement convert meter to feet (Approx 1e-5)


---

**Cursor**

**Feedback Analyzer용 GREEN 프롬프트**

```
[단계] TDD GREEN만 수행한다. REFACTOR는 하지 않는다.
프로젝트: c:\DEV\FeedbackAnalyzer_12

대상 테스트: TC-B-02
  파일: tests/FiltersTest.cpp
  테스트: Given_NeutralText_When_FilterSentimentNeutral_Then_MatchesTextAnalyzerSet
  내용(구현 후):
    // Given: Feedback("그냥 그래요. 특별한 감정 없음.")
    // When:  Filters::fil(feedbacks, u8"중립", u8"전체")
    // Then:  결과 집합 == TextAnalyzer가 동일 입력에 대해 중립으로 분류한 집합
    EXPECT_EQ(filtered.size(), 1u);
    // TextAnalyzer 중립 subset과 100% 일치 (AC-1)

  test_plan: T-03, AC-1, H-1
  defect: DEF-001 (docs/defect_list.md)

PRD §3.2 F-06 / §3.3 비즈니스 로직:
  - 감정 판정 순서: 긍정 → 부정 → 중립 (기본값)
  - TextAnalyzer·Filters **동일 규칙** 사용
  - 감정 키워드 **단일 소스**: Constants::SENTIMENT_KEYWORDS
    (Filters::S_KEYWORDS / initFilterKeywords() 의존 제거 — 이번 TC만 최소 범위)
  - 키워드 문자열 u8"긍정"|u8"중립"|u8"부정" 인라인 하드코딩 금지 → Constants 참조

할 일:
1. cmake -S . -B build && cmake --build build && ctest --test-dir build --output-on-failure
   → TC-B-02 (및 RED 스켈레톤) FAIL 확인
2. **최소 프로덕션 수정** — Filters.cpp(또는 Filters.h)만:
   · sentiment=중립 판정을 TextAnalyzer와 동일하게 (긍/부 미매칭 → 중립)
   · S_KEYWORDS[u8"중립"] 전용 키워드 조건 제거 또는 Constants 단일 소스로 교체
3. tests/FiltersTest.cpp — 해당 TC만 FAIL() 제거, Given-When-Then assertion 작성
4. cmake --build build && ctest --test-dir build -R "Given_NeutralText_When_FilterSentimentNeutral" → PASS 확인

금지:
- 다른 TC (TC-B-01, TC-B-03~08, TC-A-01~08) 동시 구현·수정
- REFACTOR (HtmlRenderer 분리, AppState, Rename sent/kw/fil, main.cpp 대규모 변경) 이번 작업에서 금지
- src/cpp/httplib.h, Feedback.h 시그니처 변경 금지
- HTTP 5엔드포인트·감정 3분류 라벨 변경 금지
- H-2/H-3(T-05/T-06) 이번 커밋에서 수정 금지

산출물:
- TC-B-02가 통과하는 최소 프로덕션 코드 (Filters ± Constants 최소 diff)
- TC-B-02 테스트 본문 (FAIL 제거)
- 커밋 메시지 제안: feat(green): fix neutral filter to match TextAnalyzer (AC-1, H-1)
```

---

**README TC-B-01(T-01)을 첫 GREEN으로 할 때** (더 단순한 경우)

```
대상 테스트: TC-B-01
  파일: tests/TextAnalyzerTest.cpp
  테스트: Given_EmptyFeedbackList_When_AnalyzeSentiment_Then_AllCountsAreZero
  Then: sent({}) → 긍정=0, 중립=0, 부정=0 (T-01)

할 일: 테스트 본문만 구현 → TextAnalyzer::sent()는 이미 0 반환 가능 → prod 변경 없이 PASS 가능
금지/산출물: 동일 (다른 TC 동시 수정 금지)
커밋: test(green): implement T-01 empty feedback sentiment counts
```

---

**UnitConverter → Feedback Analyzer 치환**

| UnitConverter | Feedback Analyzer |
|---------------|-------------------|
| Catch2 `REQUIRE(Approx…)` | Google Test `EXPECT_EQ` / `ASSERT_*` |
| TC-B-01 convert meter→feet | **TC-B-02** T-03 중립 필터 (핵심 로직) 또는 TC-B-01 T-01 |
| 3.28084 상수 | `Constants::SENTIMENT_KEYWORDS` 단일 소스 |
| convert() 최소 구현 | Filters 중립 판정 최소 수정 |
| test_converter.cpp | tests/FiltersTest.cpp |

---

**User**

아래 프롬포트를 현 프로젝트에 맞게 변경해줘.



커밋 순서	Track	대상 TC	구현 내용	커밋 메시지
1번째	Track B	TC-B-01	convert("meter",v,"feet") 최소 구현	feat(green): meter to feet
2번째	Track A	TC-A-02	":" 없는 입력 → 예외 반환	feat(green): validate missing colon
3번째	Track B	TC-B-02	convert("meter",v,"yard") 추가	feat(green): meter to yard
4번째	Track A	TC-A-03	음수 입력 → 예외 반환	feat(green): validate negative value
5번째	Track B	TC-B-03	convert("feet",v,"meter") 역변환	feat(green): feet to meter reverse
6번째	Track A	TC-A-04	없는 단위 → 예외 반환	feat(green): validate unknown unit
7번째	Track B	TC-B-04~05	convertAll() + registerUnit()	feat(green): convertAll and registerUnit
8번째	Track A	TC-A-01,06,07	Happy path + 포맷 + 경계값	feat(green): boundary happy path
9번째	Track B	TC-B-06~07	loadConfig() 정상·실패	feat(green): loadConfig with fallback

당신은 Dual-Track UI(Boundary) + Logic(Domain) TDD 실무 전문가입니다.
프로젝트: c:\DEV\UnitConverter_Cpp

이번 커밋에서 처리할 묶음을 1개 선택하고 아래 순서로 진행하라:

Step 1) 대상 TC 확인 (위 커밋 순서표 참고)
Step 2) cmake -S . -B build && cmake --build build → 대상 TC가 FAIL인지 확인
Step 3) 대상 TC만 통과하는 최소 구현 작성
Step 4) cmake -S . -B build && cmake --build build 재실행 → 대상 TC PASS, 나머지 영향 없음 확인
Step 5) 커밋

출력 형식:
1) 이번 커밋 목표 (Track A 또는 B, TC ID)
2) 추가/수정한 코드 범위
3) 테스트 결과 (PASS/FAIL 목록)
4) 커밋 메시지 (Conventional Commit)
5) 다음 커밋 후보 TC


---

**Cursor**

**Feedback Analyzer용 Dual-Track GREEN 커밋 순서 + 실행 프롬프트**

---

### 커밋 순서표

| 커밋 순서 | Track | 대상 TC | 구현 내용 | 커밋 메시지 |
|-----------|-------|---------|-----------|-------------|
| 1번째 | Track B | TC-B-01 | T-01 — 빈 Feedback → analyzeSentiment 긍/중/부=0 (테스트 assertion) | `test(green): T-01 empty feedback sentiment counts` |
| 2번째 | Track B | **TC-B-02** | T-03 / H-1 — sentiment=중립 필터 = TextAnalyzer 중립 집합 일치 | `feat(green): fix neutral filter to match TextAnalyzer (AC-1, H-1)` ✅ **완료** |
| 3번째 | Track B | TC-B-05 | T-06 / H-3 — keyword=배송 시 main 키워드("택배" 등) 포함 | `feat(green): include main keywords in category filter (AC-3, H-3)` |
| 4번째 | Track B | TC-B-04 | T-05 / H-2 — CSV 헤더 `text` 컬럼 파싱 (fields[0] 금지) | `feat(green): parse CSV text column by header (AC-2, H-2)` |
| 5번째 | Track B | TC-B-03 | T-04 — sentiment=전체, keyword=전체 → 입력 전체 반환 | `feat(green): filter sentinel 전체 returns all feedbacks` |
| 6번째 | Track B | TC-B-06 | T-07 — 카테고리 키워드만 → analyzeSentiment=중립 | `feat(green): separate category from sentiment (T-07)` |
| 7번째 | Track B | TC-B-07 | EX-09 — 긍·부 동시 → 긍정 우선 (F-06) | `feat(green): positive-first sentiment order (EX-09)` |
| 8번째 | Track B | TC-B-08 | EX-01 / M-7 — `initFilterKeywords()`·`S_KEYWORDS` 제거, Constants 단일 소스 | `feat(green): unify sentiment keywords in Constants (M-7)` |
| 9번째 | Track A | TC-A-02 | T-08 — 빈/공백 text → 피드백 미추가 | `feat(green): skip empty analyze text (T-08)` |
| 10번째 | Track A | TC-A-03 | T-09 — Session 비어 있을 때 `/filter` → warning | `feat(green): warn when filtering empty session (T-09)` |
| 11번째 | Track A | TC-A-04 | T-10 — 필터 0건 → warning `필터링 결과가 없습니다.` | `feat(green): warn when filter returns zero (T-10)` |
| 12번째 | Track A | TC-A-01 | T-02 — Happy Path analyze 부정+배송 stat | `feat(green): analyze happy path negative delivery (T-02)` |
| 13번째 | Track A | TC-A-06 | T-05 / EX-06 — CSV `text` 없음 → error alert | `feat(green): upload error when text column missing` |
| 14번째 | Track A | TC-A-05, TC-A-07 | EX-04/M-6 download 빈 CSV + T-11 개행 유지 | `feat(green): download boundary and multiline (M-6, AC-7)` |
| 15번째 | Track A | TC-A-08 | 부록 A HTTP-S-01~05 회귀 체크리스트 (AC-5) | `docs(green): verify appendix A endpoint checklist (AC-5)` |

> **현재 진행:** 2번째(TC-B-02) 완료 — [`Report/Green/09.Green_TC-B-02_진행완료.md`](Report/Green/09.Green_TC-B-02_진행완료.md)  
> **다음 후보:** 3번째 **TC-B-05** (H-3, P0)

---

### 실행 프롬프트

```
당신은 Dual-Track HTTP(Boundary) + Domain(Service) TDD 실무 전문가입니다.
프로젝트: c:\DEV\FeedbackAnalyzer_12
기술 스택: C++17, CMake 3.14+, Google Test, cpp-httplib

근거: README.md §RED 단계 To-Do, docs/test_plan.md, docs/PRD.md §7.1, docs/defect_list.md
완료 이력: TC-B-02 Green (Report/Green/09.Green_TC-B-02_진행완료.md)

[단계] TDD GREEN만. REFACTOR 금지 (HtmlRenderer 분리, AppState, Rename, main 대규모 변경).
[원칙] 커밋 1건 = TC 1건(또는 표에 묶인 TC만). 최소 diff.

⚠️ 금지:
- src/cpp/httplib.h 수정
- Feedback.h 시그니처 변경
- HTTP 5엔드포인트 path/method·감정 3분류 라벨 변경
- 표에 없는 TC 동시 구현
- Catch2 / meter·feet / registerUnit / loadConfig

이번 커밋에서 처리할 묶음을 커밋 순서표에서 1개 선택하고 아래 순서로 진행하라:

Step 1) 대상 TC 확인 (위 커밋 순서표 · 미완료 항목 우선: **TC-B-05**)
Step 2) cmake -S . -B build && cmake --build build
       ctest --test-dir build --output-on-failure
       → 대상 TC FAIL(Red) 확인
Step 3) 대상 TC만 통과하는 최소 구현
       · Track B: src/cpp/Filters.*, TextAnalyzer.*, Constants.*, main.cpp(CsvParser만 TC-B-04 시) 등 **최소 파일**
       · Track A: main.cpp 라우트 핸들러 최소 수정 (해당 alert/동작만)
       · tests/*.cpp — 해당 TC만 FAIL() 제거 + assertion
Step 4) ctest -R "<테스트명 패턴>" → 대상 TC PASS
       ctest 전체 → 기존 Green(TC-B-02 등) 회귀 없음 확인
Step 5) 커밋 (Conventional Commit)

출력 형식:
1) 이번 커밋 목표 (Track A 또는 B, TC ID, test_plan ID, defect ID)
2) 추가/수정한 코드 범위 (파일 목록)
3) 테스트 결과 (PASS/FAIL 목록, ctest N/M)
4) 커밋 메시지 (Conventional Commit)
5) 다음 커밋 후보 TC (순서표 기준)
```

---

**UnitConverter → Feedback Analyzer 치환 요약**

| UnitConverter | Feedback Analyzer |
|---------------|-------------------|
| convert meter→feet | 중립 필터 / CSV text / main 키워드 |
| Track A 예외 (`:` 없음, 음수) | Track A HTTP warning/error alert |
| convertAll, registerUnit, loadConfig | (없음 — Phase 5 Trend/File DB는 별도) |
| TC-B-06~07 loadConfig | TC-B-06~08 Domain + Track A 묶음 |

---

**User**

현재 프로젝트에 맞게 아래 프롬포트 변경해줘.

모든 TC(TC-A-01~07, TC-B-01~07)가 통과한 후 전체 테스트를 실행하라.

cmake -S . -B build && cmake --build build ctest --test-dir build -V

커버리지 측정:
# gcov / lcov gcov UnitConverter.cpp

확인 항목:
- 모든 TC PASS (0 failures)
- Domain Logic 커버리지 ≥ 95%
- Boundary Layer 커버리지 ≥ 85%
- 비율 상수 3.28084 / 1.09361이 인라인으로 남아 있지 않음
- main() 함수에 변환 로직이 남아 있지 않음 (Domain으로 분리됨)

이후: green → A_01 PR 생성 → 리뷰 → 머지


---

**Cursor**

**Feedback Analyzer용 — Phase 1 GREEN 완료 검증 프롬프트**

```
모든 TC(TC-A-01~08, TC-B-01~08)가 통과한 후 전체 테스트를 실행하라.

프로젝트: c:\DEV\FeedbackAnalyzer_12
근거: README.md §RED 단계 To-Do, docs/test_plan.md §6~§7, docs/PRD.md §7.1 (AC-4~AC-5)

---

## 1) 전체 테스트 실행

cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=C:/mingw64/bin/g++.exe
cmake --build build
ctest --test-dir build -V --output-on-failure

(커버리지 빌드 시)
cmake -S . -B build -DENABLE_COVERAGE=ON
cmake --build build
ctest --test-dir build --output-on-failure

---

## 2) 커버리지 측정 (gcov / lcov)

# 측정 대상 (Service / Domain — docs/test_plan.md §7.2)
#   src/cpp/TextAnalyzer.cpp
#   src/cpp/Filters.cpp
#   src/cpp/CsvParser.cpp  (또는 main.cpp에서 추출된 파서)
#   Feedback 관련

lcov --capture --directory build --output-file coverage.info
lcov --remove coverage.info '*/tests/*' '*/googletest/*' '*/httplib.h' --output-file coverage.filtered.info
genhtml coverage.filtered.info --output-directory build/coverage_html

# 제외: main.cpp(HTTP), httplib.h (부록 A 수동 검증)

---

## 3) 확인 항목

### 테스트
- [ ] ctest **0 failures** — TC-A-01~08, TC-B-01~08 전부 PASS
- [ ] TC-B-02 (T-03 / AC-1 / H-1) — 중립 필터 = TextAnalyzer 중립 집합 100% 일치
- [ ] TC-B-04 (T-05 / AC-2 / H-2) — CSV `text` 컬럼 파싱, fields[0] 미사용
- [ ] TC-B-05 (T-06 / AC-3 / H-3) — keyword=배송 시 main 키워드 포함
- [ ] TC-A-08 — docs/PRD.md 부록 A HTTP-S-01~05 회귀 (AC-5)

### 커버리지 (docs/test_plan.md §6.1~§6.2)
- [ ] TextAnalyzer / Filters / CsvParser / Feedback: 각 **≥ 90%** (PRD AC-4 필수)
- [ ] Domain (Service) stretch: **≥ 95%** (TextAnalyzer + Filters + CsvParser 합산)
- [ ] Boundary (T-01~T-11, EX-01~EX-10, Track A): **≥ 85%**
- [ ] HTTP 핸들러(main.cpp): 90% 목표 **아님** — 부록 A 체크리스트로 대체

### 코드 품질 · 아키텍처 (REFACTOR는 별 PR — 이번은 검증만)
- [ ] 감정 키워드가 `Constants::SENTIMENT_KEYWORDS` **단일 소스** (Filters::S_KEYWORDS / initFilterKeywords() 미사용)
- [ ] 감정·카테고리 라벨 `u8"긍정"|u8"중립"|u8"부정"` **인라인 하드코딩 잔존 없음** (Constants 참조)
- [ ] `containsAny` TextAnalyzer·Filters **중복 없음** (KeywordUtils 통합 또는 Phase 3 예정 명시)
- [ ] H-1 / H-2 / H-3 — docs/defect_list.md 상태 **Fixed**
- [ ] `httplib.h` diff **0**
- [ ] HTTP 5엔드포인트 path/method·CSV BOM/`text\n` 형식 **변경 없음** (PRD §7.2)

### (Phase 2 이후 추가 확인 — Phase 1 Green만이면 Skip)
- [ ] main.cpp ≤ 200줄 (HtmlRenderer·RouteHandlers 분리 후)
- [ ] God Function 분해 완료 여부

---

## 4) 산출물

- ctest 전체 로그 (0 failures)
- `build/coverage_html/index.html` 또는 lcov 리포트
- `coverage_baseline_phase1.info` (docs/test_plan.md §7.4)
- docs/defect_list.md H-1~H-3 → Fixed 업데이트
- README §RED To-Do — 통과 TC 체크박스 [x] 반영

---

## 5) 이후 워크플로

green (Phase 1 전 TC Green)
  → feature/A_01 PR 생성 (예: `feature/phase1-green-p0-fixes`)
  → PR 본문: AC-1~AC-5 체크리스트, ctest 결과, 커버리지 스크린샷
  → 리뷰 (회귀: TC-B-02 중립 필터, CSV text, main 키워드)
  → 머지
  → Phase 2 REFACTOR 브랜치 분리 (HtmlRenderer, RouteHandlers — GREEN 유지)

UnitConverter 전용(3.28084, 1.09361, convert, Catch2, TC-A/B 07까지만) 항목 금지.
```

---

**치환 요약**

| UnitConverter | Feedback Analyzer |
|---------------|-------------------|
| TC-A/B 01~07 | TC-A-01~**08**, TC-B-01~**08** |
| gcov UnitConverter.cpp | TextAnalyzer.cpp, Filters.cpp, CsvParser.cpp |
| Domain ≥95%, Boundary ≥85% | 동일 (test_plan §6.2 stretch + §6.1 필수 90%) |
| 3.28084 인라인 금지 | Constants::SENTIMENT_KEYWORDS 단일 소스 |
| main() 변환 로직 분리 | God Function / H-1~H-3 해소 (Phase 2 분리는 별도) |
| green → A_01 PR | feature/phase1-green-p0-fixes → AC-1~AC-5 리뷰 |

---

**User**

현프로젝트에 맞게 아래 프롬포트를 수정해줘.

[P] 회귀 테스트(Approval/Golden Master) 설계 전문가입니다.
[C] C++17, CMake, Catch2
[T] UnitConverter 출력 기반 Golden Master 기준 파일을 생성해줘.

    1) 기준 출력(tests/golden_master_expected.txt) 생성/보관 전략
       입력 시나리오 4개:
         "meter:2.5" / "feet:1.0" / "yard:1.0" / "meter:0.0"
       캡처 방식: stdout 리디렉션: ./UnitConverter < input.txt > actual.txt
       생성 후: git add tests/golden_master_expected.txt  (버전 관리 필수)

    2) approve 패턴 적용:
       - 기준 파일 없으면 → 현재 출력을 기준으로 자동 생성
       - 기준 파일 있으면 → actual vs expected 문자열 비교
       - 불일치 시: diff 출력 후 테스트 FAIL

    3) 기준 파일 구조:
       [meter:2.5]
       2.5 meter = 8.202100 feet
       2.5 meter = 2.734025 yard
       ---
       [feet:1.0]
       ...

[F] tests/golden_master_expected.txt 파일 내용 + 생성 스크립트


---

**Cursor**

**Feedback Analyzer용 Golden Master 프롬프트 (P-C-T-F)**

```
[P] 회귀 테스트(Approval / Golden Master) 설계 전문가입니다.
    HTTP 웹앱 + Domain Service 이중 Golden Master 전략을 설계합니다.

[C] 워크스페이스: c:\DEV\FeedbackAnalyzer_12
    기술 스택: C++17, CMake 3.14+, Google Test, cpp-httplib
    근거: docs/test_plan.md §2·§4, docs/PRD.md §3.2·§6.2, README.md §RED To-Do (TC-A/B)
    ⚠️ Golden 생성 시 프로덕션 로직 변경 금지 — 캡처·기준 파일·approve 테스트만 추가

[T] Feedback Analyzer 출력 기반 Golden Master 기준 파일을 생성해줘.

---

## 1) 기준 출력 생성/보관 전략

### 1-A. Domain Golden (권장 — Phase 1, 안정적)

경로: `tests/golden/domain_golden_expected.txt`  
버전 관리: **필수** (`git add tests/golden/domain_golden_expected.txt`)

입력 시나리오 4개 (test_plan / PRD §2.2 기준):

| 시나리오 ID | Given | When | 캡처 대상 |
|-------------|-------|------|-----------|
| GM-D-01 | Feedback `"배송이 너무 늦어요. 화가 납니다."` | TextAnalyzer::sent + kw | 감정 map(긍/중/부), 키워드 map(배송+1…) |
| GM-D-02 | Feedback `"그냥 그래요. 특별한 감정 없음."` | Filters::fil(중립, 전체) | 필터 결과 text 목록 + 건수 |
| GM-D-03 | CSV `text\n택배가 빨라요.\n` | CsvParser::parse | Feedback text 목록 (1건) |
| GM-D-04 | Feedback 3건 혼합 | Filters::fil(전체, 전체) | 반환 건수=3, text 목록 |

캡처 방식:
- `tests/golden/capture_domain_golden.cpp` 또는 GTest `GoldenMasterCapture` 1회 실행
- stdout 또는 `tests/golden/_actual_domain.txt` 리디렉션
- **타임스탬프·랜덤값 제외** — 순수 Domain 직렬화만 (stable sort)

생성 후:
```powershell
git add tests/golden/domain_golden_expected.txt
git add tests/golden/capture_domain_golden.ps1   # 또는 .sh
```

### 1-B. HTTP Golden (선택 — Phase 2, Track A)

경로: `tests/golden/http_golden_expected.txt`  
서버 기동 후 curl/httplib 클라이언트로 응답 body 캡처

| 시나리오 ID | 요청 | 캡처 대상 |
|-------------|------|-----------|
| GM-H-01 | POST `/analyze` text=배송… | HTML stat 영역만 추출 (또는 정규화된 snippet) |
| GM-H-02 | POST `/filter` sentiment=중립&keyword=전체 | stat + alert (타임스탬프 **마스킹**) |
| GM-H-03 | POST `/upload` CSV(text 2행) | success alert 패턴 |
| GM-H-04 | GET `/download` (filter 후) | CSV 본문 (BOM `\xEF\xBB\xBF` + text\n + rows) |

캡처 방식:
```powershell
# scripts/capture_http_golden.ps1
# 1) build\feedback_analyzer.exe 백그라운드 기동
# 2) curl.exe -s -X POST http://localhost:8080/analyze -d "text=..." > tests/golden/_actual_http.txt
# 3) 타임스탬프 정규화: YYYY-MM-DD HH:MM:SS → {{TIMESTAMP}}
```

---

## 2) Approve 패턴 적용 (Google Test)

Catch2 사용 금지 → Google Test + 헬퍼 `tests/support/GoldenMaster.h`

| 조건 | 동작 |
|------|------|
| 기준 파일 없음 | 현재 출력을 `*_expected.txt`로 **자동 생성** + `GTEST_SKIP()` 또는 INFO 로그 |
| 기준 파일 있음 | `actual` vs `expected` **전체 문자열 비교** (또는 line-by-line) |
| 불일치 | diff 출력 (`std::cerr` 또는 실패 메시지에 unified diff) → `EXPECT_EQ(actual, expected)` **FAIL** |
| UPDATE 모드 | env `GOLDEN_UPDATE=1` 시 기준 파일 덮어쓰기 (CI에서는 비활성) |

테스트 파일: `tests/GoldenMasterTest.cpp`

---

## 3) 기준 파일 구조 (domain_golden_expected.txt 예시)

```
[GM-D-01: analyze negative delivery]
input_text: 배송이 너무 늦어요. 화가 납니다.
sentiment: 긍정=0, 중립=0, 부정=1
keyword: 배송=1, 품질=0, 가격=0, 서비스=0, 사용성=0
---
[GM-D-02: filter neutral]
input_text: 그냥 그래요. 특별한 감정 없음.
filter: sentiment=중립, keyword=전체
result_count: 1
result_texts:
  - 그냥 그래요. 특별한 감정 없음.
---
[GM-D-03: csv parse]
input_csv: |
  text
  택배가 빨라요.
parse_count: 1
parse_texts:
  - 택배가 빨라요.
---
[GM-D-04: filter all]
input_count: 3
filter: sentiment=전체, keyword=전체
result_count: 3
---
```

HTTP golden (선택) 구조:
```
[GM-H-04: download csv]
content_type: text/csv; charset=UTF-8
body_hex_prefix: EFBBBF   # UTF-8 BOM
body:
text
배송이 너무 늦어요. 화가 납니다.
---
```

---

## 4) 확인 항목 (Golden Master PR)

- [ ] 4개 Domain 시나리오 expected 파일 커밋됨
- [ ] `GOLDEN_UPDATE=0` CI에서 approve 테스트 PASS
- [ ] H-1/H-2/H-3 수정 후에도 GM-D-02/D-03 기준 **의도적 갱신**만 (silent drift 없음)
- [ ] HTML golden 사용 시 타임스탬프 마스킹 적용
- [ ] httplib.h / Feedback.h 변경 없음

[F] 산출물:
1. `tests/golden/domain_golden_expected.txt` 전체 내용
2. `tests/support/GoldenMaster.h` (approve 헬퍼)
3. `tests/GoldenMasterTest.cpp` (4 시나리오)
4. `tests/golden/capture_domain_golden.ps1` (또는 scripts/capture_golden.ps1)
5. CMakeLists.txt — GoldenMasterTest 타깃 추가만

UnitConverter 전용(meter:2.5, 3.28084, UnitConverter, Catch2, stdout ./UnitConverter) 금지.
```

---

**UnitConverter → Feedback Analyzer 치환**

| UnitConverter | Feedback Analyzer |
|---------------|-------------------|
| Catch2 | **Google Test** |
| meter:2.5 / feet / yard | analyze·filter·CsvParser·download 4시나리오 |
| `./UnitConverter < input.txt` | Domain 직렬화 또는 **curl + localhost:8080** |
| convert 출력 한 줄 | sentiment/keyword map, filter 결과, CSV(BOM) |
| 3.28084 인라인 검증 | Constants 단일 소스·타임스탬프 마스킹 |
| golden_master_expected.txt | `tests/golden/domain_golden_expected.txt` (+ http 선택) |

---

**User**

현프로젝트에 맞게 아래 프롬포트 수정해줘.

[P] 회귀 테스트(Approval/Golden Master) 설계 전문가입니다.
[C] C++17, CMake, Catch2
[T] UnitConverter Golden Master 테스트 코드를 작성해줘.

    구현 요건:
    1) 출력 캡처: stdout 리디렉션: ./UnitConverter < input.txt > actual.txt
    2) 파일 비교: Google Test: EXPECT_EQ(expected_str, actual_str)        std::ifstream으로 파일 읽어 문자열 비교
    3) 테스트 마킹: TEST_F(GoldenMasterTest, UnitConverter_meter_2_5)
    4) 빌드/실행: CMake: add_test(NAME GoldenMaster COMMAND UnitConverter_test) ctest --test-dir build -R GoldenMaster
    5) (선택) ApprovalTests.cpp 라이브러리 적용 가능

    테스트 케이스 (4개):
    - GM-TC-01: "meter:2.5" 출력 == 기준 파일 [meter:2.5] 섹션
    - GM-TC-02: "feet:1.0"  출력 == 기준 파일 [feet:1.0]  섹션
    - GM-TC-03: "yard:1.0"  출력 == 기준 파일 [yard:1.0]  섹션
    - GM-TC-04: "meter:0.0" 출력 == 기준 파일 [meter:0.0] 섹션

    실패 시 출력:
    --- expected
    +++ actual
    @@ 라인별 diff 표시 @@

[F] test_golden_master.cpp + golden_master_expected.txt 완성 코드 + CMake: add_test(NAME GoldenMaster COMMAND UnitConverter_test) 실행 결과


---

**Cursor**

**Feedback Analyzer용 Golden Master 테스트 작성 프롬프트 (P-C-T-F)**

```
[P] 회귀 테스트(Approval/Golden Master) 설계 전문가입니다.
    Domain Service 출력 직렬화 기반 Golden Master 테스트를 구현합니다.

[C] 워크스페이스: c:\DEV\FeedbackAnalyzer_12
    기술 스택: C++17, CMake 3.14+, Google Test (Catch2 사용 금지)
    근거: docs/test_plan.md §2·§4, docs/PRD.md §3.2, tests/golden/domain_golden_expected.txt
    기준 파일: tests/golden/domain_golden_expected.txt (섹션별 [GM-D-xx] 블록)
    ⚠️ src/cpp/httplib.h 수정 금지 · Golden/테스트 코드만 추가

[T] Feedback Analyzer Golden Master 테스트 코드를 작성해줘.

    구현 요건:
    1) 출력 캡처 (Domain — stdout 리디렉션 대신 Service 직렬화):
       · TextAnalyzer::sent / Filters::fil / CsvParser::parse 결과를
         canonical 문자열로 직렬화 (타임스탬프·HTML 제외)
       · tests/support/GoldenMaster.h — serializeSentimentMap(), serializeFilterResult() 등
       · (선택 HTTP) scripts/capture_http_golden.ps1 — curl → actual 파일 (Phase 2)

    2) 파일 비교:
       · std::ifstream으로 tests/golden/domain_golden_expected.txt 읽기
       · 섹션 파서: [GM-D-01: ...] ~ --- 구간 추출
       · Google Test: EXPECT_EQ(expected_section, actual_str)
       · 불일치 시 unified diff 출력:
         --- expected
         +++ actual
         @@ 라인별 diff @@

    3) 테스트 마킹 (Google Test):
       TEST_F(GoldenMasterTest, GM_D01_AnalyzeNegativeDelivery_MatchesGolden)
       TEST_F(GoldenMasterTest, GM_D02_FilterNeutral_MatchesGolden)
       TEST_F(GoldenMasterTest, GM_D03_CsvParseTextColumn_MatchesGolden)
       TEST_F(GoldenMasterTest, GM_D04_FilterAll_MatchesGolden)

    4) 빌드/실행 (CMake):
       · feedback_analyzer_tests에 GoldenMasterTest.cpp 추가
       · gtest_discover_tests 또는 add_test:
         add_test(NAME GoldenMaster COMMAND feedback_analyzer_tests --gtest_filter=GoldenMasterTest.*)
       · 실행:
         cmake -S . -B build && cmake --build build
         ctest --test-dir build -R GoldenMaster -V --output-on-failure

    5) (선택) ApprovalTests / 자체 GoldenMaster.h
       · env GOLDEN_UPDATE=1 → expected 파일 해당 섹션 갱신 (로컬만, CI 금지)

    테스트 케이스 (4개):
    - GM-TC-01 (GM-D-01): Feedback "배송이 너무 늦어요. 화가 납니다."
      → analyze sentiment/keyword map == 기준 [GM-D-01] 섹션 (T-02, TC-A-01)
    - GM-TC-02 (GM-D-02): Feedback "그냥 그래요. 특별한 감정 없음."
      → filter sentiment=중립, keyword=전체 == [GM-D-02] (T-03, AC-1, TC-B-02)
    - GM-TC-03 (GM-D-03): CSV `text\n택배가 빨라요.\n`
      → CsvParser parse 결과 == [GM-D-03] (T-06 연계, TC-B-05)
    - GM-TC-04 (GM-D-04): Feedback 3건 혼합
      → filter sentiment=전체, keyword=전체, result_count=3 == [GM-D-04] (T-04, TC-B-03)

    GoldenMasterTest Fixture:
    · SetUp(): Constants::init(), Filters::initFilterKeywords() (또는 Green 후 init 제거 상태 반영)
    · 피드백 목록·CSV 문자열은 test_plan canonical 값 사용

    실패 시 출력:
    --- expected (GM-D-02)
    +++ actual
    @@ -1,3 +1,3 @@
     result_count: 1
    -result_texts: ...

[F] 산출물:
1. tests/GoldenMasterTest.cpp (4 GM-TC)
2. tests/support/GoldenMaster.h (섹션 load / compare / diff / optional update)
3. tests/golden/domain_golden_expected.txt (4 섹션 전체)
4. CMakeLists.txt — GoldenMasterTest 타깃·add_test 추가 diff
5. ctest --test-dir build -R GoldenMaster 실행 결과 요약 (4/4 PASS 기대)

금지:
- Catch2, meter:2.5, feet, yard, 3.28084, UnitConverter, ApprovalTests 외부 의존(미적용 시)
- ./UnitConverter < input.txt 패턴
- HTML 전체 body golden (타임스탬프 불안정 — stat 직렬화만)
```

---

**기준 파일 섹션 예시** (`domain_golden_expected.txt`)

```
[GM-D-01: analyze negative delivery]
input_text: 배송이 너무 늦어요. 화가 납니다.
sentiment: 긍정=0, 중립=0, 부정=1
keyword: 배송=1, 품질=0, 가격=0, 서비스=0, 사용성=0
---
[GM-D-02: filter neutral]
input_text: 그냥 그래요. 특별한 감정 없음.
filter: sentiment=중립, keyword=전체
result_count: 1
result_texts:
  - 그냥 그래요. 특별한 감정 없음.
---
```

---

**UnitConverter → Feedback Analyzer 치환**

| UnitConverter | Feedback Analyzer |
|---------------|-------------------|
| Catch2 `[C]` | **Google Test only** |
| `./UnitConverter < input.txt` | TextAnalyzer/Filters/CsvParser 직렬화 |
| GM-TC-01 meter:2.5 | GM-D-01 analyze 부정+배송 |
| GM-TC-02 feet:1.0 | GM-D-02 filter 중립 |
| GM-TC-03 yard:1.0 | GM-D-03 CSV text 파싱 |
| GM-TC-04 meter:0.0 | GM-D-04 filter 전체×전체 |
| test_golden_master.cpp | tests/GoldenMasterTest.cpp |
| UnitConverter_test | feedback_analyzer_tests |

