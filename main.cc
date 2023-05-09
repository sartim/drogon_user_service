#include "controllers/UserController.h"
#include "controllers/RoleController.h"
#include "tables/PermissionTable.h"
#include "tables/RolePermissionTable.h"
#include "tables/RoleTable.h"
#include "tables/UserPermissionTable.h"
#include "tables/UserTable.h"
#include <drogon/HttpAppFramework.h>

using namespace std;
using namespace drogon;

void createTables()
{
    // User table
    UserTable userTable;
    userTable.create();

    // Role  table
    RoleTable roleTable;
    roleTable.create();

    // Permission  table
    PermissionTable permissionTable;
    permissionTable.create();

    // User permission  table
    UserPermissionTable userPermissionTable;
    userPermissionTable.create();

    // Role permission  table
    RolePermissionTable rolePermissionTable;
    rolePermissionTable.create();
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

    // Register role routes
    auto roleController = make_shared<RoleController>();
    app().registerHandler(
        "/api/v1/role",
        [roleController](
            const HttpRequestPtr& req,
            function<void(const HttpResponsePtr&)>&& callback) {
          if (req->method() == Options) {
            roleController->getHeaders(req, move(callback));
          } else if (req->method() == Get) {
            roleController->getRoles(req, move(callback));
          } else if (req->method() == Post) {
            roleController->createRole(req, move(callback));
          }
        },
        {Options, Get, Post, "AuthFilter"}
    );
    app().registerHandler(
        "/api/v1/role/{id}",
        [roleController](
            const HttpRequestPtr& req,
            function<void(const HttpResponsePtr&)>&& callback, const string& id) {
          if (req->method() == Options) {
            roleController->getByIdHeaders(req, move(callback), id);
          } else if (req->method() == Get) {
            roleController->getRoleById(req, move(callback), id);
          } else if (req->method() == Put) {
            roleController->updateRoleById(req, move(callback), id);;
          } else if (req->method() == Delete) {
            roleController->deleteRoleById(req, move(callback), id);;
          }
        },
        {Options, Get, Put, Delete, "AuthFilter"}
    );
}


int main() {

    // Set log level
    app().setLogLevel(trantor::Logger::kTrace);
    int32_t port = 8000;
    // Set HTTP listener address and port
    app().addListener("0.0.0.0", port);

    // Load config file
    try {
        app().loadConfigFile("../config.json");
    } catch (const exception& e) {
        cerr << "Exception caught: "
             << typeid(e).name() << " - " << e.what() << endl;
    }

    // Register routes
    registerRoutes();

    // Run server
    LOG_INFO << "Server running on 127.0.0.1:" << port;
    app().run();

    return 0;
}
