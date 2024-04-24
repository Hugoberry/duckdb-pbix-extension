#pragma once

#include "duckdb.hpp"
#include "vertipaq_meta.hpp"
#include "vertipaq_decoder.hpp"

namespace duckdb
{

	class SQLiteDB;

	class PbixReadFunction : public TableFunction
	{
	public:
		PbixReadFunction();
	};

}