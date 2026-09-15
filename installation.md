# Installation

## Install Zen

Install Zen on your system first.

Then install the package:

```bash
zen install mariadb
```

## Install MariaDB

The `mariadb` Zen package uses the MariaDB C client library.

You must have **MariaDB Connector/C** installed on your system before using this package.

The package provides the native implementation:

```text
native/mysql.c
```

Zen automatically compiles this C source into a target-specific `.o` file during compilation.

You do not need to manually compile `mysql.c`.

### Linux

Install the MariaDB Connector/C development package using your distribution's package manager.

### Android / Termux

Install the MariaDB client/development package available for Termux.

### Windows

Install MariaDB Connector/C for Windows and make sure its headers and libraries are available to the compiler.

### macOS

Install MariaDB Connector/C using the available package manager or MariaDB distribution.

## Build

After Zen and MariaDB Connector/C are installed, simply use the package normally:

```zen
import (
  Database,
  connect
) from "mariadb"

Database db = connect(
  "127.0.0.1",
  "root",
  "",
  "test",
  3306
)
```

Zen automatically compiles the package's `native/mysql.c` for the current target and links it with the MariaDB client library.
