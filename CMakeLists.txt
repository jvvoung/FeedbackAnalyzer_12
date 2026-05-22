cmake_minimum_required(VERSION 3.14)
project(FeedbackAnalyzer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FA_SRC_ROOT ${CMAKE_SOURCE_DIR}/src/cpp)
set(FA_INCLUDE_DIRS
    ${FA_SRC_ROOT}
    ${FA_SRC_ROOT}/app
    ${FA_SRC_ROOT}/app/http
    ${FA_SRC_ROOT}/app/ui
    ${FA_SRC_ROOT}/app/state
    ${FA_SRC_ROOT}/app/usecase
    ${FA_SRC_ROOT}/csv
    ${FA_SRC_ROOT}/feedback
)

add_executable(feedback_analyzer
    src/cpp/app/main.cpp
    src/cpp/feedback/Constants.cpp
    src/cpp/feedback/KeywordRegistry.cpp
    src/cpp/app/usecase/AnalyzeUseCase.cpp
    src/cpp/app/usecase/FilterUseCase.cpp
    src/cpp/app/state/AppState.cpp
    src/cpp/app/http/FormParser.cpp
    src/cpp/csv/CsvParser.cpp
    src/cpp/app/ui/AppMessages.cpp
    src/cpp/app/ui/HtmlPageRenderer.cpp
    src/cpp/csv/CsvExporter.cpp
    src/cpp/app/http/RouteHandlers.cpp
    src/cpp/app/state/Logger.cpp
    src/cpp/app/state/FeedbackSession.cpp
)

target_include_directories(feedback_analyzer PRIVATE ${FA_INCLUDE_DIRS})

# Windows: httplib mmap uses CreateFile2 / CreateFileMappingFromApp (Win8+).
# MinGW defaults to older WINVER unless raised here.
if(WIN32)
    target_compile_definitions(feedback_analyzer PRIVATE
        _WIN32_WINNT=0x0A00
        WINVER=0x0A00
    )
    target_link_libraries(feedback_analyzer PRIVATE ws2_32)
endif()

# --- Google Test (Phase 1 RED) ---
# Expected RED failures (ctest not fully Green until GREEN phase):
#   T-05/H-2: CsvParserTest.Given_NoTextColumnCsv_When_Parse_Then_ZeroFeedbacks
#   T-03/H-1: FiltersTest.Given_MixedFeedbacksIncludingNeutral_When_FilterSentimentNeutral_Then_MatchesTextAnalyzerSet
#   T-06/H-3: FiltersTest.Given_QualityMainKeywordOnlyText_When_FilterKeywordQuality_Then_IncludedInResult
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(feedback_analyzer_tests
    tests/SentimentClassifierTest.cpp
    tests/TextAnalyzerTest.cpp
    tests/FiltersTest.cpp
    tests/CsvParserTest.cpp
    tests/FeedbackTest.cpp
    tests/KeywordUtilsTest.cpp
    tests/GoldenMasterTest.cpp
    tests/FileHandlerTest.cpp
    tests/BoundaryUseCaseTest.cpp
    tests/RouteHandlersBoundaryTest.cpp
    src/cpp/csv/CsvParser.cpp
    src/cpp/feedback/Constants.cpp
    src/cpp/feedback/KeywordRegistry.cpp
    src/cpp/csv/CsvExporter.cpp
    src/cpp/csv/FileHandler.cpp
    src/cpp/app/usecase/AnalyzeUseCase.cpp
    src/cpp/app/usecase/FilterUseCase.cpp
    src/cpp/app/state/AppState.cpp
    src/cpp/app/state/FeedbackSession.cpp
    src/cpp/app/ui/AppMessages.cpp
    src/cpp/app/http/RouteHandlers.cpp
    src/cpp/app/ui/HtmlPageRenderer.cpp
    src/cpp/app/http/FormParser.cpp
    src/cpp/app/state/Logger.cpp
)

target_include_directories(feedback_analyzer_tests PRIVATE
    ${FA_INCLUDE_DIRS}
    tests
    tests/support
)

target_compile_definitions(feedback_analyzer_tests PRIVATE
    GOLDEN_DIR="${CMAKE_SOURCE_DIR}/tests/golden"
)

target_link_libraries(feedback_analyzer_tests PRIVATE GTest::gtest_main)

if(WIN32)
    target_compile_definitions(feedback_analyzer_tests PRIVATE
        _WIN32_WINNT=0x0A00
        WINVER=0x0A00
    )
    target_link_libraries(feedback_analyzer_tests PRIVATE ws2_32)
endif()

include(GoogleTest)
gtest_discover_tests(feedback_analyzer_tests)

add_test(NAME GoldenMaster
    COMMAND feedback_analyzer_tests --gtest_filter=GoldenMasterTest.*
)
set_tests_properties(GoldenMaster PROPERTIES
    LABELS "golden;domain"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
