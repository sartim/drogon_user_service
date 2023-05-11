# User Service

User service running on Drogon Framework which handles RBAC management. Make sure to add models.json and config.json.

## Requirements

* [Drogon](https://github.com/drogonframework/drogon)
* [PostgreSQL](https://www.postgresql.org)
* [JWT-CPP](https://github.com/Thalhammer/jwt-cpp)
* [Bcrypt](https://github.com/rg3/bcrypt.git)
* [OpenSSL](https://github.com/openssl/openssl.git)

## Create .env file

    SECRET_KEY={SECRET_KEY}
    DB_NAME={DB_NAME}
    DB_USER={DB_USER}
    DB_PASSWORD={DB_PASSWORD}

## Setup bcrypt

    $ sudo cp bcrypt.a /usr/local/lib/
    $ sudo cp bcrypt.h /usr/local/include/

## Install jwt-cpp

    $ cd build
    $ cmake ..
    $ make
    $ sudo make install
    

## Install OpenSSL

    $ cd openssl
    $ ./config shared no-ssl2
    $ make
    $ sudo make install


## Running server

    $ cd build
    $ cmake ..
    $ make
    $ ./drogoncore_user_service --action=run-server

## Running with docker
    
    $ docker build --build-arg DB_HOST=127.0.0.1 --build-arg DB_PORT=5432 --build-arg DB_NAME=db_name --build-arg DB_USER=username --build-arg DB_PASS=password -t drogon-user-service .
    $ docker run --name drogon-user-service -d -p 8000:8000 drogon-user-service:latest
