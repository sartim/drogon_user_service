cat << EOF > models/model.json
{
    "rdbms": "postgresql",
    "host": ${DB_HOST},
    "port": 5432,
    "dbname": ${DB_NAME},
    "schema": "public",
    "user": ${DB_USER},
    "password": ${DB_PASSWORD},
    "tables": [
        "users",
        "roles",
        "permissions",
        "user_permisisons",
        "role_permissions"
    ],
    "relationships": {
        "enabled": false,
        "items": []
    },
    "restful_api_controllers": {
        "enabled": false,
        "resource_uri": "/*",
        "class_name": "Restful*Ctrl",
        "filters": [],
        "db_client": {
            "name": "default",
            "is_fast": false
        },
        "directory": "controllers",
        "generate_base_only": false
    }
}
EOF
