#!/bin/sh

cat << EOF > config.json
{
  "secret_key": "${SECRET_KEY}",
  "db_clients":[
      {
        "name":"default",
        "rdbms": "postgresql",
        "host": ${DB_HOST},
        "port": ${DB_PORT},
        "dbname": ${DB_NAME},
        "user": ${DB_USER},
        "passwd": ${DB_PASS},
        "is_fast": false,
        "connection_number": 1,
        "filename": ""
      }
    ]
}
EOF
