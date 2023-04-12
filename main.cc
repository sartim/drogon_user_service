#include <drogon/drogon.h>
#include <controllers/UserController.h>


int main() {
    // Set log level
    drogon::app().setLogLevel(trantor::Logger::kTrace);

    //Set HTTP listener address and port
    drogon::app().addListener("0.0.0.0",8000);

    //Load config file
    drogon::app().loadConfigFile("../config.json");

    // Register root route
    drogon::app().registerHandler(
        "/", 
        [](
            const drogon::HttpRequestPtr& req, std::function<void (
                const drogon::HttpResponsePtr&
                )>&& callback
          ) {
        drogon::HttpResponsePtr resp = drogon::HttpResponse::newHttpResponse();
        Json::Value response;
            response["status"] = "up";
        resp->setBody("Welcome to drogon user service");
        callback(resp);
    });

    // Register user route
    auto userController = std::make_shared<UserController>();
    drogon::app().registerHandler(
        "/api/v1/user", [userController](
            const HttpRequestPtr& req, std::function<void(
                const HttpResponsePtr&
                )>&& callback) {
        userController->getUsers(req, std::move(callback));
    }, {Get});

    // Run server
    drogon::app().run();

    return 0;
}
