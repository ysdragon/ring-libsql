# Example 2: Local File-Based Database
# This example demonstrates using a persistent file-based SQLite database

load "libsql.ring"

# Open file-based database
myDB = new LibSQL {
	openFile("products.db")
}
myConn = myDB.connect()

# Create table if it doesn't exist
myConn.execute("CREATE TABLE IF NOT EXISTS products (id INTEGER, name TEXT, price REAL)")

# Insert some data
myConn.execute("INSERT INTO products VALUES (1, 'Laptop', 999.99)")
myConn.execute("INSERT INTO products VALUES (2, 'Mouse', 29.99)")
myConn.execute("INSERT INTO products VALUES (3, 'Keyboard', 89.99)")

? "=== Products Inserted ==="
? "Rows affected: " + myConn.changes()
? "Last inserted ID: " + myConn.lastInsertRowID()

# Query and display all products
? nl + "=== All Products ==="
myRows = myConn.query("SELECT * FROM products")
while True
	myRow = myRows.fetchRow()
	if isNull(myRow)
		exit 
	ok
	
	? "ID: " + myRow.getIntValue(1) + 
	  ", Name: " + myRow.getStringValue(2) + 
	  ", Price: $" + myRow.getFloatValue(3)
end

# Cleanup
myConn.disconnect()
myDB.close()

? nl + "Database saved to products.db"
? "Example completed successfully!"