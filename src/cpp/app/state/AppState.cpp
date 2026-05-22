#include "AppState.h"

std::vector<Feedback> AppState::currentFeedbacks_;
std::vector<Feedback> AppState::lastFilteredFeedbacks_;

std::vector<Feedback>& AppState::currentFeedbacks() {
    return currentFeedbacks_;
}

const std::vector<Feedback>& AppState::currentFeedbacksConst() {
    return currentFeedbacks_;
}

std::vector<Feedback>& AppState::lastFilteredFeedbacks() {
    return lastFilteredFeedbacks_;
}

const std::vector<Feedback>& AppState::lastFilteredFeedbacksConst() {
    return lastFilteredFeedbacks_;
}

void AppState::setLastFilteredFeedbacks(const std::vector<Feedback>& filtered) {
    lastFilteredFeedbacks_ = filtered;
}
