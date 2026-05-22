#pragma once

#include <vector>

#include "AppState.h"
#include "Feedback.h"

class FeedbackSession {
public:
    static std::vector<Feedback>& getCurrent() {
        return AppState::currentFeedbacks();
    }

    static const std::vector<Feedback>& getLastFiltered() {
        return AppState::lastFilteredFeedbacksConst();
    }

    static void update(const std::vector<Feedback>& feedbacks) {
        AppState::currentFeedbacks() = feedbacks;
    }

    static void setLastFiltered(const std::vector<Feedback>& filtered) {
        AppState::setLastFilteredFeedbacks(filtered);
    }

    static void clear() {
        AppState::currentFeedbacks().clear();
        AppState::lastFilteredFeedbacks().clear();
    }
};
