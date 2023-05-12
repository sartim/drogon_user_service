#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include "controllers/UserController.h"
#include "controllers/RoleController.h"
#include "tables/PermissionTable.h"
#include "tables/RolePermissionTable.h"
#include "tables/RoleTable.h"
#include "tables/UserPermissionTable.h"
#include "tables/UserTable.h"
#include <drogon/HttpAppFramework.h>
#include "models/Users.h"
#include "helpers/BCrypt.h"

using namespace std;
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::drogon_user_service;

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

map<string, string> loadEnvVariables(const string& filename) {
    map<string, string> envVariables;

    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Failed to open .env file.");
    }

    string line;
    while (getline(file, line)) {
        stringstream lineStream(line);
        string key, value;
        if (getline(lineStream, key, '=') && getline(lineStream, value)) {
          envVariables[key] = value;
        }
    }

    file.close();

    return envVariables;
}

void generateConfigFile() {
    // Load environment variables from .env file
    map<string, string> envVariables = loadEnvVariables(".env");
    
    // Create a JSON object
    Json::Value config;
    // Set the values for the variables from environment variables
    string secretKey = envVariables["SECRET_KEY"];
    string dbName = envVariables["DB_NAME"];
    string user = envVariables["DB_USER"];
    string password = envVariables["DB_PASSWORD"];

    config["secret_key"] = secretKey;
    config["db_clients"] = Json::Value(Json::arrayValue);
    Json::Value& dbClients = config["db_clients"];

    // Create a database client object
    Json::Value dbClient;
    dbClient["name"] = "default";
    dbClient["rdbms"] = "postgresql";
    dbClient["host"] = "127.0.0.1";
    dbClient["port"] = 5432;
    dbClient["dbname"] = dbName;
    dbClient["user"] = user;
    dbClient["passwd"] = password;
    dbClient["is_fast"] = false;
    dbClient["connection_number"] = 1;
    dbClient["filename"] = "";

    // Add the database client object to the array
    dbClients.append(dbClient);

    // Generate the JSON string
    Json::StreamWriterBuilder writer;
    writer["indentation"] = "    "; // 4 spaces for indentation
    string jsonString = Json::writeString(writer, config);

    // Write the JSON string to a file
    ofstream outputFile("config.json");
    outputFile << jsonString;
    outputFile.close();

    cout << "Configuration file generated successfully." << endl;
}

int main(int argc, char* argv[]) {

    // Generate config file from .env
    generateConfigFile();

    // Check if the correct number of command-line arguments is provided
    if (argc != 2) {
        cerr <<
            "Usage: "
            "--action=run-server|create-tables|drop-tables"
             << endl;
        return 1;
    }

    // Parse the command-line argument
    string action = argv[1];

    // Extract the action from the argument
    size_t equalsPos = action.find('=');
    if (equalsPos == string::npos || equalsPos == action.length() - 1) {
        cerr << "Invalid argument format" << endl;
        return 1;
    }

    string key = action.substr(0, equalsPos);
    string value = action.substr(equalsPos + 1);

    // Check the action and perform the corresponding operation
    if (key == "--action") {
        if (value == "run-server") {
          runServer();
        } else if (value == "create-tables") {
          createTables();
        } else if (value == "drop-tables") {
          dropTables();
        } else {
          cerr << "Invalid action" << endl;
          return 1;
        }
    } else {
        cerr << "Invalid argument" << endl;
        return 1;
    }

    return 0;
}