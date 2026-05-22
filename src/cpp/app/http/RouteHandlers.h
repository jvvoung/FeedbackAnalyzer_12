#pragma once

#include "httplib.h"

#include "Filters.h"
#include "TextAnalyzer.h"

class RouteHandlers {
public:
    void registerRoutes(httplib::Server& server);

private:
    TextAnalyzer textAnalyzer_;
    Filters filters_;

    void handleGetRoot(const httplib::Request& req, httplib::Response& res);
    void handlePostAnalyze(const httplib::Request& req, httplib::Response& res);
    void handlePostUpload(const httplib::Request& req, httplib::Response& res);
    void handlePostFilter(const httplib::Request& req, httplib::Response& res);
    void handleGetDownload(const httplib::Request& req, httplib::Response& res);
};
