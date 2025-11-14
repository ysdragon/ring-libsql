# Example 4: Working with Different Data Types
# This example demonstrates handling various SQLite data types

load "libsql.ring"

myDB = new LibSQL {
	openExt(":memory:")
}
myConn = myDB.connect()

myConn.execute("CREATE TABLE mixed_types (
	id INTEGER,
	name TEXT,
	price REAL,
	data BLOB,
	nullable TEXT
)")

# Insert with different types
? "=== Inserting Different Data Types ==="
myStmt = myConn.prepare("INSERT INTO mixed_types VALUES (?, ?, ?, ?, ?)")
myStmt.bindInt(1, 1)
	 .bindString(2, "Product")
	 .bindFloat(3, 29.99)
	 .bindBlob(4, "Binary Data Here")
	 .bindNull(5)
	 .execute()
? "Data inserted successfully"

# Fetch and access by type
? nl + "=== Fetching and Accessing Values by Type ==="
myRows = myConn.query("SELECT * FROM mixed_types")
myRow = myRows.fetchRow()

if not isNull(myRow)
	? "ID (int): " + myRow.getIntValue(1)
	? "Name (string): " + myRow.getStringValue(2)
	? "Price (float): " + myRow.getFloatValue(3)
	? "Data (blob): " + myRow.getBlobValue(4)
	nullableVal = myRow.getValue(5)
	if isNull(nullableVal)
		? "Nullable: NULL"
	else
		? "Nullable: " + nullableVal
	ok
ok

# Get column information
? nl + "=== Column Information ==="
myRows2 = myConn.query("SELECT * FROM mixed_types")
colNames = myRows2.columnNames()
? "Columns: " + list2str(colNames)
? "Column count: " + myRows2.columnCount()

# Demonstrate type checking
? nl + "=== Type Checking ==="
myRows3 = myConn.query("SELECT * FROM mixed_types")
myRow3 = myRows3.fetchRow()
if not isNull(myRow3)
	for i = 1 to 5
		colType = myRow3.getType(i)
		typeName = ""
		switch colType
			on LIBSQL_INT
				typeName = "INTEGER"
			on LIBSQL_FLOAT
				typeName = "FLOAT"
			on LIBSQL_TEXT
				typeName = "TEXT"
			on LIBSQL_BLOB
				typeName = "BLOB"
			on LIBSQL_NULL
				typeName = "NULL"
		off
		? "Column " + i + " type: " + typeName
	next
ok

# Cleanup
myConn.disconnect()
myDB.close()

? nl + "Example completed successfully!"