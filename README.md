# mariadb

MariaDB client package for Zen.

**Author:** Jishith M P  
**Version:** 2.0.0

## Installation

```text
zen install mariadb
```

## Import

```zen
import (
  Database,
  Result,
  Row,
  Statement,
  connect
) from "mariadb"
```

## Connect

```zen
Database db = connect(
  "127.0.0.1",
  "root",
  "",
  "test",
  3306
)

if (!db.ok()) {
  screen(db.error())
}
```

`connect()` creates a MariaDB connection.

### Database APIs

```text
ok() bool
error() string
query(string sql) int
affectedRows() long
insertId() long
result() Result
begin() int
commit() int
rollback() int
prepare(string sql) Statement
run(string sql, List<string> params) Statement
close() void
```

## Raw Queries

Execute SQL directly with `query()`.

```zen
int code = db.query(
  "CREATE TABLE users (id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(50))"
)

if (code != 0) {
  screen(db.error())
}
```

`0` indicates success.

After an INSERT, UPDATE, or DELETE:

```zen
screen(db.affectedRows())
screen(db.insertId())
```

## Results

After a SELECT query:

```zen
db.query("SELECT id, name FROM users")

Result r = db.result()
```

### Result APIs

```text
ok() bool
columnCount() int
rowCount() long
next() Row
close() void
```

Example:

```zen
Row row = r.next()

while (row.valid()) {
  screen(row.getInt(0))
  screen(row.getString(1))

  row.close()
  row = r.next()
}

r.close()
```

## Rows

A `Row` represents one database row.

### Row APIs

```text
valid() bool
isNull(int index) bool
getString(int index) string
getInt(int index) int
getLong(int index) long
getDouble(int index) double
close() void
```

Column indexes start at `0`.

```zen
if (row.isNull(2)) {
  screen("value is NULL")
} else {
  screen(row.getLong(2))
}
```

## Prepared Statements

Use prepared statements with `?` parameters.

```zen
Statement st = db.prepare(
  "INSERT INTO users (name, age) VALUES (?, ?)"
)

st.bindString(0, "Jishith")
st.bindInt(1, 21)

int code = st.execute()

if (code != 0) {
  screen(st.error())
}

st.close()
```

### Binding APIs

```text
bindInt(int index, int value) int
bindLong(int index, long value) int
bindDouble(int index, double value) int
bindString(int index, string value) int
bindAll(List<string> params) int
```

`bindAll()` binds all supplied parameters as strings.

## Prepared SELECT

Prepared statements can also return rows.

```zen
Statement st = db.prepare(
  "SELECT id, name, age FROM users WHERE age >= ?"
)

st.bindInt(0, 20)
st.execute()

screen(st.columnCount())
screen(st.rowCount())

Row row = st.next()

while (row.valid()) {
  screen(row.getInt(0))
  screen(row.getString(1))
  screen(row.getInt(2))

  row.close()
  row = st.next()
}

st.close()
```

### Statement APIs

```text
ok() bool
bindInt(int index, int value) int
bindLong(int index, long value) int
bindDouble(int index, double value) int
bindString(int index, string value) int
bindAll(List<string> params) int
execute() int
next() Row
columnCount() int
rowCount() long
affectedRows() long
insertId() long
error() string
close() void
```

## Transactions

Use `begin()`, `commit()`, and `rollback()` for transactions.

```zen
if (db.begin() != 0) {
  screen(db.error())
}

if (db.query(
  "INSERT INTO users (name) VALUES ('Jishith')"
) != 0) {
  db.rollback()
} else {
  db.commit()
}
```

Rollback:

```zen
db.begin()
db.query("INSERT INTO users (name) VALUES ('Temporary')")
db.rollback()
```

Commit:

```zen
db.begin()
db.query("INSERT INTO users (name) VALUES ('Permanent')")
db.commit()
```

## Convenience `run()`

`run()` prepares a statement, binds all parameters as strings, executes it, and returns the statement.

```zen
Statement st = db.run(
  "INSERT INTO users (name, age) VALUES (?, ?)",
  ["Jishith", "21"]
)

if (!st.ok()) {
  screen(db.error())
} else {
  screen(st.affectedRows())
  st.close()
}
```

For typed parameters, use `prepare()` with the typed binding methods instead.

## Complete Example

```zen
import (
  Database,
  Result,
  Row,
  Statement,
  connect
) from "mariadb"

Database db = connect(
  "127.0.0.1",
  "root",
  "",
  "test",
  3306
)

if (!db.ok()) {
  screen(db.error())
  os.exit(1)
}

if (db.query(
  "CREATE TABLE IF NOT EXISTS users (" +
  "id INT AUTO_INCREMENT PRIMARY KEY, " +
  "name VARCHAR(50), " +
  "age INT)"
) != 0) {
  screen(db.error())
  db.close()
  os.exit(1)
}

Statement st = db.prepare(
  "INSERT INTO users (name, age) VALUES (?, ?)"
)

st.bindString(0, "Jishith")
st.bindInt(1, 21)

if (st.execute() != 0) {
  screen(st.error())
  st.close()
  db.close()
  os.exit(1)
}

screen(st.insertId())
st.close()

db.query("SELECT id, name, age FROM users")

Result r = db.result()

Row row = r.next()

while (row.valid()) {
  screen(row.getInt(0))
  screen(row.getString(1))
  screen(row.getInt(2))

  row.close()
  row = r.next()
}

r.close()
db.close()
```

## API Overview

```text
connect()
    ↓
Database
    ├── query()
    ├── result()
    │     └── Result
    │           └── Row
    │
    ├── prepare()
    │     └── Statement
    │           └── Row
    │
    ├── run()
    ├── begin()
    ├── commit()
    ├── rollback()
    └── close()
```

The package supports:

- MariaDB connections
- Raw SQL queries
- Result sets
- Row iteration
- Typed row getters
- SQL NULL checking
- Prepared statements
- Typed parameter binding
- String parameter binding
- Prepared result sets
- Transactions
- Affected-row counts
- Auto-increment IDs
- Database and statement errors
- Query Builder integration
- Explicit resource cleanup
