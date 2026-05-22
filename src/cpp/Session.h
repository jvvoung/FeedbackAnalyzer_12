#pragma once
#include <vector>
#include <string>
#include "Feedback.h"
#include "AppState.h"

class Session {
public:
    static void initSessionStateUgly() {
        // already initialized as static
    }

    static std::vector<Feedback>& getOldDataFromSession(const std::string& /*key*/) {
        return AppState::currentFeedbacks();
    }

    static void updateCurrentFeedbacks(const std::vector<Feedback>& feedbacks) {
        AppState::currentFeedbacks() = feedbacks;
    }

    static std::vector<Feedback>& getCurrentFeedbacks() {
        return AppState::currentFeedbacks();
    }
};
