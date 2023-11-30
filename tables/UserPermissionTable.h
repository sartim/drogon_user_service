#pragma once

#include "BaseTable.h"
#include <drogon/drogon.h>

using namespace std;
using namespace drogon;
using namespace drogon::orm;

class UserPermissionTable : public BaseTable {
public:
  UserPermissionTable() { }

public:
  void create(const string &connectionString);
  void alter(const string &connectionString);
  void _delete(const string &connectionString);
};
