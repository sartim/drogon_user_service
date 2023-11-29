#pragma once

#include "BaseTable.h"
#include <drogon/drogon.h>

using namespace drogon;
using namespace drogon::orm;

class UserPermissionTable : public BaseTable {
public:
  UserPermissionTable() { connect(); }

public:
  void create();
  void alter();
  void _delete();
};
