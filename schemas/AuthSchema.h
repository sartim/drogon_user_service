#include <functional>
#include <iostream>
#include <map>
#include <string>

using namespace std;

class AuthSchema {
    public:
        AuthSchema() {
            addField("email", true);
            addField("password", true);
        }

        void addField(const std::string &fieldName, bool required) {
            schema_[fieldName] = required;
        };
        
        std::vector<std::string> validate(const std::map<std::string, std::string> &jsonBody) const;

    private:
        std::map<std::string, bool> schema_;
};
