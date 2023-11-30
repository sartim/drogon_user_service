#pragma once

#include "BaseTable.h"
#include <drogon/drogon.h>

using namespace std;
using namespace drogon;
using namespace drogon::orm;

class RoleTable : public BaseTable {
public:
  RoleTable() { };

public:
  void create(const string &connectionString);
  void alter(const string &connectionString);
  void _delete(const string &connectionString);
};
