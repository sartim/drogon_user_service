#pragma once

#include "BaseTable.h"

using namespace std;

class UserTable : public BaseTable {
public:
  UserTable() { };

public:
  void create(const string &connectionString);
  void alter(const string &connectionString);
  void _delete(const string &connectionString);
};
