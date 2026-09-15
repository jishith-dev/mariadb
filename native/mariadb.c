/*
 * mysql.c
 *
 * Zen <-> MySQL/MariaDB native bridge.
 *
 * Requires MariaDB Connector/C.
 */

#include <mariadb/mysql.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ---------------------------------------------------------
 * Error state
 * --------------------------------------------------------- */

static char last_error[512];

static void set_error(const char *message)
{
    if (!message) {
        last_error[0] = '\0';
        return;
    }

    strncpy(last_error, message, sizeof(last_error) - 1);
    last_error[sizeof(last_error) - 1] = '\0';
}

/* ---------------------------------------------------------
 * Database
 * --------------------------------------------------------- */

long zen_mysql_connect(
    const char *host,
    const char *user,
    const char *password,
    const char *database,
    int port
) {
    MYSQL *db = mysql_init(NULL);

    if (!db) {
        set_error("mysql_init failed");
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
        set_error(mysql_error(db));
        mysql_close(db);
        return 0;
    }

    last_error[0] = '\0';

    return (long)db;
}

void zen_mysql_close(long handle)
{
    if (handle) {
        mysql_close((MYSQL *)handle);
    }
}

int zen_mysql_ok(long handle)
{
    return handle != 0;
}

const char *zen_mysql_error(long handle)
{
    if (!handle) {
        return last_error[0]
            ? last_error
            : "invalid database handle";
    }

    return mysql_error((MYSQL *)handle);
}

/* ---------------------------------------------------------
 * Normal query
 * --------------------------------------------------------- */

int zen_mysql_query(
    long handle,
    const char *sql
) {
    if (!handle || !sql) {
        set_error("invalid database handle or SQL");
        return 1;
    }

    MYSQL *db = (MYSQL *)handle;

    int result = mysql_query(db, sql);

    if (result != 0) {
        set_error(mysql_error(db));
    } else {
        last_error[0] = '\0';
    }

    return result;
}

/* ---------------------------------------------------------
 * Query statistics
 * --------------------------------------------------------- */

long zen_mysql_affected_rows(long handle)
{
    if (!handle) {
        return 0;
    }

    return (long)mysql_affected_rows((MYSQL *)handle);
}

long zen_mysql_insert_id(long handle)
{
    if (!handle) {
        return 0;
    }

    return (long)mysql_insert_id((MYSQL *)handle);
}

/* ---------------------------------------------------------
 * SELECT Result
 * --------------------------------------------------------- */

typedef struct {
    MYSQL_RES *result;
    unsigned int columns;
} ZenMySQLResult;

long zen_mysql_result(long handle)
{
    if (!handle) {
        set_error("invalid database handle");
        return 0;
    }

    MYSQL *db = (MYSQL *)handle;

    MYSQL_RES *res = mysql_store_result(db);

    if (!res) {
        if (mysql_field_count(db) != 0) {
            set_error(mysql_error(db));
        }

        return 0;
    }

    ZenMySQLResult *out =
        (ZenMySQLResult *)malloc(sizeof(ZenMySQLResult));

    if (!out) {
        mysql_free_result(res);
        set_error("out of memory");
        return 0;
    }

    out->result = res;
    out->columns = mysql_num_fields(res);

    last_error[0] = '\0';

    return (long)out;
}

int zen_mysql_column_count(long result_handle)
{
    if (!result_handle) {
        return 0;
    }

    ZenMySQLResult *res =
        (ZenMySQLResult *)result_handle;

    return (int)res->columns;
}

long zen_mysql_row_count(long result_handle)
{
    if (!result_handle) {
        return 0;
    }

    ZenMySQLResult *res =
        (ZenMySQLResult *)result_handle;

    return (long)mysql_num_rows(res->result);
}

/* ---------------------------------------------------------
 * Row
 * --------------------------------------------------------- */

typedef struct {
    MYSQL_ROW row;
    unsigned long *lengths;
    unsigned int columns;
} ZenMySQLRow;

long zen_mysql_next_row(long result_handle)
{
    if (!result_handle) {
        return 0;
    }

    ZenMySQLResult *res =
        (ZenMySQLResult *)result_handle;

    MYSQL_ROW row =
        mysql_fetch_row(res->result);

    if (!row) {
        return 0;
    }

    ZenMySQLRow *out =
        (ZenMySQLRow *)malloc(sizeof(ZenMySQLRow));

    if (!out) {
        set_error("out of memory");
        return 0;
    }

    out->row = row;
    out->lengths = mysql_fetch_lengths(res->result);
    out->columns = res->columns;

    return (long)out;
}

int zen_mysql_row_valid(long row_handle)
{
    return row_handle != 0;
}

const char *zen_mysql_row_get_string(
    long row_handle,
    int index
) {
    if (!row_handle) {
        return "";
    }

    ZenMySQLRow *row =
        (ZenMySQLRow *)row_handle;

    if (index < 0 || (unsigned int)index >= row->columns) {
        return "";
    }

    if (!row->row[index]) {
        return "";
    }

    return row->row[index];
}

int zen_mysql_row_get_int(
    long row_handle,
    int index
) {
    const char *value =
        zen_mysql_row_get_string(row_handle, index);

    if (!value || !value[0]) {
        return 0;
    }

    return atoi(value);
}

long zen_mysql_row_get_long(
    long row_handle,
    int index
) {
    const char *value =
        zen_mysql_row_get_string(row_handle, index);

    if (!value || !value[0]) {
        return 0;
    }

    return atol(value);
}

double zen_mysql_row_get_double(
    long row_handle,
    int index
) {
    const char *value =
        zen_mysql_row_get_string(row_handle, index);

    if (!value || !value[0]) {
        return 0.0;
    }

    return atof(value);
}

void zen_mysql_row_free(long row_handle)
{
    if (!row_handle) {
        return;
    }

    free((ZenMySQLRow *)row_handle);
}

/* ---------------------------------------------------------
 * Result cleanup
 * --------------------------------------------------------- */

void zen_mysql_result_free(long result_handle)
{
    if (!result_handle) {
        return;
    }

    ZenMySQLResult *res =
        (ZenMySQLResult *)result_handle;

    if (res->result) {
        mysql_free_result(res->result);
    }

    free(res);
}

/* ---------------------------------------------------------
 * Transactions
 * --------------------------------------------------------- */

int zen_mysql_begin(long handle)
{
    return zen_mysql_query(handle, "START TRANSACTION");
}

int zen_mysql_commit(long handle)
{
    return zen_mysql_query(handle, "COMMIT");
}

int zen_mysql_rollback(long handle)
{
    return zen_mysql_query(handle, "ROLLBACK");
}

/* ---------------------------------------------------------
 * Prepared statements
 * --------------------------------------------------------- */

typedef struct {
    MYSQL_STMT *stmt;

    MYSQL_BIND *binds;
    unsigned int param_count;

    char **strings;
    unsigned long *string_lengths;

    int *ints;
    long *longs;
    double *doubles;

    unsigned char *types;
} ZenMySQLStmt;

#define ZEN_MYSQL_BIND_INT    1
#define ZEN_MYSQL_BIND_LONG   2
#define ZEN_MYSQL_BIND_DOUBLE 3
#define ZEN_MYSQL_BIND_STRING 4

long zen_mysql_prepare(
    long handle,
    const char *sql
) {
    if (!handle || !sql) {
        set_error("invalid database handle or SQL");
        return 0;
    }

    MYSQL *db = (MYSQL *)handle;

    MYSQL_STMT *stmt = mysql_stmt_init(db);

    if (!stmt) {
        set_error(mysql_error(db));
        return 0;
    }

    if (mysql_stmt_prepare(
        stmt,
        sql,
        (unsigned long)strlen(sql)
    ) != 0) {
        set_error(mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return 0;
    }

    ZenMySQLStmt *out =
        (ZenMySQLStmt *)calloc(1, sizeof(ZenMySQLStmt));

    if (!out) {
        set_error("out of memory");
        mysql_stmt_close(stmt);
        return 0;
    }

    out->stmt = stmt;
    out->param_count = mysql_stmt_param_count(stmt);

    if (out->param_count > 0) {
        out->binds =
            (MYSQL_BIND *)calloc(
                out->param_count,
                sizeof(MYSQL_BIND)
            );

        out->strings =
            (char **)calloc(
                out->param_count,
                sizeof(char *)
            );

        out->string_lengths =
            (unsigned long *)calloc(
                out->param_count,
                sizeof(unsigned long)
            );

        out->ints =
            (int *)calloc(
                out->param_count,
                sizeof(int)
            );

        out->longs =
            (long *)calloc(
                out->param_count,
                sizeof(long)
            );

        out->doubles =
            (double *)calloc(
                out->param_count,
                sizeof(double)
            );

        out->types =
            (unsigned char *)calloc(
                out->param_count,
                sizeof(unsigned char)
            );
    }

    last_error[0] = '\0';

    return (long)out;
}

/* ---------------------------------------------------------
 * Prepared statement binding
 * --------------------------------------------------------- */

static int valid_param(
    ZenMySQLStmt *stmt,
    int index
) {
    if (!stmt) {
        return 0;
    }

    if (index < 0 ||
        (unsigned int)index >= stmt->param_count) {
        return 0;
    }

    return 1;
}

int zen_mysql_stmt_bind_int(
    long stmt_handle,
    int index,
    int value
) {
    if (!stmt_handle) {
        set_error("invalid statement");
        return 1;
    }

    ZenMySQLStmt *stmt =
        (ZenMySQLStmt *)stmt_handle;

    if (!valid_param(stmt, index)) {
        set_error("invalid parameter index");
        return 1;
    }

    stmt->ints[index] = value;
    stmt->types[index] = ZEN_MYSQL_BIND_INT;

    MYSQL_BIND *bind = &stmt->binds[index];

    memset(bind, 0, sizeof(MYSQL_BIND));

    bind->buffer_type = MYSQL_TYPE_LONG;
    bind->buffer = &stmt->ints[index];

    return 0;
}

int zen_mysql_stmt_bind_long(
    long stmt_handle,
    int index,
    long value
) {
    if (!stmt_handle) {
        set_error("invalid statement");
        return 1;
    }

    ZenMySQLStmt *stmt =
        (ZenMySQLStmt *)stmt_handle;

    if (!valid_param(stmt, index)) {
        set_error("invalid parameter index");
        return 1;
    }

    stmt->longs[index] = value;
    stmt->types[index] = ZEN_MYSQL_BIND_LONG;

    MYSQL_BIND *bind = &stmt->binds[index];

    memset(bind, 0, sizeof(MYSQL_BIND));

    bind->buffer_type = MYSQL_TYPE_LONGLONG;
    bind->buffer = &stmt->longs[index];

    return 0;
}

int zen_mysql_stmt_bind_double(
    long stmt_handle,
    int index,
    double value
) {
    if (!stmt_handle) {
        set_error("invalid statement");
        return 1;
    }

    ZenMySQLStmt *stmt =
        (ZenMySQLStmt *)stmt_handle;

    if (!valid_param(stmt, index)) {
        set_error("invalid parameter index");
        return 1;
    }

    stmt->doubles[index] = value;
    stmt->types[index] = ZEN_MYSQL_BIND_DOUBLE;

    MYSQL_BIND *bind = &stmt->binds[index];

    memset(bind, 0, sizeof(MYSQL_BIND));

    bind->buffer_type = MYSQL_TYPE_DOUBLE;
    bind->buffer = &stmt->doubles[index];

    return 0;
}

int zen_mysql_stmt_bind_string(
    long stmt_handle,
    int index,
    const char *value
) {
    if (!stmt_handle) {
        set_error("invalid statement");
        return 1;
    }

    ZenMySQLStmt *stmt =
        (ZenMySQLStmt *)stmt_handle;

    if (!valid_param(stmt, index)) {
        set_error("invalid parameter index");
        return 1;
    }

    free(stmt->strings[index]);

    if (!value) {
        value = "";
    }

    stmt->strings[index] =
        strdup(value);

    if (!stmt->strings[index]) {
        set_error("out of memory");
        return 1;
    }

    stmt->string_lengths[index] =
        (unsigned long)strlen(value);

    stmt->types[index] = ZEN_MYSQL_BIND_STRING;

    MYSQL_BIND *bind = &stmt->binds[index];

    memset(bind, 0, sizeof(MYSQL_BIND));

    bind->buffer_type = MYSQL_TYPE_STRING;
    bind->buffer = stmt->strings[index];
    bind->buffer_length = stmt->string_lengths[index];
    bind->length = &stmt->string_lengths[index];

    return 0;
}

/* ---------------------------------------------------------
 * Prepared statement execute
 * --------------------------------------------------------- */

int zen_mysql_stmt_execute(long stmt_handle)
{
    if (!stmt_handle) {
        set_error("invalid statement");
        return 1;
    }

    ZenMySQLStmt *stmt =
        (ZenMySQLStmt *)stmt_handle;

    if (stmt->param_count > 0) {
        if (mysql_stmt_bind_param(
            stmt->stmt,
            stmt->binds
        ) != 0) {
            set_error(mysql_stmt_error(stmt->stmt));
            return 1;
        }
    }

    if (mysql_stmt_execute(stmt->stmt) != 0) {
        set_error(mysql_stmt_error(stmt->stmt));
        return 1;
    }

    last_error[0] = '\0';

    return 0;
}

long zen_mysql_stmt_affected_rows(long stmt_handle)
{
    if (!stmt_handle) {
        return 0;
    }

    ZenMySQLStmt *stmt =
        (ZenMySQLStmt *)stmt_handle;

    return (long)mysql_stmt_affected_rows(stmt->stmt);
}

/* ---------------------------------------------------------
 * Prepared statement close
 * --------------------------------------------------------- */

void zen_mysql_stmt_close(long stmt_handle)
{
    if (!stmt_handle) {
        return;
    }

    ZenMySQLStmt *stmt =
        (ZenMySQLStmt *)stmt_handle;

    if (stmt->stmt) {
        mysql_stmt_close(stmt->stmt);
    }

    if (stmt->strings) {
        for (unsigned int i = 0;
             i < stmt->param_count;
             i++) {
            free(stmt->strings[i]);
        }
    }

    free(stmt->binds);
    free(stmt->strings);
    free(stmt->string_lengths);
    free(stmt->ints);
    free(stmt->longs);
    free(stmt->doubles);
    free(stmt->types);

    free(stmt);
}
