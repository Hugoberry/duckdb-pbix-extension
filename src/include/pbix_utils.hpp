#pragma once

#include "duckdb.hpp"

namespace duckdb {

class PbixUtils {
public:
	static void Check(int rc, pbix *db);
	static string TypeToString(int pbix_type);
	static LogicalType TypeToLogicalType(const string &pbix_type);
	static string SanitizeString(const string &table_name);
	static string SanitizeIdentifier(const string &table_name);
	static LogicalType ToPbixType(const LogicalType &input);
	string ToPbixTypeAlias(const LogicalType &input);
};

} 