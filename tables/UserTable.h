#pragma once

#include <drogon/drogon.h>

using namespace drogon;
using namespace drogon::orm;

class UserTable
{
  public:
    UserTable() {};

  public:
    void create(drogon::orm::DbClientPtr client);
    void alter(drogon::orm::DbClientPtr client);
    void _delete(drogon::orm::DbClientPtr client);
};

