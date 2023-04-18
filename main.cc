#include <drogon/drogon.h>
#include <drogon/HttpAppFramework.h>
#include "controllers/UserController.h"
#include "tables/UserTable.h"


void createTables() {
    // User table
    UserTable userTable;
    userTable.create();
}

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
        // Create tables
        createTables();
        
        // Return response
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
            if (req->method() == Options) {
                userController->getHeaders(req, std::move(callback));
            } else if (req->method() == Get) {
                userController->getUsers(req, std::move(callback));
            } else if (req->method() == Post) {
                userController->createUser(req, std::move(callback));
            }
        }, 
        {Options, Get, Post, "AuthFilter"}
    );
    drogon::app().registerHandler(
    "/api/v1/user/{id}", 
    [userController](
        const HttpRequestPtr& req, 
        std::function<void(const HttpResponsePtr&)>&& callback, std::string id) {
            if (req->method() == Options) {
                userController->getByIdHeaders(req, std::move(callback), id);
            } else if (req->method() == Get) {
                userController->getUserById(req, std::move(callback), id);
            } else if (req->method() == Put) {
                userController->updateUserById(req, std::move(callback), id);;
            } else if (req->method() == Delete) {
                userController->deleteUserById(req, std::move(callback), id);;
            }
    }, 
    {Options, Get, Put, Delete, "AuthFilter"}
    );

}


int main() {

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
