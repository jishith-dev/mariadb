# Installation

## Install Zen

Install Zen on your system first.

Then install the package:

```bash
zen install mariadb
```

## MariaDB Requirements

The `mariadb` Zen package uses the **MariaDB Connector/C** client library.

You need:

1. MariaDB Connector/C development files
2. A running MariaDB server when using the package

The package includes its native implementation:

```text
native/mariadb.c
```

Zen automatically compiles the native source for the current target and links it with the MariaDB client library.

You do not need to manually compile the native C source.

## Linux

Install MariaDB Connector/C development files using your distribution's package manager.

You may also need to install and start a MariaDB server if you are connecting to a local database.

## Android / Termux

Install the MariaDB client/development package available for Termux.

Make sure your MariaDB server is running before connecting to it.

## Windows

Install MariaDB Connector/C for Windows and make sure its headers and libraries are available to the compiler.

## macOS

Install MariaDB Connector/C using the available package manager or MariaDB distribution.

## Usage

After Zen and MariaDB Connector/C are installed, import the package:

```zen
import (
  Database,
  Result,
  Row,
  Statement,
  connect
) from "mariadb"
```

Then connect to MariaDB:

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
  os.exit(1)
}
```

Zen automatically compiles the package's `native/mariadb.c` for the current target and links it with the MariaDB client library.
