#include <mariadb/mysql.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define ZEN_COL_BUF 256

#define ZEN_MYSQL_BIND_INT    1
#define ZEN_MYSQL_BIND_LONG   2
#define ZEN_MYSQL_BIND_DOUBLE 3
#define ZEN_MYSQL_BIND_STRING 4

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

#define TO_HANDLE(p) ((int64_t)(intptr_t)(p))
#define FROM_HANDLE(type, h) ((type *)(intptr_t)(h))

int64_t zen_mysql_connect(
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

    unsigned int connect_timeout = 10;
    unsigned int io_timeout = 60;

    mysql_options(db, MYSQL_OPT_CONNECT_TIMEOUT, &connect_timeout);
    mysql_options(db, MYSQL_OPT_READ_TIMEOUT, &io_timeout);
    mysql_options(db, MYSQL_OPT_WRITE_TIMEOUT, &io_timeout);
    mysql_options(db, MYSQL_SET_CHARSET_NAME, "utf8mb4");

    if (!mysql_real_connect(
        db,
        host,
        user,
        password,
        database,
        (unsigned int)port,
        NULL,
        0
    )) {
        set_error(mysql_error(db));
        mysql_close(db);
        return 0;
    }

    last_error[0] = '\0';

    return TO_HANDLE(db);
}

void zen_mysql_close(int64_t handle)
{
    if (handle) {
        mysql_close(FROM_HANDLE(MYSQL, handle));
    }
}

int zen_mysql_ok(int64_t handle)
{
    return handle != 0;
}

const char *zen_mysql_error(int64_t handle)
{
    if (last_error[0]) {
        return last_error;
    }

    if (!handle) {
        return "invalid database handle";
    }

    return mysql_error(FROM_HANDLE(MYSQL, handle));
}

int zen_mysql_query(int64_t handle, const char *sql)
{
    if (!handle || !sql) {
        set_error("invalid database handle or SQL");
        return 1;
    }

    MYSQL *db = FROM_HANDLE(MYSQL, handle);

    int result = mysql_query(db, sql);

    if (result != 0) {
        set_error(mysql_error(db));
    } else {
        last_error[0] = '\0';
    }

    return result;
}

int64_t zen_mysql_affected_rows(int64_t handle)
{
    if (!handle) {
        return 0;
    }

    return (int64_t)mysql_affected_rows(FROM_HANDLE(MYSQL, handle));
}

int64_t zen_mysql_insert_id(int64_t handle)
{
    if (!handle) {
        return 0;
    }

    return (int64_t)mysql_insert_id(FROM_HANDLE(MYSQL, handle));
}

typedef struct {
    MYSQL_RES *result;
    unsigned int columns;
} ZenMySQLResult;

typedef struct {
    MYSQL_ROW row;
    unsigned int columns;
    int owned;
} ZenMySQLRow;

static void row_destroy(ZenMySQLRow *row)
{
    if (row->owned && row->row) {
        for (unsigned int i = 0; i < row->columns; i++) {
            free(row->row[i]);
        }

        free(row->row);
    }

    free(row);
}

int64_t zen_mysql_result(int64_t handle)
{
    if (!handle) {
        set_error("invalid database handle");
        return 0;
    }

    MYSQL *db = FROM_HANDLE(MYSQL, handle);

    MYSQL_RES *res = mysql_store_result(db);

    if (!res) {
        if (mysql_field_count(db) != 0) {
            set_error(mysql_error(db));
        }

        return 0;
    }

    ZenMySQLResult *out = (ZenMySQLResult *)malloc(sizeof(ZenMySQLResult));

    if (!out) {
        mysql_free_result(res);
        set_error("out of memory");
        return 0;
    }

    out->result = res;
    out->columns = mysql_num_fields(res);

    last_error[0] = '\0';

    return TO_HANDLE(out);
}

int zen_mysql_column_count(int64_t result_handle)
{
    if (!result_handle) {
        return 0;
    }

    return (int)FROM_HANDLE(ZenMySQLResult, result_handle)->columns;
}

int64_t zen_mysql_row_count(int64_t result_handle)
{
    if (!result_handle) {
        return 0;
    }

    return (int64_t)mysql_num_rows(
        FROM_HANDLE(ZenMySQLResult, result_handle)->result
    );
}

int64_t zen_mysql_next_row(int64_t result_handle)
{
    if (!result_handle) {
        return 0;
    }

    ZenMySQLResult *res = FROM_HANDLE(ZenMySQLResult, result_handle);

    MYSQL_ROW row = mysql_fetch_row(res->result);

    if (!row) {
        return 0;
    }

    ZenMySQLRow *out = (ZenMySQLRow *)calloc(1, sizeof(ZenMySQLRow));

    if (!out) {
        set_error("out of memory");
        return 0;
    }

    out->row = row;
    out->columns = res->columns;
    out->owned = 0;

    return TO_HANDLE(out);
}

int zen_mysql_row_valid(int64_t row_handle)
{
    return row_handle != 0;
}

int zen_mysql_row_is_null(int64_t row_handle, int index)
{
    if (!row_handle) {
        return 1;
    }

    ZenMySQLRow *row = FROM_HANDLE(ZenMySQLRow, row_handle);

    if (index < 0 || (unsigned int)index >= row->columns) {
        return 1;
    }

    return row->row[index] == NULL;
}

const char *zen_mysql_row_get_string(int64_t row_handle, int index)
{
    if (!row_handle) {
        return "";
    }

    ZenMySQLRow *row = FROM_HANDLE(ZenMySQLRow, row_handle);

    if (index < 0 || (unsigned int)index >= row->columns) {
        return "";
    }

    if (!row->row[index]) {
        return "";
    }

    return row->row[index];
}

int zen_mysql_row_get_int(int64_t row_handle, int index)
{
    const char *value = zen_mysql_row_get_string(row_handle, index);

    if (!value[0]) {
        return 0;
    }

    return atoi(value);
}

int64_t zen_mysql_row_get_long(int64_t row_handle, int index)
{
    const char *value = zen_mysql_row_get_string(row_handle, index);

    if (!value[0]) {
        return 0;
    }

    return (int64_t)strtoll(value, NULL, 10);
}

double zen_mysql_row_get_double(int64_t row_handle, int index)
{
    const char *value = zen_mysql_row_get_string(row_handle, index);

    if (!value[0]) {
        return 0.0;
    }

    return atof(value);
}

void zen_mysql_row_free(int64_t row_handle)
{
    if (!row_handle) {
        return;
    }

    row_destroy(FROM_HANDLE(ZenMySQLRow, row_handle));
}

void zen_mysql_result_free(int64_t result_handle)
{
    if (!result_handle) {
        return;
    }

    ZenMySQLResult *res = FROM_HANDLE(ZenMySQLResult, result_handle);

    if (res->result) {
        mysql_free_result(res->result);
    }

    free(res);
}

int zen_mysql_begin(int64_t handle)
{
    return zen_mysql_query(handle, "START TRANSACTION");
}

int zen_mysql_commit(int64_t handle)
{
    return zen_mysql_query(handle, "COMMIT");
}

int zen_mysql_rollback(int64_t handle)
{
    return zen_mysql_query(handle, "ROLLBACK");
}

typedef struct {
    MYSQL_STMT *stmt;

    MYSQL_BIND *binds;
    unsigned int param_count;

    char **strings;
    unsigned long *string_lengths;
    int *ints;
    int64_t *longs;
    double *doubles;
    unsigned char *types;

    MYSQL_BIND *rbinds;
    char **rbufs;
    unsigned long *rlengths;
    unsigned char *rnull;
    unsigned int columns;
} ZenMySQLStmt;

static void stmt_result_free(ZenMySQLStmt *s)
{
    if (s->rbufs) {
        for (unsigned int i = 0; i < s->columns; i++) {
            free(s->rbufs[i]);
        }
    }

    free(s->rbinds);
    free(s->rbufs);
    free(s->rlengths);
    free(s->rnull);

    s->rbinds = NULL;
    s->rbufs = NULL;
    s->rlengths = NULL;
    s->rnull = NULL;
    s->columns = 0;
}

static void stmt_destroy(ZenMySQLStmt *s)
{
    if (s->stmt) {
        mysql_stmt_close(s->stmt);
    }

    if (s->strings) {
        for (unsigned int i = 0; i < s->param_count; i++) {
            free(s->strings[i]);
        }
    }

    stmt_result_free(s);

    free(s->binds);
    free(s->strings);
    free(s->string_lengths);
    free(s->ints);
    free(s->longs);
    free(s->doubles);
    free(s->types);
    free(s);
}

int64_t zen_mysql_prepare(int64_t handle, const char *sql)
{
    if (!handle || !sql) {
        set_error("invalid database handle or SQL");
        return 0;
    }

    MYSQL *db = FROM_HANDLE(MYSQL, handle);

    MYSQL_STMT *stmt = mysql_stmt_init(db);

    if (!stmt) {
        set_error(mysql_error(db));
        return 0;
    }

    if (mysql_stmt_prepare(stmt, sql, (unsigned long)strlen(sql)) != 0) {
        set_error(mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        return 0;
    }

    ZenMySQLStmt *out = (ZenMySQLStmt *)calloc(1, sizeof(ZenMySQLStmt));

    if (!out) {
        set_error("out of memory");
        mysql_stmt_close(stmt);
        return 0;
    }

    out->stmt = stmt;
    out->param_count = mysql_stmt_param_count(stmt);

    if (out->param_count > 0) {
        unsigned int n = out->param_count;

        out->binds = (MYSQL_BIND *)calloc(n, sizeof(MYSQL_BIND));
        out->strings = (char **)calloc(n, sizeof(char *));
        out->string_lengths = (unsigned long *)calloc(n, sizeof(unsigned long));
        out->ints = (int *)calloc(n, sizeof(int));
        out->longs = (int64_t *)calloc(n, sizeof(int64_t));
        out->doubles = (double *)calloc(n, sizeof(double));
        out->types = (unsigned char *)calloc(n, sizeof(unsigned char));

        if (!out->binds || !out->strings || !out->string_lengths ||
            !out->ints || !out->longs || !out->doubles || !out->types) {
            set_error("out of memory");
            stmt_destroy(out);
            return 0;
        }
    }

    last_error[0] = '\0';

    return TO_HANDLE(out);
}

static ZenMySQLStmt *param_stmt(int64_t stmt_handle, int index)
{
    if (!stmt_handle) {
        set_error("invalid statement");
        return NULL;
    }

    ZenMySQLStmt *s = FROM_HANDLE(ZenMySQLStmt, stmt_handle);

    if (index < 0 || (unsigned int)index >= s->param_count) {
        set_error("invalid parameter index");
        return NULL;
    }

    return s;
}

int zen_mysql_stmt_bind_int(int64_t stmt_handle, int index, int value)
{
    ZenMySQLStmt *s = param_stmt(stmt_handle, index);

    if (!s) {
        return 1;
    }

    s->ints[index] = value;
    s->types[index] = ZEN_MYSQL_BIND_INT;

    MYSQL_BIND *bind = &s->binds[index];

    memset(bind, 0, sizeof(MYSQL_BIND));

    bind->buffer_type = MYSQL_TYPE_LONG;
    bind->buffer = &s->ints[index];

    return 0;
}

int zen_mysql_stmt_bind_long(int64_t stmt_handle, int index, int64_t value)
{
    ZenMySQLStmt *s = param_stmt(stmt_handle, index);

    if (!s) {
        return 1;
    }

    s->longs[index] = value;
    s->types[index] = ZEN_MYSQL_BIND_LONG;

    MYSQL_BIND *bind = &s->binds[index];

    memset(bind, 0, sizeof(MYSQL_BIND));

    bind->buffer_type = MYSQL_TYPE_LONGLONG;
    bind->buffer = &s->longs[index];

    return 0;
}

int zen_mysql_stmt_bind_double(int64_t stmt_handle, int index, double value)
{
    ZenMySQLStmt *s = param_stmt(stmt_handle, index);

    if (!s) {
        return 1;
    }

    s->doubles[index] = value;
    s->types[index] = ZEN_MYSQL_BIND_DOUBLE;

    MYSQL_BIND *bind = &s->binds[index];

    memset(bind, 0, sizeof(MYSQL_BIND));

    bind->buffer_type = MYSQL_TYPE_DOUBLE;
    bind->buffer = &s->doubles[index];

    return 0;
}

int zen_mysql_stmt_bind_string(
    int64_t stmt_handle,
    int index,
    const char *value
) {
    ZenMySQLStmt *s = param_stmt(stmt_handle, index);

    if (!s) {
        return 1;
    }

    if (!value) {
        value = "";
    }

    char *copy = strdup(value);

    if (!copy) {
        set_error("out of memory");
        return 1;
    }

    free(s->strings[index]);

    s->strings[index] = copy;
    s->string_lengths[index] = (unsigned long)strlen(copy);
    s->types[index] = ZEN_MYSQL_BIND_STRING;

    MYSQL_BIND *bind = &s->binds[index];

    memset(bind, 0, sizeof(MYSQL_BIND));

    bind->buffer_type = MYSQL_TYPE_STRING;
    bind->buffer = copy;
    bind->buffer_length = s->string_lengths[index];
    bind->length = &s->string_lengths[index];

    return 0;
}

static int stmt_result_setup(ZenMySQLStmt *s)
{
    unsigned int n = mysql_stmt_field_count(s->stmt);

    if (n == 0) {
        return 0;
    }

    s->columns = n;
    s->rbinds = (MYSQL_BIND *)calloc(n, sizeof(MYSQL_BIND));
    s->rbufs = (char **)calloc(n, sizeof(char *));
    s->rlengths = (unsigned long *)calloc(n, sizeof(unsigned long));
    s->rnull = (unsigned char *)calloc(n, sizeof(unsigned char));

    if (!s->rbinds || !s->rbufs || !s->rlengths || !s->rnull) {
        set_error("out of memory");
        stmt_result_free(s);
        return 1;
    }

    for (unsigned int i = 0; i < n; i++) {
        s->rbufs[i] = (char *)malloc(ZEN_COL_BUF);

        if (!s->rbufs[i]) {
            set_error("out of memory");
            stmt_result_free(s);
            return 1;
        }

        s->rbinds[i].buffer_type = MYSQL_TYPE_STRING;
        s->rbinds[i].buffer = s->rbufs[i];
        s->rbinds[i].buffer_length = ZEN_COL_BUF - 1;
        s->rbinds[i].length = &s->rlengths[i];
        s->rbinds[i].is_null = (void *)&s->rnull[i];
    }

    if (mysql_stmt_bind_result(s->stmt, s->rbinds) != 0 ||
        mysql_stmt_store_result(s->stmt) != 0) {
        set_error(mysql_stmt_error(s->stmt));
        stmt_result_free(s);
        return 1;
    }

    return 0;
}

int zen_mysql_stmt_execute(int64_t stmt_handle)
{
    if (!stmt_handle) {
        set_error("invalid statement");
        return 1;
    }

    ZenMySQLStmt *s = FROM_HANDLE(ZenMySQLStmt, stmt_handle);

    stmt_result_free(s);

    for (unsigned int i = 0; i < s->param_count; i++) {
        if (!s->types[i]) {
            set_error("unbound parameter");
            return 1;
        }
    }

    if (s->param_count > 0) {
        if (mysql_stmt_bind_param(s->stmt, s->binds) != 0) {
            set_error(mysql_stmt_error(s->stmt));
            return 1;
        }
    }

    if (mysql_stmt_execute(s->stmt) != 0) {
        set_error(mysql_stmt_error(s->stmt));
        return 1;
    }

    if (stmt_result_setup(s) != 0) {
        return 1;
    }

    last_error[0] = '\0';

    return 0;
}

int64_t zen_mysql_stmt_next(int64_t stmt_handle)
{
    if (!stmt_handle) {
        return 0;
    }

    ZenMySQLStmt *s = FROM_HANDLE(ZenMySQLStmt, stmt_handle);

    if (!s->rbinds) {
        return 0;
    }

    int rc = mysql_stmt_fetch(s->stmt);

    if (rc == 1) {
        set_error(mysql_stmt_error(s->stmt));
        return 0;
    }

    if (rc == MYSQL_NO_DATA) {
        return 0;
    }

    ZenMySQLRow *out = (ZenMySQLRow *)calloc(1, sizeof(ZenMySQLRow));
    char **values = (char **)calloc(s->columns, sizeof(char *));

    if (!out || !values) {
        free(out);
        free(values);
        set_error("out of memory");
        return 0;
    }

    out->row = values;
    out->columns = s->columns;
    out->owned = 1;

    for (unsigned int i = 0; i < s->columns; i++) {
        if (s->rnull[i]) {
            continue;
        }

        unsigned long len = s->rlengths[i];

        char *copy = (char *)malloc(len + 1);

        if (!copy) {
            set_error("out of memory");
            row_destroy(out);
            return 0;
        }

        if (len > s->rbinds[i].buffer_length) {
            MYSQL_BIND bind;

            memset(&bind, 0, sizeof(bind));

            bind.buffer_type = MYSQL_TYPE_STRING;
            bind.buffer = copy;
            bind.buffer_length = len;

            if (mysql_stmt_fetch_column(s->stmt, &bind, i, 0) != 0) {
                set_error(mysql_stmt_error(s->stmt));
                free(copy);
                row_destroy(out);
                return 0;
            }
        } else {
            memcpy(copy, s->rbufs[i], len);
        }

        copy[len] = '\0';
        values[i] = copy;
    }

    return TO_HANDLE(out);
}

int zen_mysql_stmt_column_count(int64_t stmt_handle)
{
    if (!stmt_handle) {
        return 0;
    }

    return (int)FROM_HANDLE(ZenMySQLStmt, stmt_handle)->columns;
}

int64_t zen_mysql_stmt_row_count(int64_t stmt_handle)
{
    if (!stmt_handle) {
        return 0;
    }

    ZenMySQLStmt *s = FROM_HANDLE(ZenMySQLStmt, stmt_handle);

    if (!s->rbinds) {
        return 0;
    }

    return (int64_t)mysql_stmt_num_rows(s->stmt);
}

int64_t zen_mysql_stmt_affected_rows(int64_t stmt_handle)
{
    if (!stmt_handle) {
        return 0;
    }

    return (int64_t)mysql_stmt_affected_rows(
        FROM_HANDLE(ZenMySQLStmt, stmt_handle)->stmt
    );
}

int64_t zen_mysql_stmt_insert_id(int64_t stmt_handle)
{
    if (!stmt_handle) {
        return 0;
    }

    return (int64_t)mysql_stmt_insert_id(
        FROM_HANDLE(ZenMySQLStmt, stmt_handle)->stmt
    );
}

const char *zen_mysql_stmt_error(int64_t stmt_handle)
{
    if (last_error[0]) {
        return last_error;
    }

    if (!stmt_handle) {
        return "invalid statement";
    }

    return mysql_stmt_error(FROM_HANDLE(ZenMySQLStmt, stmt_handle)->stmt);
}

void zen_mysql_stmt_close(int64_t stmt_handle)
{
    if (!stmt_handle) {
        return;
    }

    stmt_destroy(FROM_HANDLE(ZenMySQLStmt, stmt_handle));
}

