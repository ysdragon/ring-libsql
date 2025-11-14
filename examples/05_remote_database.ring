# Example 5: Remote Database (Turso or Self-Hosted)
# This example demonstrates connecting to a remote database
# Options: Turso hosted, or self-hosted on localhost/VPS/cloud

load "libsql.ring"

# Option 1: Turso hosted database
# Get credentials from: https://turso.tech
remoteURL = "libsql://your-database.turso.io"
authToken = "your-auth-token-here"

# Option 2: Self-hosted on localhost (development)
# remoteURL = "http://localhost:8080"
# authToken = ""  # No auth needed for local

# Option 3: Self-hosted on VPS/Cloud (production)
# remoteURL = "http://your-server.com:8080"
# authToken = "your-secret-token"  # Auth recommended for production

? "=== Remote Database Connection Options ==="
? "1. Turso hosted: libsql://your-database.turso.io"
? "2. Self-hosted local: http://localhost:8080 (no auth)"
? "3. Self-hosted remote: http://your-server.com:8080 (with auth)"
? ""
? "Deploy your own server: docker run -p 8080:8080 ghcr.io/tursodatabase/libsql-server"
? ""

# Uncomment the following code after replacing with your credentials:
/*
myDB = new LibSQL {
	openRemote(remoteURL, authToken)
}
myConn = myDB.connect()

# Create table if it doesn't exist
myConn.execute("CREATE TABLE IF NOT EXISTS logs (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	message TEXT,
	timestamp TEXT
)")

# Insert a log entry
myConn.execute("INSERT INTO logs (message, timestamp) VALUES ('Hello from Ring!', datetime('now'))")
? "Log entry inserted"

# Query recent logs
? nl + "=== Recent Logs ==="
myRows = myConn.query("SELECT * FROM logs ORDER BY timestamp DESC LIMIT 10")
logs = myRows.fetchAllAssoc()

for log in logs
	? "ID: " + log[1][2] + " | Message: " + log[2][2] + " | Time: " + log[3][2]
next

# Cleanup
myConn.disconnect()
myDB.close()

? nl + "Example completed successfully!"
*/

? "Please configure your Turso credentials to run this example."