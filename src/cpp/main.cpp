#include "httplib.h"

#include "Constants.h"
#include "RouteHandlers.h"
#include "Logger.h"
#include "ServerConfig.h"

int main() {
    Constants::init();

    httplib::Server server;
    RouteHandlers routeHandlers;
    routeHandlers.registerRoutes(server);

    Logger::logInfo(u8"서버가 " + ServerConfig::publicUrl() + u8" 에서 시작됩니다.");
    server.listen(ServerConfig::kBindHost, ServerConfig::kPort);

    return 0;
}
