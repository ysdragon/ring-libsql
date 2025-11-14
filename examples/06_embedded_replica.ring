# Example 6: Embedded Replica (Offline-First with Sync)
# This example demonstrates using embedded replica for offline-first applications
# NOTE: You need to replace the URL and auth token with your actual Turso credentials

load "libsql.ring"

# IMPORTANT: Replace these with your actual Turso database credentials
remoteURL = "libsql://your-database.turso.io"
authToken = "your-auth-token-here"

? "=== Embedded Replica with Sync Configuration ==="
? "Note: This example requires valid Turso credentials"
? "Get yours at: https://turso.tech"
? ""

# Uncomment the following code after replacing with your credentials:
/*
# Open embedded replica with sync configuration
config = [
	:db_path = "local_replica.db",
	:primary_url = remoteURL,
	:auth_token = authToken,
	:read_your_writes = 1,
	:sync_interval = 60  # Auto-sync every 60 seconds
]

myDB = new LibSQL {
	openSyncWithConfig(config)
}
myConn = myDB.connect()

# Create table
myConn.execute("CREATE TABLE IF NOT EXISTS tasks (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	title TEXT,
	done INTEGER
)")

# Work with local replica (works offline)
? "=== Adding Tasks ==="
myConn.execute("INSERT INTO tasks (title, done) VALUES ('Learn Ring', 0)")
myConn.execute("INSERT INTO tasks (title, done) VALUES ('Build App', 0)")
myConn.execute("INSERT INTO tasks (title, done) VALUES ('Deploy to Production', 0)")
? "Tasks added to local replica"

# Manually sync with remote
? nl + "=== Syncing with Remote ==="
myDB.sync()
? "Manual sync completed"

# Get sync statistics
syncInfo = myDB.sync2()
? "Frame number: " + syncInfo[1]
? "Frames synced: " + syncInfo[2]

# Query data from local replica
? nl + "=== Current Tasks ==="
myRows = myConn.query("SELECT * FROM tasks")
tasks = myRows.fetchAllAssoc()

for task in tasks
	if task[3][2] = 1
		status = "[DONE]"
	else
		status = "[ ]"
	ok
	? status + " " + task[2][2]
next

# Cleanup
myConn.disconnect()
myDB.close()

? nl + "Example completed successfully!"
? "Local replica saved to: local_replica.db"
*/

? "Please configure your Turso credentials to run this example."