# Example 3: Prepared Statements with Parameter Binding
# This example demonstrates various ways to use prepared statements

load "libsql.ring"

myDB = new LibSQL {
	openExt(":memory:")
}
myConn = myDB.connect()

myConn.execute("CREATE TABLE users (id INTEGER, name TEXT, score REAL, active INTEGER)")

# Prepare statement
myStmt = myConn.prepare("INSERT INTO users (id, name, score, active) VALUES (?, ?, ?, ?)")

# Method 1: Bind individually with type-specific methods
? "=== Method 1: Type-specific binding ==="
myStmt.bindInt(1, 1)
	.bindString(2, "Alice")
	.bindFloat(3, 95.5)
	.bindInt(4, 1)
	.execute()
? "Inserted Alice"

# Method 2: Bind all parameters at once (auto-detects types)
? nl + "=== Method 2: Bind all parameters at once ==="
myStmt.reset()
	.bindParams([2, "Bob", 87.3, 1])
	.execute()
? "Inserted Bob"

# Method 3: Use smart bind (handles nulls and auto-types)
? nl + "=== Method 3: Smart bind with NULL ==="
myStmt.reset()
	.bind(1, 3)
	.bind(2, "Charlie")
	.bind(3, 92.0)
	.bind(4, null)  # NULL value
	.execute()
? "Inserted Charlie with NULL active status"

# Query with prepared statements
? nl + "=== Query with Prepared Statement ==="
queryStmt = myConn.prepare("SELECT * FROM users WHERE score > ?")
queryStmt.bindFloat(1, 90.0)
results = queryStmt.query().fetchAllAssoc()

? "Users with score > 90:"
for user in results
	? "  - " + user[2][2] + ": " + user[3][2]
next

# Cleanup
myConn.disconnect()
myDB.close()

? nl + "Example completed successfully!"