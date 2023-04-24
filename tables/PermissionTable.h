#pragma once

#include <drogon/drogon.h>
#include "BaseTable.h"

using namespace drogon;
using namespace drogon::orm;

class PermissionTable: public BaseTable
{
    public:
        PermissionTable() {
            connect();
        }

    public:
        void create();
        void alter();
        void _delete();
};
