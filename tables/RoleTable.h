#pragma once

#include <drogon/drogon.h>
#include "BaseTable.h"

using namespace drogon;
using namespace drogon::orm;

class RoleTable : public BaseTable
{
  public:
      RoleTable() {
          connect();
      };

  public:
      void create();
      void alter();
      void _delete();
};
