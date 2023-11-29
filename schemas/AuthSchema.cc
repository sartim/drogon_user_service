#include "AuthSchema.h"
#include <functional>
#include <iostream>
#include <map>
#include <string>

using namespace std;

std::vector<std::string>
AuthSchema::validate(const std::map<std::string, std::string> &jsonBody) const {
  std::vector<std::string> errors;

  for (const auto &field : schema_) {
    const std::string &fieldName = field.first;
    bool required = field.second;

    // if (required && !jsonBody.isMember(fieldName)) {
    //     errors.push_back("Field '" + fieldName + "' is required.");
    // }
  }

  return errors;
};
