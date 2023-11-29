#pragma once

#include "BaseController.h"
#include <drogon/HttpController.h>
#include <drogon/HttpResponse.h>
#include <drogon/orm/DbClient.h>

using namespace std;
using namespace drogon;
using namespace drogon::orm;

class UserController : public HttpController<UserController, false>,
                       public BaseController {
public:
  METHOD_LIST_BEGIN
  METHOD_ADD(UserController::getHeaders, "/api/v1/user", Options);
  METHOD_ADD(UserController::getByIdHeaders, "/api/v1/user/{id}", Options);
  METHOD_ADD(UserController::getUsers, "/api/v1/user", Get, "AuthFilter");
  METHOD_ADD(UserController::getUserById, "/api/v1/user/{id}", Get,
             "AuthFilter");
  METHOD_ADD(UserController::createUser, "/api/v1/user", Post, "AuthFilter");
  METHOD_ADD(UserController::updateUserById, "/api/v1/user/{id}", Put,
             "AuthFilter");
  METHOD_ADD(UserController::deleteUserById, "/api/v1/user/{id}", Delete,
             "AuthFilter");
  METHOD_LIST_END

public:
  UserController() { connect(); }

public:
  // Get a list of all users
  void getUsers(const HttpRequestPtr &req,
                function<void(const HttpResponsePtr &)> &&callback);

  // Get a user by their ID
  void getUserById(const HttpRequestPtr &req,
                   function<void(const HttpResponsePtr &)> &&callback,
                   string id);

  // Create a new user
  void createUser(const HttpRequestPtr &req,
                  function<void(const HttpResponsePtr &)> &&callback);

  // Update a user by their ID
  void updateUserById(const HttpRequestPtr &req,
                      function<void(const HttpResponsePtr &)> &&callback,
                      const string &id);

  // Delete a user by their ID
  void deleteUserById(const HttpRequestPtr &req,
                      function<void(const HttpResponsePtr &)> &&callback,
                      const string &id);

private:
  drogon::orm::DbClientPtr client;
  void connect();
};
