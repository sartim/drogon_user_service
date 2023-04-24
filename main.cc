#include <drogon/HttpAppFramework.h>
#include "controllers/UserController.h"
#include "tables/UserTable.h"

using namespace std;
using namespace drogon;

void createTables()
{
    // User table
    UserTable userTable;
    userTable.create();
}

void registerRoutes()
{
    // Register root route
    app().registerHandler(
        "/", 
        [](
            const HttpRequestPtr& req, function<void (
                const HttpResponsePtr&
                )>&& callback
          ) {
        // Create tables
        createTables();
        
        // Return response
        Json::Value response;
        response["status"] = "up";
        HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(response);
        callback(resp);
    });

    // Register user routes
    auto userController = make_shared<UserController>();
    app().registerHandler(
        "/api/v1/user", 
        [userController](
        const HttpRequestPtr& req, 
        function<void(const HttpResponsePtr&)>&& callback) {
            if (req->method() == Options) {
                userController->getHeaders(req, move(callback));
            } else if (req->method() == Get) {
                userController->getUsers(req, move(callback));
            } else if (req->method() == Post) {
                userController->createUser(req, move(callback));
            }
        }, 
        {Options, Get, Post, "AuthFilter"}
    );
    app().registerHandler(
    "/api/v1/user/{id}", 
    [userController](
        const HttpRequestPtr& req, 
        function<void(const HttpResponsePtr&)>&& callback, const string& id) {
            if (req->method() == Options) {
                userController->getByIdHeaders(req, move(callback), id);
            } else if (req->method() == Get) {
                userController->getUserById(req, move(callback), id);
            } else if (req->method() == Put) {
                userController->updateUserById(req, move(callback), id);;
            } else if (req->method() == Delete) {
                userController->deleteUserById(req, move(callback), id);;
            }
    }, 
    {Options, Get, Put, Delete, "AuthFilter"}
    );

}


int main() {

    // Set log level
    app().setLogLevel(trantor::Logger::kTrace);
    int32_t port = 8000;
    //Set HTTP listener address and port
    app().addListener("0.0.0.0", port);

    //Load config file
    app().loadConfigFile("../config.json");

    // Register routes
    registerRoutes();

    // Run server
    LOG_INFO << "Server running on 127.0.0.1:" << port;
    app().run();

    return 0;
}
