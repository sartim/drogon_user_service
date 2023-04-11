#include <drogon/drogon.h>
#include <controllers/UserController.h>


int main() {
    drogon::app().setLogLevel(trantor::Logger::kTrace);
    //Set HTTP listener address and port
    drogon::app().addListener("0.0.0.0",8000);
    //Load config file
    drogon::app().loadConfigFile("../config.json");
    //Run HTTP framework,the method will block in the internal event loop
    drogon::app().registerHandler(
        "/hello", 
        [](
            const drogon::HttpRequestPtr& req, std::function<void (
                const drogon::HttpResponsePtr&
                )>&& callback
          ) {
        drogon::HttpResponsePtr resp = drogon::HttpResponse::newHttpResponse();
        resp->setBody("Hello, world!");
        callback(resp);
    });
    auto userController = std::make_shared<UserController>();
    drogon::app().registerHandler(
        "/user", [userController](
            const HttpRequestPtr& req, std::function<void(
                const HttpResponsePtr&
                )>&& callback) {
        userController->getUsers(req, std::move(callback));
    }, {Get});


    drogon::app().run();
    return 0;
}
