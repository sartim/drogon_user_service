#pragma once

#include "BaseTable.h"
#include <drogon/drogon.h>

using namespace drogon;
using namespace drogon::orm;

class RolePermissionTable : public BaseTable {
public:
  RolePermissionTable() { connect(); }

public:
  void create();
  void alter();
  void _delete();
};
