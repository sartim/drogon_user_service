#pragma once

#include "BaseTable.h"
#include <drogon/drogon.h>

using namespace drogon;
using namespace drogon::orm;

class UserTable : public BaseTable {
public:
  UserTable() { connect(); };

public:
  void create();
  void alter();
  void _delete();
};
