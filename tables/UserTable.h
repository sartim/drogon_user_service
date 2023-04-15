#pragma once

#include <drogon/drogon.h>

using namespace drogon;
using namespace drogon::orm;

class UserTable
{
  public:
    void create();
    void alter();
    void _delete();
};

