<div align="center">

# Ring LibSQL

[ring]: https://img.shields.io/badge/Made_with_❤️_for-Ring-2D54CB?style=for-the-badge
[license]: https://img.shields.io/github/license/ysdragon/ring-libsql?style=for-the-badge&logo=opensourcehardware&label=License&logoColor=C0CAF5&labelColor=414868&color=8c73cc

[![][ring]](https://ring-lang.net/)
[![][license]](https://github.com/ysdragon/ring-libsql/blob/master/LICENSE)

**LibSQL client library for Ring with SQLite compatibility and support for local, remote, and embedded replica databases.**

</div>

## ✨ Features

- Full SQLite-compatible database operations
- Local file-based databases (`:memory:` or file paths)
- Remote database access with authentication
- Embedded replica support for offline-first applications with sync capabilities
- Prepared statements with parameter binding (int, float, string, blob, null)
- Encryption support for both local and remote databases
- WebPKI support for secure HTTPS connections
- Transaction support and connection management
- Cross-platform support (Windows, Linux, macOS, FreeBSD)

> [!NOTE]
> This extension/library is built on top of the experimental c bindings of [libsql](https://github.com/tursodatabase/libsql).

## 📦 Installation

This package can be installed using the Ring Package Manager (**RingPM**):

```
ringpm install ring-libsql from ysdragon
```

## 💡 Usage

First, load the library in your Ring script:

```ring
load "libsql.ring"
```

### Local Database (In-Memory)

```ring
load "libsql.ring"

# Open in-memory database
db = new LibSQL
db.openExt(":memory:")
conn = db.connect()

# Create a table and insert data
conn.execute("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT, email TEXT)")
conn.execute("INSERT INTO users (name, email) VALUES ('Alice', 'alice@example.com')")
conn.execute("INSERT INTO users (name, email) VALUES ('Bob', 'bob@example.com')")

# Query data
rows = conn.query("SELECT * FROM users")
while True
	row = rows.fetchRow()
	if isNull(row)
		exit
	ok
	
	data = row.toList()
	? "ID: " + data[1] + ", Name: " + data[2] + ", Email: " + data[3]
end

# Or fetch all at once as associative array
results = conn.query("SELECT * FROM users").fetchAllAssoc()
for user in results
	? user  # Prints [["id", 1], ["name", "Alice"], ["email", "alice@example.com"]]
next

# Cleanup
conn.disconnect()
db.close()
```

### Local Database (File)

```ring
load "libsql.ring"

# Open file-based database
db = new LibSQL
db.openFile("mydata.db")
conn = db.connect()

# Use the database
conn.execute("CREATE TABLE IF NOT EXISTS products (id INTEGER, name TEXT, price REAL)")
conn.execute("INSERT INTO products VALUES (1, 'Laptop', 999.99)")

# Cleanup
conn.disconnect()
db.close()
```

### Remote Database (Turso or Self-Hosted)

```ring
load "libsql.ring"

# Option 1: Connect to Turso hosted database
db = new LibSQL
db.openRemote("libsql://your-database.turso.io", "your-auth-token")
conn = db.connect()

# Option 2: Connect to your self-hosted LibSQL server
# Can be localhost, VPS, cloud server, or any remote machine
# db = new LibSQL
# db.openRemote("http://your-server.com:8080", "optional-auth-token")
# conn = db.connect()

# Execute queries
conn.execute("INSERT INTO logs (message, timestamp) VALUES ('Hello from Ring!', datetime('now'))")

# Query remote data
rows = conn.query("SELECT * FROM logs ORDER BY timestamp DESC LIMIT 10")
logs = rows.fetchAllAssoc()
for log in logs
	? log
next

# Cleanup
conn.disconnect()
db.close()
```

### Embedded Replica (Offline-First with Sync)

```ring
load "libsql.ring"

# Open embedded replica with sync configuration
config = [
	:db_path = "local_replica.db",
	:primary_url = "libsql://your-database.turso.io",
	:auth_token = "your-auth-token",
	:read_your_writes = 1,
	:sync_interval = 60  # Auto-sync every 60 seconds
]

db = new LibSQL
db.openSyncWithConfig(config)
conn = db.connect()

# Work with local replica (works offline)
conn.execute("INSERT INTO tasks (title, done) VALUES ('Learn Ring', 0)")
conn.execute("INSERT INTO tasks (title, done) VALUES ('Build App', 0)")

# Manually sync with remote
db.sync()

# Get sync statistics
syncInfo = db.sync2()
? "Synced frames: " + syncInfo[2]

# Cleanup
conn.disconnect()
db.close()
```

### Prepared Statements with Parameter Binding

```ring
load "libsql.ring"

db = new LibSQL
db.openExt(":memory:")
conn = db.connect()

conn.execute("CREATE TABLE users (id INTEGER, name TEXT, score REAL, active INTEGER)")

# Prepare statement
stmt = conn.prepare("INSERT INTO users (id, name, score, active) VALUES (?, ?, ?, ?)")

# Method 1: Bind individually with type-specific methods
stmt.bindInt(1, 1)
    .bindString(2, "Alice")
    .bindFloat(3, 95.5)
    .bindInt(4, 1)
    .execute()

# Method 2: Bind all parameters at once (auto-detects types)
stmt.reset()
    .bindParams([2, "Bob", 87.3, 1])
    .execute()

# Method 3: Use smart bind (handles nulls and auto-types)
stmt.reset()
    .bind(1, 3)
    .bind(2, "Charlie")
    .bind(3, 92.0)
    .bind(4, null)  # NULL value
    .execute()

# Query with prepared statements
queryStmt = conn.prepare("SELECT * FROM users WHERE score > ?")
queryStmt.bindFloat(1, 90.0)
results = queryStmt.query().fetchAllAssoc()

for user in results
	? user
next

# Cleanup
conn.disconnect()
db.close()
```

### Database Encryption

```ring
load "libsql.ring"

# Embedded replica with encryption (syncs with remote server)
db = new LibSQL
db.openSync(
	"local_encrypted.db",
	"libsql://your-database.turso.io",  # Remote URL required
	"your-auth-token",
	1,   # read_your_writes
	"my-encryption-key-32-bytes-long"
)

conn = db.connect()
conn.execute("CREATE TABLE IF NOT EXISTS secrets (id INTEGER, data TEXT)")
conn.execute("INSERT INTO secrets VALUES (1, 'Top Secret Data')")

# Sync encrypted data with remote
db.sync()

# Cleanup
conn.disconnect()
db.close()
```

### Working with Different Data Types

```ring
load "libsql.ring"

db = new LibSQL
db.openExt(":memory:")
conn = db.connect()

conn.execute("CREATE TABLE mixed_types (
	id INTEGER,
	name TEXT,
	price REAL,
	data BLOB,
	nullable TEXT
)")

# Insert with different types
stmt = conn.prepare("INSERT INTO mixed_types VALUES (?, ?, ?, ?, ?)")
stmt.bindInt(1, 1)
    .bindString(2, "Product")
    .bindFloat(3, 29.99)
    .bindBlob(4, "Binary Data Here")
    .bindNull(5)
    .execute()

# Fetch and access by type
rows = conn.query("SELECT * FROM mixed_types")
row = rows.fetchRow()

? "ID (int): " + row.getIntValue(1)
? "Name (string): " + row.getStringValue(2)
? "Price (float): " + row.getFloatValue(3)
? "Data (blob): " + row.getBlobValue(4)

# Cleanup
conn.disconnect()
db.close()
```

## 📚 API Reference

### LibSQL Class (Database)

The main database class for opening and managing database connections.

#### Opening Databases

- **`openFile(path)`** - Open local file database
- **`openExt(path)`** - Open database (`:memory:`, file path, or URL)
- **`openRemote(url, auth_token)`** - Open remote database
- **`openRemoteWithWebPKI(url, auth_token)`** - Open remote with WebPKI
- **`openRemoteWithEncryption(url, auth_token, key)`** - Open remote with encryption
- **`openSync(db_path, url, token, read_your_writes, key)`** - Open embedded replica
- **`openSyncWithWebPKI(db_path, url, token, read_your_writes, key)`** - Open embedded replica with WebPKI
- **`openSyncWithConfig(config_list)`** - Open with configuration list

#### Database Operations

- **`connect()`** - Create connection, returns LibSQLConnection object
- **`sync()`** - Manually sync embedded replica with remote
- **`sync2()`** - Sync and return frame statistics `[frame_no, frames_synced]`
- **`close()`** - Close database

### LibSQLConnection Class (Connection)

Represents an active database connection for executing queries.

#### Query Execution

- **`execute(sql)`** - Execute SQL without returning rows
- **`query(sql)`** - Execute query, returns LibSQLRows object
- **`prepare(sql)`** - Prepare statement, returns LibSQLStatement object
- **`changes()`** - Get number of rows affected by last operation
- **`lastInsertRowID()`** - Get last inserted row ID

#### Connection Management

- **`disconnect()`** - Close connection
- **`reset()`** - Reset connection state
- **`loadExtension(path, entry_point)`** - Load SQLite extension
- **`setReservedBytes(bytes)`** - Set reserved bytes for encryption
- **`getReservedBytes()`** - Get reserved bytes

### LibSQLStatement Class (Prepared Statement)

Prepared statement with parameter binding.

#### Parameter Binding

- **`bindInt(index, value)`** - Bind integer (1-based index)
- **`bindFloat(index, value)`** - Bind float (1-based index)
- **`bindString(index, value)`** - Bind string (1-based index)
- **`bindBlob(index, value)`** - Bind blob (1-based index)
- **`bindNull(index)`** - Bind NULL (1-based index)
- **`bind(index, value)`** - Smart bind (auto-detects type, 1-based index)
- **`bindParams(params_list)`** - Bind all parameters from list (1-based)

#### Execution

- **`execute()`** - Execute statement without returning rows
- **`query()`** - Execute statement, returns LibSQLRows object
- **`reset()`** - Reset statement for reuse

### LibSQLRows Class (Result Set)

Represents query results.

#### Column Information

- **`columnCount()`** - Get number of columns
- **`columnName(index)`** - Get column name (1-based index)
- **`columnNames()`** - Get list of all column names

#### Fetching Rows

- **`fetchRow()`** - Fetch next row, returns LibSQLRow or null
- **`fetchAll()`** - Fetch all rows as list of lists
- **`fetchAllAssoc()`** - Fetch all rows as associative arrays

### LibSQLRow Class (Row Data)

Represents a single result row.

#### Type-Specific Access

- **`getIntValue(index)`** - Get integer value (1-based index)
- **`getFloatValue(index)`** - Get float value
- **`getStringValue(index)`** - Get string value
- **`getBlobValue(index)`** - Get blob value
- **`getType(index)`** - Get column type constant
- **`getValue(index)`** - Get value with automatic type conversion

#### Row Conversion

- **`toList()`** - Convert row to list of values
- **`toAssoc()`** - Convert row to associative array `[["col", val], ...]`

### Constants

- **`LIBSQL_INT`** - Integer column type
- **`LIBSQL_FLOAT`** - Float column type
- **`LIBSQL_TEXT`** - Text column type
- **`LIBSQL_BLOB`** - Blob column type
- **`LIBSQL_NULL`** - NULL column type

### Low-Level C Functions

For advanced users, all underlying C functions are also available:
`libsql_open_*`, `libsql_connect`, `libsql_execute`, `libsql_query`, `libsql_prepare`,
`libsql_bind_*`, `libsql_next_row`, `libsql_get_*`, etc.

## 🛠️ Development

If you wish to contribute to the development of Ring LibSQL or build it from the source, follow these steps.

### Prerequisites

-   **CMake**: Version 3.16 or higher
-   **C Compiler**: A C compiler compatible with your platform (e.g., GCC, Clang, MSVC)
-   **Rust & Cargo**: Required to build libsql from source
-   **Git**: For cloning repositories
-   **[Ring](https://ring-lang.net/) Source Code**: Ring language source code must be available on your machine

### Build Steps

1.  **Clone the Repository:**
	```sh
	git clone https://github.com/ysdragon/ring-libsql.git
	cd ring-libsql
	```

	> **Note**
	> If you installed the library via RingPM, you can skip this step.

2.  **Set the `RING` Environment Variable:**
	This variable must point to the root directory of the Ring language source code.

	-   **Windows (Command Prompt):**
		```cmd
		set RING=X:\path\to\ring
		```
	-   **Windows (PowerShell):**
		```powershell
		$env:RING = "X:\path\to\ring"
		```
	-   **Unix-like Systems (Linux, macOS or FreeBSD):**
		```bash
		export RING=/path/to/ring
		```

3.  **Configure with CMake:**
	Create a build directory and run CMake from within it. CMake will automatically:
	- Clone libsql from GitHub if not found
	- Build libsql using Cargo
	
	```sh
	mkdir build
	cd build
	cmake .. -DCMAKE_BUILD_TYPE=Release
	```

4.  **Build the Project:**
	Compile the source code using the build toolchain configured by CMake.
	```sh
	cmake --build .
	```

	The compiled library will be available in the `lib/<os>/<arch>` directory.

## 🤝 Contributing

Contributions are always welcome! If you have suggestions for improvements or have identified a bug, please feel free to open an issue or submit a pull request.

## 📄 License

This project is licensed under the MIT License. See the [`LICENSE`](LICENSE) file for more details.