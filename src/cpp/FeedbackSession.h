#pragma once

#include <vector>

#include "AppState.h"
#include "Feedback.h"

class FeedbackSession {
public:
    static std::vector<Feedback>& getCurrent() {
        return AppState::currentFeedbacks();
    }

    static void update(const std::vector<Feedback>& feedbacks) {
        AppState::currentFeedbacks() = feedbacks;
    }

    static void clear() {
        AppState::currentFeedbacks().clear();
    }
};
