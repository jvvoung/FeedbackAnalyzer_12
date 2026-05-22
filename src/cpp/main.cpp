#include "httplib.h"

#include "Constants.h"
#include "RouteHandlers.h"
#include "Logger.h"

int main() {
    Constants::init();

    httplib::Server server;
    RouteHandlers routeHandlers;
    routeHandlers.registerRoutes(server);

    Logger::logInfo(u8"서버가 http://localhost:8080 에서 시작됩니다.");
    server.listen("0.0.0.0", 8080);

    return 0;
}
