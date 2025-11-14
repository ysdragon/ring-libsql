# Example 7: Database Encryption
# This example demonstrates using encryption with embedded replicas
# NOTE: You need to replace the URL and auth token with your actual Turso credentials

load "libsql.ring"

# IMPORTANT: Replace these with your actual Turso database credentials
remoteURL = "libsql://your-database.turso.io"
authToken = "your-auth-token-here"
encryptionKey = "my-encryption-key-32-bytes-long"

? "=== Embedded Replica with Encryption ==="
? "Note: This example requires valid Turso credentials"
? "Get yours at: https://turso.tech"
? ""

# Uncomment the following code after replacing with your credentials:
/*
# Embedded replica with encryption (syncs with remote server)
myDB = new LibSQL {
	openSync(
		"local_encrypted.db",
		remoteURL,  # Remote URL required
		authToken,
		1,   # read_your_writes
		encryptionKey
	)
}

myConn = myDB.connect()

# Create table
myConn.execute("CREATE TABLE IF NOT EXISTS secrets (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	data TEXT,
	created_at TEXT
)")

# Insert encrypted data
? "=== Inserting Sensitive Data ==="
myConn.execute("INSERT INTO secrets (data, created_at) VALUES ('Top Secret Data', datetime('now'))")
myConn.execute("INSERT INTO secrets (data, created_at) VALUES ('Confidential Information', datetime('now'))")
? "Data inserted into encrypted database"

# Sync encrypted data with remote
? nl + "=== Syncing Encrypted Data ==="
myDB.sync()
? "Encrypted data synced with remote server"

# Query encrypted data
? nl + "=== Encrypted Data ==="
myRows = myConn.query("SELECT * FROM secrets")
secrets = myRows.fetchAllAssoc()

for secret in secrets
	? "ID: " + secret[1][2] + " | Data: " + secret[2][2] + " | Created: " + secret[3][2]
next

# Cleanup
myConn.disconnect()
myDB.close()

? nl + "Example completed successfully!"
? "Encrypted database saved to: local_encrypted.db"
*/

? "Please configure your Turso credentials to run this example."
? ""
? "Note: For local-only databases without a remote server,"
? "      use openFile() or openExt() instead."
? "      The encryption parameter in openSync() is specifically"
? "      for embedded replicas that sync with remote Turso databases."