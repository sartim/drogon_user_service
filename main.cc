#include <drogon/drogon.h>
#include <drogon/HttpAppFramework.h>
#include "controllers/UserController.h"
#include "tables/UserTable.h"

void registerRoutes()
{
    // Register root route
    drogon::app().registerHandler(
        "/", 
        [](
            const drogon::HttpRequestPtr& req, std::function<void (
                const drogon::HttpResponsePtr&
                )>&& callback
          ) {
        Json::Value response;
        response["status"] = "up";
        drogon::HttpResponsePtr resp = drogon::HttpResponse::newHttpJsonResponse(response);
        callback(resp);
    });

    // Register user routes
    auto userController = std::make_shared<UserController>();
    drogon::app().registerHandler(
        "/api/v1/user", 
        [userController](
            const HttpRequestPtr& req, 
            std::function<void(const HttpResponsePtr&)>&& callback) {
                userController->getUsers(req, std::move(callback));
        }, 
        {Get, Post, "AuthFilter"}
    );
    drogon::app().registerHandler(
    "/api/v1/user/{id}", 
    [userController](
        const HttpRequestPtr& req, 
        std::function<void(const HttpResponsePtr&)>&& callback, std::string id) {
            userController->getUserById(req, std::move(callback), id);
    }, 
    {Get, "AuthFilter"}
    );

}

void createTables() {
    // User table
    auto userTable = std::make_shared<UserTable>();
    auto client = drogon::app().getDbClient();
    userTable->create(client);
}


int main() {
    //CreateTables
    createTables();

    // Set log level
    drogon::app().setLogLevel(trantor::Logger::kTrace);
    std::int32_t port = 8000;
    //Set HTTP listener address and port
    drogon::app().addListener("0.0.0.0", port);

    //Load config file
    drogon::app().loadConfigFile("../config.json");

    // Register routes
    registerRoutes();

    // Run server
    LOG_INFO << "Server running on 127.0.0.1:" << port;
    drogon::app().run();

    return 0;
}
