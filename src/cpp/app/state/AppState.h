#pragma once

#include <vector>

#include "Feedback.h"

class AppState {
public:
    static std::vector<Feedback>& currentFeedbacks();
    static const std::vector<Feedback>& currentFeedbacksConst();

    static std::vector<Feedback>& lastFilteredFeedbacks();
    static const std::vector<Feedback>& lastFilteredFeedbacksConst();

    static void setLastFilteredFeedbacks(const std::vector<Feedback>& filtered);

private:
    static std::vector<Feedback> currentFeedbacks_;
    static std::vector<Feedback> lastFilteredFeedbacks_;
};
