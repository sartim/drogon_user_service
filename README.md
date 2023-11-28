# User Service

[![Language](https://img.shields.io/badge/language-cpp-green.svg)](https://github.com/sartim/drogon_user_service)
[![Build Status](https://github.com/sartim/drogon_user_service/workflows/build/badge.svg)](https://github.com/sartim/drogon_user_service)

User service running on Drogon Framework which handles RBAC management. Make sure to add models.json and config.json.

## Requirements

* [Drogon](https://github.com/drogonframework/drogon)
* [PostgreSQL](https://www.postgresql.org)
* [JWT-CPP](https://github.com/Thalhammer/jwt-cpp)
* [Bcrypt](https://git@github.com:hilch/Bcrypt.cpp.git)
* [OpenSSL](https://github.com/openssl/openssl.git)

## Create .env file

    SECRET_KEY={SECRET_KEY}
    DB_NAME={DB_NAME}
    DB_USER={DB_USER}
    DB_PASSWORD={DB_PASSWORD}

## Setup bcrypt

On the project root:

    $ git clone git@github.com:hilch/Bcrypt.cpp.git

## Install jwt-cpp

    $ cd build
    $ cmake ..
    $ make
    $ sudo make install
    

## Install OpenSSL

    $ cd openssl
    $ ./config shared no-ssl2
    $ make
    $ make install


## Running server

    $ cd build
    $ cmake ..
    $ make
    $ ./drogoncore_user_service --action=run-server

## Running with docker
    
    $ docker build --build-arg DB_HOST=127.0.0.1 --build-arg DB_PORT=5432 --build-arg DB_NAME=db_name --build-arg DB_USER=username --build-arg DB_PASS=password -t drogon-user-service .
    $ docker run --name drogon-user-service -d -p 8000:8000 drogon-user-service:latest
