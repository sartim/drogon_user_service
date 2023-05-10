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

void dropTables() {

}

void runServer() {
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
}


int main(int argc, char* argv[]) {
    // Check if the correct number of command-line arguments is provided
    if (argc != 2) {
        std::cerr << "Usage: --action=run-server|create-tables|drop-tables" << std::endl;
        return 1;
    }

    // Parse the command-line argument
    std::string action = argv[1];

    // Extract the action from the argument
    size_t equalsPos = action.find('=');
    if (equalsPos == std::string::npos || equalsPos == action.length() - 1) {
        std::cerr << "Invalid argument format" << std::endl;
        return 1;
    }

    std::string key = action.substr(0, equalsPos);
    std::string value = action.substr(equalsPos + 1);

    // Check the action and perform the corresponding operation
    if (key == "--action") {
        if (value == "run-server") {
          runServer();
        } else if (value == "create-tables") {
          createTables();
        } else if (value == "drop-tables") {
          dropTables();
        } else {
          std::cerr << "Invalid action" << std::endl;
          return 1;
        }
    } else {
        std::cerr << "Invalid argument" << std::endl;
        return 1;
    }

    return 0;
}