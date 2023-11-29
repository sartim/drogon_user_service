#ifndef DROGON_USER_SERVICE_ENV_H
#define DROGON_USER_SERVICE_ENV_H

#include <fstream>
#include <iostream>
#include <map>
#include <string>

static std::map<std::string, std::string> readEnvFile(const std::string &filePath) {
  std::map<std::string, std::string> envMap;

  // Open the .env file
  std::ifstream envFile(filePath);

  // Check if the file is open
  if (!envFile.is_open()) {
    std::cerr << "Error opening file: " << filePath << std::endl;
    // You might want to handle this error more gracefully based on your
    // requirements
    exit(1);
  }

  // Read the file line by line
  std::string line;
  while (std::getline(envFile, line)) {
    // Find the position of the equal sign '='
    size_t equalPos = line.find('=');

    // Check if the line contains a valid key-value pair
    if (equalPos != std::string::npos) {
      // Extract key and value
      std::string key = line.substr(0, equalPos);
      std::string value = line.substr(equalPos + 1);

      // Insert key-value pair into the map
      envMap[key] = value;
    }
  }

  // Close the file
  envFile.close();

  return envMap;
}

static std::string getEnvVariable(const std::string &envVarName,
                           const std::map<std::string, std::string> &envMap) {
  auto it = envMap.find(envVarName);
  if (it != envMap.end()) {
    return it->second;
  } else {
    // Handle the case where the environment variable is not found
    return ""; // You might want to return a default value or throw an exception
  }
}

static std::string getEnv(const std::string &envVarName) {
  std::string envFilePath = ".env";
  std::map<std::string, std::string> envMap = readEnvFile(envFilePath);
  std::string keyVal = getEnvVariable("SECRET_KEY", envMap);

  return keyVal;
}

#endif // DROGON_USER_SERVICE_ENV_H
