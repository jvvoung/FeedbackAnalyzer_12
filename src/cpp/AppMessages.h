#pragma once

#include <cstddef>
#include <string>

class AppMessages {
public:
    static const char* analyzerStart();
    static const char* processingError();
    static const char* uploadError();
    static const char* filterNoResults();
    static const char* noFeedbacksToAnalyze();
    static const char* noDownloadResults();

    static std::string feedbackCountSuccess(std::size_t count);
};
