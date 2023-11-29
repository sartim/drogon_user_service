#include <string>
#include <jwt-cpp/jwt.h>

using namespace std;

string generateJWT(const string &secretKey, const string &email) {
  auto token =
      jwt::create()
          .set_issuer("auth0")
          .set_type("JWS")
          .set_payload_claim("identity", jwt::claim(email))
          .set_issued_at(chrono::system_clock::now())
          .set_expires_at(chrono::system_clock::now() + chrono::seconds{3600})
          .sign(jwt::algorithm::hs256{secretKey});
  return token;
}
