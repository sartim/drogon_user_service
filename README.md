# Drogon User Service

User service using Drogon Framework. Make sure to add models.json and config.json.

## Requirements

    * [Drogon](https://github.com/drogonframework/drogon)
    * [PostgreSQL](https://www.postgresql.org)
    * [JWT-CPP](https://github.com/Thalhammer/jwt-cpp)
    * [Bcrypt](https://github.com/rg3/bcrypt.git)
    * [OpenSSL](https://github.com/openssl/openssl.git)

## Setup bcrypt

    $ sudo cp bcrypt.a /usr/local/lib/
    $ sudo cp bcrypt.h /usr/local/include/

## Install jwt-cpp

    $ cd build
    $ cmake ..
    $ make
    $ sudo make installl
    

## Install OpenSSL

    $ cd openssl
    $ ./config shared no-ssl2
    $ make
    $ sudo make install


## Running server

    $ cd build
    $ cmake ..
    $ make
    $ ./drogoncore_user_service
