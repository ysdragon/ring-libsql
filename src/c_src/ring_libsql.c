#include "libsql.h"
#include "ring.h"

#define RING_POINTER_LIBSQL_DB "LIBSQL_DATABASE"
#define RING_POINTER_LIBSQL_CONN "LIBSQL_CONNECTION"
#define RING_POINTER_LIBSQL_STMT "LIBSQL_STATEMENT"
#define RING_POINTER_LIBSQL_ROWS "LIBSQL_ROWS"
#define RING_POINTER_LIBSQL_ROW "LIBSQL_ROW"
#define RING_POINTER_LIBSQL_ROWS_FUTURE "LIBSQL_ROWS_FUTURE"

#define LIBSQL_CHECK_OK(result, err_msg)                                                                               \
	if ((result) != 0)                                                                                                 \
	{                                                                                                                  \
		RING_API_ERROR(err_msg);                                                                                       \
		return;                                                                                                        \
	}

/* Helper Functions */

static char *ring_string_lower(char *cStr)
{
	unsigned int x, nLen;
	nLen = strlen(cStr);
	for (x = 0; x < nLen; x++)
	{
		if (isalpha((unsigned char)cStr[x]))
		{
			cStr[x] = tolower(cStr[x]);
		}
	}
	return cStr;
}

/* Free Functions for Managed Pointers */

void ring_libsql_free_db(void *pState, void *pPtr)
{
	if (pPtr)
	{
		libsql_close((libsql_database_t)pPtr);
	}
}

void ring_libsql_free_conn(void *pState, void *pPtr)
{
	if (pPtr)
	{
		libsql_disconnect((libsql_connection_t)pPtr);
	}
}

void ring_libsql_free_stmt(void *pState, void *pPtr)
{
	if (pPtr)
	{
		libsql_free_stmt((libsql_stmt_t)pPtr);
	}
}

void ring_libsql_free_rows(void *pState, void *pPtr)
{
	if (pPtr)
	{
		libsql_free_rows((libsql_rows_t)pPtr);
	}
}

void ring_libsql_free_row(void *pState, void *pPtr)
{
	if (pPtr)
	{
		libsql_free_row((libsql_row_t)pPtr);
	}
}

/* Functions */

RING_FUNC(ring_libsql_enable_internal_tracing)
{
	RING_API_RETNUMBER(libsql_enable_internal_tracing());
}

RING_FUNC(ring_libsql_sync)
{
	const char *err_msg;
	if (RING_API_PARACOUNT != 1)
	{
		RING_API_ERROR(RING_API_MISS1PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_database_t db = (libsql_database_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_DB);
	int rc = libsql_sync(db, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
}

RING_FUNC(ring_libsql_sync2)
{
	const char *err_msg;
	replicated repl;
	if (RING_API_PARACOUNT != 1)
	{
		RING_API_ERROR(RING_API_MISS1PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_database_t db = (libsql_database_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_DB);
	int rc = libsql_sync2(db, &repl, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
	List *pList = RING_API_NEWLIST;
	ring_list_adddouble(pList, repl.frame_no);
	ring_list_adddouble(pList, repl.frames_synced);
	RING_API_RETLIST(pList);
}

RING_FUNC(ring_libsql_open_sync)
{
	const char *err_msg;
	libsql_database_t db;
	if (RING_API_PARACOUNT != 5)
	{
		RING_API_ERROR("Expected 5 parameters: db_path, primary_url, "
					   "auth_token, read_your_writes, encryption_key");
		return;
	}
	if (!RING_API_ISSTRING(1) || !RING_API_ISSTRING(2) || !RING_API_ISSTRING(3) || !RING_API_ISNUMBER(4) ||
		!RING_API_ISSTRING(5))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	int rc = libsql_open_sync(RING_API_GETSTRING(1), RING_API_GETSTRING(2), RING_API_GETSTRING(3),
							  (char)RING_API_GETNUMBER(4), RING_API_GETSTRING(5), &db, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
	RING_API_RETMANAGEDCPOINTER(db, RING_POINTER_LIBSQL_DB, ring_libsql_free_db);
}

RING_FUNC(ring_libsql_open_sync_with_webpki)
{
	const char *err_msg;
	libsql_database_t db;
	if (RING_API_PARACOUNT != 5)
	{
		RING_API_ERROR("Expected 5 parameters: db_path, primary_url, "
					   "auth_token, read_your_writes, encryption_key");
		return;
	}
	if (!RING_API_ISSTRING(1) || !RING_API_ISSTRING(2) || !RING_API_ISSTRING(3) || !RING_API_ISNUMBER(4) ||
		!RING_API_ISSTRING(5))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	int rc = libsql_open_sync_with_webpki(RING_API_GETSTRING(1), RING_API_GETSTRING(2), RING_API_GETSTRING(3),
										  (char)RING_API_GETNUMBER(4), RING_API_GETSTRING(5), &db, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
	RING_API_RETMANAGEDCPOINTER(db, RING_POINTER_LIBSQL_DB, ring_libsql_free_db);
}

RING_FUNC(ring_libsql_open_sync_with_config)
{
	if (RING_API_PARACOUNT != 1 || !RING_API_ISLIST(1))
	{
		RING_API_ERROR("Expected one parameter: a list representing the config.");
		return;
	}
	List *pList = RING_API_GETLIST(1);
	libsql_config config = {0};
	const char *err_msg;
	libsql_database_t db;
	for (int i = 1; i <= ring_list_getsize(pList); i++)
	{
		if (!ring_list_islist(pList, i))
			continue;
		List *pItem = ring_list_getlist(pList, i);
		if (ring_list_getsize(pItem) != 2 || !ring_list_isstring(pItem, 1))
			continue;
		char *key = ring_string_lower(ring_list_getstring(pItem, 1));
		if (strcmp(key, "db_path") == 0 && ring_list_isstring(pItem, 2))
		{
			config.db_path = ring_list_getstring(pItem, 2);
		}
		else if (strcmp(key, "primary_url") == 0 && ring_list_isstring(pItem, 2))
		{
			config.primary_url = ring_list_getstring(pItem, 2);
		}
		else if (strcmp(key, "auth_token") == 0 && ring_list_isstring(pItem, 2))
		{
			config.auth_token = ring_list_getstring(pItem, 2);
		}
		else if (strcmp(key, "read_your_writes") == 0 && ring_list_isnumber(pItem, 2))
		{
			config.read_your_writes = (char)ring_list_getdouble(pItem, 2);
		}
		else if (strcmp(key, "encryption_key") == 0 && ring_list_isstring(pItem, 2))
		{
			config.encryption_key = ring_list_getstring(pItem, 2);
		}
		else if (strcmp(key, "sync_interval") == 0 && ring_list_isnumber(pItem, 2))
		{
			config.sync_interval = (int)ring_list_getdouble(pItem, 2);
		}
		else if (strcmp(key, "with_webpki") == 0 && ring_list_isnumber(pItem, 2))
		{
			config.with_webpki = (char)ring_list_getdouble(pItem, 2);
		}
		else if (strcmp(key, "offline") == 0 && ring_list_isnumber(pItem, 2))
		{
			config.offline = (char)ring_list_getdouble(pItem, 2);
		}
		else if (strcmp(key, "remote_encryption_key") == 0 && ring_list_isstring(pItem, 2))
		{
			config.remote_encryption_key = ring_list_getstring(pItem, 2);
		}
	}
	int rc = libsql_open_sync_with_config(config, &db, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
	RING_API_RETMANAGEDCPOINTER(db, RING_POINTER_LIBSQL_DB, ring_libsql_free_db);
}

RING_FUNC(ring_libsql_open_ext)
{
	const char *err_msg;
	libsql_database_t db;
	if (RING_API_PARACOUNT != 1)
	{
		RING_API_ERROR(RING_API_MISS1PARA);
		return;
	}
	if (!RING_API_ISSTRING(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	int rc = libsql_open_ext(RING_API_GETSTRING(1), &db, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
	RING_API_RETMANAGEDCPOINTER(db, RING_POINTER_LIBSQL_DB, ring_libsql_free_db);
}

RING_FUNC(ring_libsql_open_file)
{
	const char *err_msg;
	libsql_database_t db;
	if (RING_API_PARACOUNT != 1)
	{
		RING_API_ERROR(RING_API_MISS1PARA);
		return;
	}
	if (!RING_API_ISSTRING(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	int rc = libsql_open_file(RING_API_GETSTRING(1), &db, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
	RING_API_RETMANAGEDCPOINTER(db, RING_POINTER_LIBSQL_DB, ring_libsql_free_db);
}

RING_FUNC(ring_libsql_open_remote)
{
	const char *err_msg;
	libsql_database_t db;
	if (RING_API_PARACOUNT != 2)
	{
		RING_API_ERROR(RING_API_MISS2PARA);
		return;
	}
	if (!RING_API_ISSTRING(1) || !RING_API_ISSTRING(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	int rc = libsql_open_remote(RING_API_GETSTRING(1), RING_API_GETSTRING(2), &db, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
	RING_API_RETMANAGEDCPOINTER(db, RING_POINTER_LIBSQL_DB, ring_libsql_free_db);
}

RING_FUNC(ring_libsql_open_remote_with_remote_encryption)
{
	const char *err_msg;
	libsql_database_t db;
	if (RING_API_PARACOUNT != 3)
	{
		RING_API_ERROR(RING_API_MISS3PARA);
		return;
	}
	if (!RING_API_ISSTRING(1) || !RING_API_ISSTRING(2) || !RING_API_ISSTRING(3))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	int rc = libsql_open_remote_with_remote_encryption(RING_API_GETSTRING(1), RING_API_GETSTRING(2),
													   RING_API_GETSTRING(3), &db, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
	RING_API_RETMANAGEDCPOINTER(db, RING_POINTER_LIBSQL_DB, ring_libsql_free_db);
}

RING_FUNC(ring_libsql_open_remote_with_webpki)
{
	const char *err_msg;
	libsql_database_t db;
	if (RING_API_PARACOUNT != 2)
	{
		RING_API_ERROR(RING_API_MISS2PARA);
		return;
	}
	if (!RING_API_ISSTRING(1) || !RING_API_ISSTRING(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	int rc = libsql_open_remote_with_webpki(RING_API_GETSTRING(1), RING_API_GETSTRING(2), &db, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
	RING_API_RETMANAGEDCPOINTER(db, RING_POINTER_LIBSQL_DB, ring_libsql_free_db);
}

RING_FUNC(ring_libsql_close)
{
	if (RING_API_PARACOUNT != 1)
	{
		RING_API_ERROR(RING_API_MISS1PARA);
		return;
	}
	if (RING_API_ISPOINTER(1))
	{
		libsql_database_t db = (libsql_database_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_DB);
		if (db)
		{
			libsql_close(db);
			RING_API_SETNULLPOINTER(1);
		}
	}
	else
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
	}
}

RING_FUNC(ring_libsql_connect)
{
	const char *err_msg;
	libsql_connection_t conn;
	if (RING_API_PARACOUNT != 1)
	{
		RING_API_ERROR(RING_API_MISS1PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_database_t db = (libsql_database_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_DB);
	int rc = libsql_connect(db, &conn, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
	RING_API_RETMANAGEDCPOINTER(conn, RING_POINTER_LIBSQL_CONN, ring_libsql_free_conn);
}

RING_FUNC(ring_libsql_load_extension)
{
	const char *err_msg;
	if (RING_API_PARACOUNT != 3)
	{
		RING_API_ERROR(RING_API_MISS3PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1) || !RING_API_ISSTRING(2) || !RING_API_ISSTRING(3))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_connection_t conn = (libsql_connection_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_CONN);
	int rc = libsql_load_extension(conn, RING_API_GETSTRING(2), RING_API_GETSTRING(3), &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
}

RING_FUNC(ring_libsql_set_reserved_bytes)
{
	const char *err_msg;
	if (RING_API_PARACOUNT != 2)
	{
		RING_API_ERROR(RING_API_MISS2PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1) || !RING_API_ISNUMBER(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_connection_t conn = (libsql_connection_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_CONN);
	int rc = libsql_set_reserved_bytes(conn, (int32_t)RING_API_GETNUMBER(2), &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
}

RING_FUNC(ring_libsql_get_reserved_bytes)
{
	const char *err_msg;
	int32_t reserved_bytes;
	if (RING_API_PARACOUNT != 1)
	{
		RING_API_ERROR(RING_API_MISS1PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_connection_t conn = (libsql_connection_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_CONN);
	int rc = libsql_get_reserved_bytes(conn, &reserved_bytes, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
	RING_API_RETNUMBER(reserved_bytes);
}

RING_FUNC(ring_libsql_reset)
{
	const char *err_msg;
	if (RING_API_PARACOUNT != 1)
	{
		RING_API_ERROR(RING_API_MISS1PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_connection_t conn = (libsql_connection_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_CONN);
	int rc = libsql_reset(conn, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
}

RING_FUNC(ring_libsql_disconnect)
{
	if (RING_API_PARACOUNT != 1)
	{
		RING_API_ERROR(RING_API_MISS1PARA);
		return;
	}
	if (RING_API_ISPOINTER(1))
	{
		libsql_connection_t conn = (libsql_connection_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_CONN);
		if (conn)
		{
			libsql_disconnect(conn);
			RING_API_SETNULLPOINTER(1);
		}
	}
	else
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
	}
}

RING_FUNC(ring_libsql_prepare)
{
	const char *err_msg;
	libsql_stmt_t stmt;
	if (RING_API_PARACOUNT != 2)
	{
		RING_API_ERROR(RING_API_MISS2PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1) || !RING_API_ISSTRING(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_connection_t conn = (libsql_connection_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_CONN);
	int rc = libsql_prepare(conn, RING_API_GETSTRING(2), &stmt, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
	RING_API_RETMANAGEDCPOINTER(stmt, RING_POINTER_LIBSQL_STMT, ring_libsql_free_stmt);
}

RING_FUNC(ring_libsql_bind_int)
{
	const char *err_msg;
	if (RING_API_PARACOUNT != 3)
	{
		RING_API_ERROR(RING_API_MISS3PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1) || !RING_API_ISNUMBER(2) || !RING_API_ISNUMBER(3))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_stmt_t stmt = (libsql_stmt_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_STMT);
	int rc = libsql_bind_int(stmt, (int)RING_API_GETNUMBER(2), (long long)RING_API_GETNUMBER(3), &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
}

RING_FUNC(ring_libsql_bind_float)
{
	const char *err_msg;
	if (RING_API_PARACOUNT != 3)
	{
		RING_API_ERROR(RING_API_MISS3PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1) || !RING_API_ISNUMBER(2) || !RING_API_ISNUMBER(3))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_stmt_t stmt = (libsql_stmt_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_STMT);
	int rc = libsql_bind_float(stmt, (int)RING_API_GETNUMBER(2), RING_API_GETNUMBER(3), &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
}

RING_FUNC(ring_libsql_bind_null)
{
	const char *err_msg;
	if (RING_API_PARACOUNT != 2)
	{
		RING_API_ERROR(RING_API_MISS2PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1) || !RING_API_ISNUMBER(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_stmt_t stmt = (libsql_stmt_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_STMT);
	int rc = libsql_bind_null(stmt, (int)RING_API_GETNUMBER(2), &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
}

RING_FUNC(ring_libsql_bind_string)
{
	const char *err_msg;
	if (RING_API_PARACOUNT != 3)
	{
		RING_API_ERROR(RING_API_MISS3PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1) || !RING_API_ISNUMBER(2) || !RING_API_ISSTRING(3))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_stmt_t stmt = (libsql_stmt_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_STMT);
	int rc = libsql_bind_string(stmt, (int)RING_API_GETNUMBER(2), RING_API_GETSTRING(3), &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
}

RING_FUNC(ring_libsql_bind_blob)
{
	const char *err_msg;
	if (RING_API_PARACOUNT != 3)
	{
		RING_API_ERROR(RING_API_MISS3PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1) || !RING_API_ISNUMBER(2) || !RING_API_ISSTRING(3))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_stmt_t stmt = (libsql_stmt_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_STMT);
	int rc = libsql_bind_blob(stmt, (int)RING_API_GETNUMBER(2), (const unsigned char *)RING_API_GETSTRING(3),
							  RING_API_GETSTRINGSIZE(3), &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
}

RING_FUNC(ring_libsql_query_stmt)
{
	const char *err_msg;
	libsql_rows_t rows;
	if (RING_API_PARACOUNT != 1)
	{
		RING_API_ERROR(RING_API_MISS1PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_stmt_t stmt = (libsql_stmt_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_STMT);
	int rc = libsql_query_stmt(stmt, &rows, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
	RING_API_RETMANAGEDCPOINTER(rows, RING_POINTER_LIBSQL_ROWS, ring_libsql_free_rows);
}

RING_FUNC(ring_libsql_execute_stmt)
{
	const char *err_msg;
	if (RING_API_PARACOUNT != 1)
	{
		RING_API_ERROR(RING_API_MISS1PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_stmt_t stmt = (libsql_stmt_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_STMT);
	int rc = libsql_execute_stmt(stmt, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
}

RING_FUNC(ring_libsql_reset_stmt)
{
	const char *err_msg;
	if (RING_API_PARACOUNT != 1)
	{
		RING_API_ERROR(RING_API_MISS1PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_stmt_t stmt = (libsql_stmt_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_STMT);
	int rc = libsql_reset_stmt(stmt, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
}

RING_FUNC(ring_libsql_query)
{
	const char *err_msg;
	libsql_rows_t rows;
	if (RING_API_PARACOUNT != 2)
	{
		RING_API_ERROR(RING_API_MISS2PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1) || !RING_API_ISSTRING(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_connection_t conn = (libsql_connection_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_CONN);
	int rc = libsql_query(conn, RING_API_GETSTRING(2), &rows, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
	RING_API_RETMANAGEDCPOINTER(rows, RING_POINTER_LIBSQL_ROWS, ring_libsql_free_rows);
}

RING_FUNC(ring_libsql_execute)
{
	const char *err_msg;
	if (RING_API_PARACOUNT != 2)
	{
		RING_API_ERROR(RING_API_MISS2PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1) || !RING_API_ISSTRING(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_connection_t conn = (libsql_connection_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_CONN);
	int rc = libsql_execute(conn, RING_API_GETSTRING(2), &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
}

RING_FUNC(ring_libsql_wait_result)
{
	if (RING_API_PARACOUNT != 1)
	{
		RING_API_ERROR(RING_API_MISS1PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_rows_future_t future = (libsql_rows_future_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_ROWS_FUTURE);
	libsql_wait_result(future);
}

RING_FUNC(ring_libsql_column_count)
{
	if (RING_API_PARACOUNT != 1)
	{
		RING_API_ERROR(RING_API_MISS1PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_rows_t rows = (libsql_rows_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_ROWS);
	RING_API_RETNUMBER(libsql_column_count(rows));
}

RING_FUNC(ring_libsql_column_name)
{
	const char *name;
	const char *err_msg;
	if (RING_API_PARACOUNT != 2)
	{
		RING_API_ERROR(RING_API_MISS2PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1) || !RING_API_ISNUMBER(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_rows_t rows = (libsql_rows_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_ROWS);
	int rc = libsql_column_name(rows, (int)RING_API_GETNUMBER(2), &name, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
	RING_API_RETSTRING(name);
}

RING_FUNC(ring_libsql_column_type)
{
	int type;
	const char *err_msg;
	if (RING_API_PARACOUNT != 3)
	{
		RING_API_ERROR(RING_API_MISS3PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1) || !RING_API_ISPOINTER(2) || !RING_API_ISNUMBER(3))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_rows_t rows = (libsql_rows_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_ROWS);
	libsql_row_t row = (libsql_row_t)RING_API_GETCPOINTER(2, RING_POINTER_LIBSQL_ROW);
	int rc = libsql_column_type(rows, row, (int)RING_API_GETNUMBER(3), &type, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
	RING_API_RETNUMBER(type);
}

RING_FUNC(ring_libsql_changes)
{
	if (RING_API_PARACOUNT != 1)
	{
		RING_API_ERROR(RING_API_MISS1PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_connection_t conn = (libsql_connection_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_CONN);
	RING_API_RETNUMBER(libsql_changes(conn));
}

RING_FUNC(ring_libsql_last_insert_rowid)
{
	if (RING_API_PARACOUNT != 1)
	{
		RING_API_ERROR(RING_API_MISS1PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_connection_t conn = (libsql_connection_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_CONN);
	RING_API_RETNUMBER(libsql_last_insert_rowid(conn));
}

RING_FUNC(ring_libsql_next_row)
{
	const char *err_msg;
	libsql_row_t row;
	if (RING_API_PARACOUNT != 1)
	{
		RING_API_ERROR(RING_API_MISS1PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_rows_t rows = (libsql_rows_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_ROWS);
	int rc = libsql_next_row(rows, &row, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
	if (row)
	{
		RING_API_RETMANAGEDCPOINTER(row, RING_POINTER_LIBSQL_ROW, ring_libsql_free_row);
	}
}

RING_FUNC(ring_libsql_get_string)
{
	const char *value;
	const char *err_msg;
	if (RING_API_PARACOUNT != 2)
	{
		RING_API_ERROR(RING_API_MISS2PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1) || !RING_API_ISNUMBER(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_row_t row = (libsql_row_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_ROW);
	int rc = libsql_get_string(row, (int)RING_API_GETNUMBER(2), &value, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
	RING_API_RETSTRING(value);
	libsql_free_string(value);
}

RING_FUNC(ring_libsql_get_int)
{
	long long value;
	const char *err_msg;
	if (RING_API_PARACOUNT != 2)
	{
		RING_API_ERROR(RING_API_MISS2PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1) || !RING_API_ISNUMBER(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_row_t row = (libsql_row_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_ROW);
	int rc = libsql_get_int(row, (int)RING_API_GETNUMBER(2), &value, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
	RING_API_RETNUMBER(value);
}

RING_FUNC(ring_libsql_get_float)
{
	double value;
	const char *err_msg;
	if (RING_API_PARACOUNT != 2)
	{
		RING_API_ERROR(RING_API_MISS2PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1) || !RING_API_ISNUMBER(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_row_t row = (libsql_row_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_ROW);
	int rc = libsql_get_float(row, (int)RING_API_GETNUMBER(2), &value, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
	RING_API_RETNUMBER(value);
}

RING_FUNC(ring_libsql_get_blob)
{
	blob b;
	const char *err_msg;
	if (RING_API_PARACOUNT != 2)
	{
		RING_API_ERROR(RING_API_MISS2PARA);
		return;
	}
	if (!RING_API_ISPOINTER(1) || !RING_API_ISNUMBER(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	libsql_row_t row = (libsql_row_t)RING_API_GETCPOINTER(1, RING_POINTER_LIBSQL_ROW);
	int rc = libsql_get_blob(row, (int)RING_API_GETNUMBER(2), &b, &err_msg);
	LIBSQL_CHECK_OK(rc, err_msg);
	RING_API_RETSTRING2(b.ptr, b.len);
	libsql_free_blob(b);
}

/* Constants */

RING_FUNC(ring_get_libsql_int)
{
	RING_API_RETNUMBER(LIBSQL_INT);
}

RING_FUNC(ring_get_libsql_float)
{
	RING_API_RETNUMBER(LIBSQL_FLOAT);
}

RING_FUNC(ring_get_libsql_text)
{
	RING_API_RETNUMBER(LIBSQL_TEXT);
}

RING_FUNC(ring_get_libsql_blob)
{
	RING_API_RETNUMBER(LIBSQL_BLOB);
}

RING_FUNC(ring_get_libsql_null)
{
	RING_API_RETNUMBER(LIBSQL_NULL);
}

RING_LIBINIT
{
	/* Constants */
	RING_API_REGISTER("get_libsql_int", ring_get_libsql_int);
	RING_API_REGISTER("get_libsql_float", ring_get_libsql_float);
	RING_API_REGISTER("get_libsql_text", ring_get_libsql_text);
	RING_API_REGISTER("get_libsql_blob", ring_get_libsql_blob);
	RING_API_REGISTER("get_libsql_null", ring_get_libsql_null);

	/* Functions */
	RING_API_REGISTER("libsql_enable_internal_tracing", ring_libsql_enable_internal_tracing);
	RING_API_REGISTER("libsql_sync", ring_libsql_sync);
	RING_API_REGISTER("libsql_sync2", ring_libsql_sync2);
	RING_API_REGISTER("libsql_open_sync", ring_libsql_open_sync);
	RING_API_REGISTER("libsql_open_sync_with_webpki", ring_libsql_open_sync_with_webpki);
	RING_API_REGISTER("libsql_open_sync_with_config", ring_libsql_open_sync_with_config);
	RING_API_REGISTER("libsql_open_ext", ring_libsql_open_ext);
	RING_API_REGISTER("libsql_open_file", ring_libsql_open_file);
	RING_API_REGISTER("libsql_open_remote", ring_libsql_open_remote);
	RING_API_REGISTER("libsql_open_remote_with_remote_encryption", ring_libsql_open_remote_with_remote_encryption);
	RING_API_REGISTER("libsql_open_remote_with_webpki", ring_libsql_open_remote_with_webpki);
	RING_API_REGISTER("libsql_close", ring_libsql_close);
	RING_API_REGISTER("libsql_connect", ring_libsql_connect);
	RING_API_REGISTER("libsql_load_extension", ring_libsql_load_extension);
	RING_API_REGISTER("libsql_set_reserved_bytes", ring_libsql_set_reserved_bytes);
	RING_API_REGISTER("libsql_get_reserved_bytes", ring_libsql_get_reserved_bytes);
	RING_API_REGISTER("libsql_reset", ring_libsql_reset);
	RING_API_REGISTER("libsql_disconnect", ring_libsql_disconnect);
	RING_API_REGISTER("libsql_prepare", ring_libsql_prepare);
	RING_API_REGISTER("libsql_bind_int", ring_libsql_bind_int);
	RING_API_REGISTER("libsql_bind_float", ring_libsql_bind_float);
	RING_API_REGISTER("libsql_bind_null", ring_libsql_bind_null);
	RING_API_REGISTER("libsql_bind_string", ring_libsql_bind_string);
	RING_API_REGISTER("libsql_bind_blob", ring_libsql_bind_blob);
	RING_API_REGISTER("libsql_query_stmt", ring_libsql_query_stmt);
	RING_API_REGISTER("libsql_execute_stmt", ring_libsql_execute_stmt);
	RING_API_REGISTER("libsql_reset_stmt", ring_libsql_reset_stmt);
	RING_API_REGISTER("libsql_query", ring_libsql_query);
	RING_API_REGISTER("libsql_execute", ring_libsql_execute);
	RING_API_REGISTER("libsql_wait_result", ring_libsql_wait_result);
	RING_API_REGISTER("libsql_column_count", ring_libsql_column_count);
	RING_API_REGISTER("libsql_column_name", ring_libsql_column_name);
	RING_API_REGISTER("libsql_column_type", ring_libsql_column_type);
	RING_API_REGISTER("libsql_changes", ring_libsql_changes);
	RING_API_REGISTER("libsql_last_insert_rowid", ring_libsql_last_insert_rowid);
	RING_API_REGISTER("libsql_next_row", ring_libsql_next_row);
	RING_API_REGISTER("libsql_get_string", ring_libsql_get_string);
	RING_API_REGISTER("libsql_get_int", ring_libsql_get_int);
	RING_API_REGISTER("libsql_get_float", ring_libsql_get_float);
	RING_API_REGISTER("libsql_get_blob", ring_libsql_get_blob);
}