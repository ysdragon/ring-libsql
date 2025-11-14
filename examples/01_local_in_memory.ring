# Example 1: Local In-Memory Database
# This example demonstrates basic SQLite operations using an in-memory database

load "libsql.ring"

# Open in-memory database
db = new LibSQL {
	openExt(":memory:")
}
myConn = db.connect()

# Create a table and insert data
myConn.execute("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT, email TEXT)")
myConn.execute("INSERT INTO users (name, email) VALUES ('Alice', 'alice@example.com')")
myConn.execute("INSERT INTO users (name, email) VALUES ('Bob', 'bob@example.com')")

# Query data - Method 1: Fetch row by row
? "=== Fetching Rows One by One ==="
myRows = myConn.query("SELECT * FROM users")
while true
	myRow = myRows.fetchRow()
	if isNull(myRow) exit ok
	
	data = myRow.toList()
	? "ID: " + data[1] + ", Name: " + data[2] + ", Email: " + data[3]
end

# Query data - Method 2: Fetch all at once as associative array
? nl + "=== Fetching All Rows as Associative Array ==="
results = myConn.query("SELECT * FROM users").fetchAllAssoc()
for user in results
	? "User: " + user[2][2] + " (" + user[3][2] + ")"
next

# Cleanup
myConn.disconnect()
db.close()

? nl + "Example completed successfully!"