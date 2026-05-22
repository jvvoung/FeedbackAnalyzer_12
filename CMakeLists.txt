cmake_minimum_required(VERSION 3.14)
project(FeedbackAnalyzer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(feedback_analyzer
    src/cpp/main.cpp
    src/cpp/Constants.cpp
    src/cpp/Logger.cpp
    src/cpp/Session.cpp
    src/cpp/TextAnalyzer.cpp
    src/cpp/UIComponents.cpp
)

target_include_directories(feedback_analyzer PRIVATE src/cpp)

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
    tests/TextAnalyzerTest.cpp
    tests/FiltersTest.cpp
    tests/CsvParserTest.cpp
    tests/FeedbackTest.cpp
    tests/KeywordUtilsTest.cpp
    tests/GoldenMasterTest.cpp
    tests/support/CsvParser.cpp
    src/cpp/Constants.cpp
    src/cpp/TextAnalyzer.cpp
)

target_include_directories(feedback_analyzer_tests PRIVATE
    src/cpp
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
endif()

include(GoogleTest)
gtest_discover_tests(feedback_analyzer_tests)
