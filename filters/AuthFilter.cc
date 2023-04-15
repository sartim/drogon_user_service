/**
 *
 *  AuthFilter.cc
 *
 */

#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/hmac.h>
#include <sstream>
#include <iomanip>
#include <ctime>
#include "AuthFilter.h"

using namespace drogon;

bool verifyJWT(const std::string& secretKey, const std::string& jwt)
{
    // Split the JWT into header, payload and signature
    std::stringstream ss(jwt);
    std::string header, payload, signature;
    if (!std::getline(ss, header, '.') ||
        !std::getline(ss, payload, '.') ||
        !std::getline(ss, signature)) {
        return false;
    }

    // Decode the payload
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO* bmem = BIO_new_mem_buf(payload.c_str(), payload.length());
    bmem = BIO_push(b64, bmem);
    char* decodedPayload = new char[payload.length() * 3 / 4 + 1];
    int decodedPayloadLen = BIO_read(bmem, decodedPayload, payload.length() * 3 / 4 + 1);
    decodedPayload[decodedPayloadLen] = '\0';
    std::string decodedPayloadStr(decodedPayload, decodedPayloadLen);
    delete[] decodedPayload;

    // Generate the header
    std::string header2 = "{\"alg\":\"HS256\",\"typ\":\"JWT\"}";
    std::string encodedHeader;
    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new(BIO_s_mem());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(b64, header2.c_str(), header2.length());
    BIO_flush(b64);
    bmem = BIO_push(b64, bmem);
    char* encodedHeaderCharPtr;
    long encodedHeaderSize = BIO_get_mem_data(bmem, &encodedHeaderCharPtr);
    encodedHeader = std::string(encodedHeaderCharPtr, encodedHeaderSize);

    // Generate the signature
    std::stringstream ss2;
    ss2 << encodedHeader << "." << decodedPayloadStr;
    std::string message = ss2.str();
    unsigned char* hmacResult = nullptr;
    unsigned int hmacResultLen = 0;
    hmacResult = HMAC(EVP_sha256(), secretKey.c_str(), secretKey.length(),
        (unsigned char*)message.c_str(), message.length(),
        hmacResult, &hmacResultLen);
    std::string calculatedSignature;
    for (unsigned int i = 0; i < hmacResultLen; i++) {
        std::stringstream ss2;
        ss2 << std::hex << std::setfill('0') << std::setw(2) << (int)hmacResult[i];
        calculatedSignature += ss2.str();
    }

    // Compare the calculated signature with the signature in the JWT
    return calculatedSignature == signature;
}


void AuthFilter::doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb)
{  
    const std::string token = req->getHeader("Authorization");
    std::string secretKey = "mysecretkey";
    if (verifyJWT(secretKey, token)) {
        auto response = drogon::HttpResponse::newHttpResponse();
        response->setStatusCode(k401Unauthorized);
        response->setBody("Unauthorized");
        fcb(response);
    } else {
        fccb();
    }
}
