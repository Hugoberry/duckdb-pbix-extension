#pragma once

#include "duckdb.hpp"

namespace duckdb
{
	class SQLiteDB;

	class PbixReadFunction : public TableFunction
	{
	public:
		PbixReadFunction();
	};

}