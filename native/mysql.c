#include <mariadb/mysql.h>
#include <string.h>

static char last_error[512];

long zen_mysql_connect(
    const char *host,
    const char *user,
    const char *password,
    const char *database,
    int port
) {
    MYSQL *db = mysql_init(NULL);

    if (!db) {
        strcpy(last_error, "mysql_init failed");
        return 0;
    }

    if (!mysql_real_connect(
        db,
        host,
        user,
        password,
        database,
        port,
        NULL,
        0
    )) {
        strncpy(last_error, mysql_error(db), sizeof(last_error) - 1);
        last_error[sizeof(last_error) - 1] = '\0';

        mysql_close(db);
        return 0;
    }

    last_error[0] = '\0';
    return (long)db;
}

void zen_mysql_close(long handle) {
    if (handle) {
        mysql_close((MYSQL *)handle);
    }
}

int zen_mysql_query(long handle, const char *sql) {
    if (!handle || !sql) {
        return 1;
    }

    return mysql_query((MYSQL *)handle, sql);
}

const char *zen_mysql_error(long handle) {
    if (!handle) {
        return last_error[0]
            ? last_error
            : "invalid database handle";
    }

    return mysql_error((MYSQL *)handle);
}
