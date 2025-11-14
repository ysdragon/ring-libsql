# Ring LibSQL Library

load "libsql.rh"

# Helper function to enable internal tracing
func libsql_enable_tracing
	return libsql_enable_internal_tracing()

# Main LibSQL Database Class
class LibSQL
	self.db
	self.conn

	func openFile path
		tempDB = libsql_open_file(path)
		if isNull(tempDB)
			raise("Failed to open database file: " + path)
		ok
		self.db = tempDB

	func openExt path
		tempDB = libsql_open_ext(path)
		if isNull(tempDB)
			raise("Failed to open database: " + path)
		ok
		self.db = tempDB

	func openRemote url, auth_token
		tempDB = libsql_open_remote(url, auth_token)
		if isNull(tempDB)
			raise("Failed to open remote database: " + url)
		ok
		self.db = tempDB

	func openRemoteWithWebPKI url, auth_token
		tempDB = libsql_open_remote_with_webpki(url, auth_token)
		if isNull(tempDB)
			raise("Failed to open remote database with WebPKI: " + url)
		ok
		self.db = tempDB

	func openRemoteWithEncryption url, auth_token, encryption_key
		tempDB = libsql_open_remote_with_remote_encryption(url, auth_token, encryption_key)
		if isNull(tempDB)
			raise("Failed to open remote database with encryption")
		ok
		self.db = tempDB

	func openSync db_path, primary_url, auth_token, read_your_writes, encryption_key
		tempDB = libsql_open_sync(db_path, primary_url, auth_token, read_your_writes, encryption_key)
		if isNull(tempDB)
			raise("Failed to open sync database")
		ok
		self.db = tempDB

	func openSyncWithWebPKI db_path, primary_url, auth_token, read_your_writes, encryption_key
		tempDB = libsql_open_sync_with_webpki(db_path, primary_url, auth_token, read_your_writes, encryption_key)
		if isNull(tempDB)
			raise("Failed to open sync database with WebPKI")
		ok
		self.db = tempDB

	func openSyncWithConfig config
		tempDB = libsql_open_sync_with_config(config)
		if isNull(tempDB)
			raise("Failed to open sync database with config")
		ok
		self.db = tempDB

	func sync
		libsql_sync(self.db)

	func sync2
		return libsql_sync2(self.db)

	func connect
		self.conn = libsql_connect(self.db)
		if isNull(self.conn)
			raise("Failed to connect to database")
		ok
		return new LibSQLConnection(self.conn)

	func close
		if not isNull(self.db)
			libsql_close(self.db)
			self.db = null
		ok

class LibSQLConnection
	self.conn

	func init pConn
		self.conn = pConn

	func execute sql
		libsql_execute(conn, sql)
		return self

	func query sql
		rows = libsql_query(conn, sql)
		if isNull(rows)
			raise("Query failed")
		ok
		return new LibSQLRows(rows)

	func prepare sql
		stmt = libsql_prepare(conn, sql)
		if isNull(stmt)
			raise("Failed to prepare statement")
		ok
		return new LibSQLStatement(stmt, self)

	func changes
		return libsql_changes(conn)

	func lastInsertRowID
		return libsql_last_insert_rowid(conn)

	func loadExtension path, entry_point
		libsql_load_extension(conn, path, entry_point)
		return self

	func setReservedBytes bytes
		libsql_set_reserved_bytes(conn, bytes)
		return self

	func getReservedBytes
		return libsql_get_reserved_bytes(conn)

	func reset
		libsql_reset(conn)
		return self

	func disconnect
		if not isNull(conn)
			libsql_disconnect(conn)
			conn = null
		ok

	func getPointer
		return conn

class LibSQLStatement
	self.stmt
	self.conn

	func init pStmt, pConn
		self.stmt = pStmt
		self.conn = pConn

	func bindInt index, value
		libsql_bind_int(stmt, index, value)
		return self

	func bindFloat index, value
		libsql_bind_float(stmt, index, value)
		return self

	func bindString index, value
		libsql_bind_string(stmt, index, value)
		return self

	func bindNull index
		libsql_bind_null(stmt, index)
		return self

	func bindBlob index, value
		libsql_bind_blob(stmt, index, value)
		return self

	func bind index, value
		if isNull(value)
			return bindNull(index)
		but isnumber(value)
			if value = floor(value)
				return bindInt(index, value)
			else
				return bindFloat(index, value)
			ok
		but isstring(value)
			return bindString(index, value)
		else
			raise("Unsupported bind value type")
		ok

	func bindParams params
		for i = 1 to len(params)
			bind(i, params[i])
		next
		return self

	func execute
		libsql_execute_stmt(stmt)
		return self

	func query
		rows = libsql_query_stmt(stmt)
		if isNull(rows)
			raise("Query failed")
		ok
		return new LibSQLRows(rows)

	func reset
		libsql_reset_stmt(stmt)
		return self

class LibSQLRows
	self.rows = null
	self.current_row = null

	func init pRows
		self.rows = pRows
		self.current_row = null

	func columnCount
		return libsql_column_count(rows)

	func columnName index
		return libsql_column_name(rows, index - 1)

	func columnNames
		names = []
		count = columnCount()
		for i = 1 to count
			add(names, columnName(i))
		next
		return names

	func fetchRow
		current_row = libsql_next_row(rows)
		if isNull(current_row)
			return null
		ok
		return new LibSQLRow(rows, current_row)

	func fetchAll
		results = []
		while true
			row = fetchRow()
			if isNull(row)
				exit
			ok
			add(results, row.toList())
		end
		return results

	func fetchAllAssoc
		results = []
		while true
			row = fetchRow()
			if isNull(row)
				exit
			ok
			add(results, row.toAssoc())
		end
		return results

class LibSQLRow
	self.rows = null
	self.row = null

	func init pRows, pRow
		self.rows = pRows
		self.row = pRow

	func getIntValue index
		return libsql_get_int(row, index - 1)

	func getFloatValue index
		return libsql_get_float(row, index - 1)

	func getStringValue index
		return libsql_get_string(row, index - 1)

	func getBlobValue index
		return libsql_get_blob(row, index - 1)

	func getType index
		return libsql_column_type(rows, row, index - 1)

	func getValue index
		type = getType(index)
		switch type
		on LIBSQL_INT
			return getIntValue(index)
		on LIBSQL_FLOAT
			return getFloatValue(index)
		on LIBSQL_TEXT
			return getStringValue(index)
		on LIBSQL_BLOB
			return getBlobValue(index)
		on LIBSQL_NULL
			return null
		other
			return null
		off

	func toList
		result = []
		count = libsql_column_count(rows)
		for i = 1 to count
			add(result, getValue(i))
		next
		return result

	func toAssoc
		result = []
		count = libsql_column_count(rows)
		for i = 1 to count
			name = libsql_column_name(rows, i - 1)
			value = getValue(i)
			add(result, [name, value])
		next
		return result