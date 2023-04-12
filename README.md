# Drogon User Service

User service using Drogon Framework. Make sure to add models.json and config.json.

## Requirements

    * Drogon 1.8.4

# Install OpenSSL

    $ git clone https://github.com/openssl/openssl.git
    $ cd openssl
    $ ./config shared no-ssl2
    $ make
    $ sudo make install


## Running server

    $ cd build
    $ cmake ..
    $ make
    $ ./drogoncore_user_service
