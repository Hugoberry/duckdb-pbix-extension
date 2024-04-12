//===----------------------------------------------------------------------===//
//                         DuckDB
//
// sqlite_db.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "sqlite_utils.hpp"
#include "sqlite_options.hpp"

namespace duckdb {
class SQLiteStatement;
struct IndexInfo;

class SQLiteDB {
public:
	SQLiteDB();
	SQLiteDB(sqlite3 *db);
	~SQLiteDB();
	// disable copy constructors
	SQLiteDB(const SQLiteDB &other) = delete;
	SQLiteDB &operator=(const SQLiteDB &) = delete;
	//! enable move constructors
	SQLiteDB(SQLiteDB &&other) noexcept;
	SQLiteDB &operator=(SQLiteDB &&) noexcept;

	sqlite3 *db;
	std::vector<unsigned char> dbBuffer; // buffer for in-memory database

public:
	static SQLiteDB Open(const string &path, const SQLiteOpenOptions &options, bool is_shared = false);
	static SQLiteDB OpenFromBuffer(const SQLiteOpenOptions &options,const std::vector<unsigned char> &buffer);
	bool TryPrepare(const string &query, SQLiteStatement &result);
	SQLiteStatement Prepare(const string &query);
	void Execute(const string &query);
	vector<string> GetTables();

	vector<string> GetEntries(string entry_type);
	CatalogType GetEntryType(const string &name);
	void GetTableInfo(const string &table_name, ColumnList &columns, vector<unique_ptr<Constraint>> &constraints,
	                  bool all_varchar);
	void GetViewInfo(const string &view_name, string &sql);
	void GetIndexInfo(const string &index_name, string &sql, string &table_name);
	idx_t RunPragma(string pragma_name);
	//! Gets the max row id of a table, returns false if the table does not have a
	//! rowid column
	bool GetMaxRowId(const string &table_name, idx_t &row_id);
	bool ColumnExists(const string &table_name, const string &column_name);
	vector<IndexInfo> GetIndexInfo(const string &table_name);

	bool IsOpen();
	void Close();
};

} // namespace duckdb