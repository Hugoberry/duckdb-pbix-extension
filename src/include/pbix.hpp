#pragma once

#include "pbix_utils.hpp"

namespace duckdb {
class PbixStatement;
struct IndexInfo;

class Pbix {
public:
	Pbix();
	Pbix(pbix *db);
	~Pbix();
	// disable copy constructors
	Pbix(const Pbix &other) = delete;
	Pbix &operator=(const Pbix &) = delete;
	//! enable move constructors
	Pbix(Pbix &&other) noexcept;
	Pbix &operator=(Pbix &&) noexcept;

	pbix *db;

public:
	static Pbix Open(const string &path, const PbixOpenOptions &options, bool is_shared = false);
	bool TryPrepare(const string &query, PbixStatement &result);
	PbixStatement Prepare(const string &query);
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