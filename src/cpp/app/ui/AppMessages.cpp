#include "AppMessages.h"

const char* AppMessages::analyzerStart() {
    return u8"피드백 분석기 시작";
}

const char* AppMessages::processingError() {
    return u8"처리 중 오류가 발생했습니다.";
}

const char* AppMessages::uploadError() {
    return u8"파일 업로드 중 오류가 발생했습니다.";
}

const char* AppMessages::filterNoResults() {
    return u8"필터링 결과가 없습니다.";
}

const char* AppMessages::noFeedbacksToAnalyze() {
    return u8"분석할 피드백이 없습니다.";
}

const char* AppMessages::noDownloadResults() {
    return u8"다운로드할 필터 결과가 없습니다.";
}

std::string AppMessages::feedbackCountSuccess(std::size_t count) {
    return std::to_string(count) + u8"개의 피드백이 입력되었습니다.";
}
