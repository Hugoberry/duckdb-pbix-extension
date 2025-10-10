# pbix

This repository is based on https://github.com/duckdb/extension-template, check it out if you want to build and ship your own DuckDB extension.

---

This duckdb extension, pbix, allows you to parse the data model embedded in PowerBI (pbix) files.

For a pure Python implementation of the pbix parser, check out this library 👉 [PBIXray](https://github.com/Hugoberry/pbixray).

## Features

The extension provides three main functions:

1. **`pbix_meta()`** - Access metadata tables from the data model (table function)
2. **`pbix_read()`** - Read data from tables in the data model (table function)
3. **`pbix2vpax()`** - Generate comprehensive VPAX analysis of the entire data model (scalar function)

## Building
### Build steps
Now to build the extension, run:
```sh
make
```
The main binaries that will be built are:
```sh
./build/release/duckdb
./build/release/test/unittest
./build/release/extension/pbix/pbix.duckdb_extension
```
- `duckdb` is the binary for the duckdb shell with the extension code automatically loaded.
- `unittest` is the test runner of duckdb. Again, the extension is already linked into the binary.
- `pbix.duckdb_extension` is the loadable binary as it would be distributed.

## Running the extension
To run the extension code, start the shell with `./build/release/duckdb`.

Now we can use the features from the extension directly in DuckDB. The extension contains two table functions `pbix_meta()` that takes two string arguments (filename and metadata table) and returns the contents of the metadata table and `pbix_read` that takes two string arguments (filename and table) and returns the contents of the table:

### pbix_meta()
Returns metadata table for a data model (consult [MS-SSAS-T](https://learn.microsoft.com/en-us/openspecs/sql_server_protocols/ms-ssas-t/f85cd3b9-690c-4bc7-a1f0-a854d7daecd8) for metadata structures). For a list of available tables try `sqlite_master`.
```
D SELECT Name FROM pbix_meta('Adventure Works DW 2020.pbix','table') where isHidden=0;
┌─────────────────┐
│ Name            │
╞═════════════════╡
│ Customer        │
│ Date            │
│ Sales Territory │
│ Product         │
│ Sales Order     │
│ Sales           │
│ Reseller        │
│ Currency Rate   │
│ Currency        │
└─────────────────┘
```
### pbix_read()
Returns the contents of table from pbix file.
```sql
D SELECT 
  ResellerKey, 
  'Business Type', 
  Reseller, 
  'Reseller ID' 
  FROM pbix_read('Adventure Works DW 2020.pbix','Reseller') limit 10; 
┌─────────────┬──────────────────────┬─────────────────────────────────┬─────────────┐
│ ResellerKey ┆ 'Business Type'      ┆ Reseller                        ┆'Reseller ID'│
╞═════════════╪══════════════════════╪═════════════════════════════════╪═════════════╡
│         277 ┆ Specialty Bike Shop  ┆ The Bicycle Accessories Company ┆ AW00000277  │
│         455 ┆ Value Added Reseller ┆ Timely Shipping Service         ┆ AW00000455  │
│         609 ┆ Value Added Reseller ┆ Good Toys                       ┆ AW00000609  │
│         492 ┆ Specialty Bike Shop  ┆ Basic Sports Equipment          ┆ AW00000492  │
│         365 ┆ Specialty Bike Shop  ┆ Distinctive Store               ┆ AW00000365  │
│         168 ┆ Specialty Bike Shop  ┆ Economy Bikes Company           ┆ AW00000168  │
│           6 ┆ Warehouse            ┆ Aerobic Exercise Company        ┆ AW00000006  │
│         402 ┆ Warehouse            ┆ Pro Sporting Goods              ┆ AW00000402  │
│         529 ┆ Warehouse            ┆ Big-Time Bike Store             ┆ AW00000529  │
│         241 ┆ Specialty Bike Shop  ┆ Vale Riding Supplies            ┆ AW00000241  │
└─────────────┴──────────────────────┴─────────────────────────────────┴─────────────┘
```
### pbix2vpax()
Generates a comprehensive VPAX (VertiPaq Analyzer eXport) analysis of the entire Power BI data model. This scalar function returns a structured analysis of tables, columns, measures, relationships, hierarchies, and partitions with detailed size and cardinality information.

**Syntax:**
```sql
pbix2vpax(filename)
```

**Returns:** A STRUCT containing:
- `Tables` - List of all tables with size and row count information
- `Columns` - Detailed column metadata including cardinality, encoding, and sizes
- `Measures` - DAX measures with expressions and formatting
- `Relationships` - Relationship definitions with cardinality information
- `ColumnsHierarchies` - Column-level hierarchy structures
- `UserHierarchies` - User-defined hierarchies
- `Partitions` - Partition information including refresh times
- `ColumnsSegments` - (Reserved for future use)
- `TablePermissions` - Row Level Security details
- `CalculationItems` - (Reserved for future use)

#### Basic Usage

**Get list of all tables:**
```sql
SELECT UNNEST(pbix2vpax('Adventure Works DW 2020.pbix').Tables).TableName;
```

**Get complete VPAX structure:**
```sql
SELECT pbix2vpax('Adventure Works DW 2020.pbix');
```

**Count tables and columns:**
```sql
SELECT 
    list_count(pbix2vpax('Adventure Works DW 2020.pbix').Tables) as table_count,
    list_count(pbix2vpax('Adventure Works DW 2020.pbix').Columns) as column_count;
```

**Analyze table sizes:**
```sql
SELECT 
    tab.TableName,
    tab.RowsCount,
    tab.TableSize / 1024.0 / 1024.0 as TableSizeMB,
    tab.ColumnsSize / 1024.0 / 1024.0 as ColumnsSizeMB,
    tab.RelationshipsSize / 1024.0 / 1024.0 as RelationshipsSizeMB
FROM (SELECT UNNEST(pbix2vpax('Adventure Works DW 2020.pbix').Tables) as tab)
ORDER BY tab.TableSize DESC;
```

**Find largest columns:**
```sql
SELECT 
    col.TableName,
    col.ColumnName,
    col.DataType,
    col.ColumnCardinality,
    col.TotalSize / 1024.0 / 1024.0 as TotalSizeMB,
    col.DictionarySize / 1024.0 / 1024.0 as DictionarySizeMB,
    col.Encoding
FROM (SELECT UNNEST(pbix2vpax('Adventure Works DW 2020.pbix').Columns) as col)
ORDER BY col.TotalSize DESC
LIMIT 10;
```

**Analyze measures by table:**
```sql
SELECT 
    meas.TableName,
    COUNT(*) as MeasureCount,
    STRING_AGG(meas.MeasureName, ', ') as Measures
FROM (SELECT UNNEST(pbix2vpax('Adventure Works DW 2020.pbix').Measures) as meas)
GROUP BY meas.TableName
ORDER BY MeasureCount DESC;
```

**Examine relationships:**
```sql
SELECT 
    rel.FromTableName,
    rel.FromFullColumnName,
    rel.ToTableName,
    rel.ToFullColumnName,
    rel.FromCardinalityType,
    rel.ToCardinalityType,
    rel.IsActive,
    rel.CrossFilteringBehavior,
    rel.UsedSize / 1024.0 / 1024.0 as RelationshipSizeMB
FROM (SELECT UNNEST(pbix2vpax('Adventure Works DW 2020.pbix').Relationships) as rel)
ORDER BY rel.UsedSize DESC;
```

**Check encoding efficiency:**
```sql
SELECT 
    col.TableName,
    col.ColumnName,
    col.Encoding,
    col.ColumnCardinality,
    col.TotalSize / 1024.0 / 1024.0 as SizeMB,
    CASE 
        WHEN col.ColumnCardinality > 0 
        THEN col.TotalSize::DOUBLE / col.ColumnCardinality 
        ELSE 0 
    END as BytesPerValue
FROM (SELECT UNNEST(pbix2vpax('Adventure Works DW 2020.pbix').Columns) as col)
WHERE col.ColumnCardinality > 0
ORDER BY BytesPerValue DESC
LIMIT 10;
```

**Analyze hierarchies:**
```sql
SELECT 
    hier.TableName,
    hier.UserHierarchyName,
    hier.Levels,
    hier.UsedSize / 1024.0 / 1024.0 as HierarchySizeMB
FROM (SELECT UNNEST(pbix2vpax('Adventure Works DW 2020.pbix').UserHierarchies) as hier)
ORDER BY hier.UsedSize DESC;
```

**Partition refresh status:**
```sql
SELECT 
    part.TableName,
    part.PartitionName,
    part.RecordCount,
    part.SegmentCount,
    part.RefreshedTime,
    part.RecordsPerSegment
FROM (SELECT UNNEST(pbix2vpax('Adventure Works DW 2020.pbix').Partitions) as part)
ORDER BY part.RecordCount DESC;
```

#### Advanced Analysis Examples

**Model size breakdown:**
```sql
WITH vpax AS (
    SELECT pbix2vpax('Adventure Works DW 2020.pbix') as analysis
),
table_sizes AS (
    SELECT 
        UNNEST(vpax.analysis.Tables).TableName as TableName,
        UNNEST(vpax.analysis.Tables).TableSize as TableSize
    FROM vpax
)
SELECT 
    TableName,
    TableSize / 1024.0 / 1024.0 as SizeMB,
    100.0 * TableSize / SUM(TableSize) OVER () as PercentOfTotal
FROM table_sizes
ORDER BY TableSize DESC;
```

**High cardinality columns analysis:**
```sql
WITH vpax AS (
    SELECT pbix2vpax('Adventure Works DW 2020.pbix') as analysis
)
SELECT 
    col.TableName,
    col.ColumnName,
    col.ColumnCardinality,
    col.TotalSize / 1024.0 / 1024.0 as SizeMB,
    col.Encoding,
    CASE 
        WHEN col.ColumnCardinality > 1000000 THEN 'Very High'
        WHEN col.ColumnCardinality > 100000 THEN 'High'
        WHEN col.ColumnCardinality > 10000 THEN 'Medium'
        ELSE 'Low'
    END as CardinalityLevel
FROM (SELECT UNNEST(vpax.analysis.Columns) as col FROM vpax)
WHERE col.ColumnCardinality > 0
ORDER BY col.ColumnCardinality DESC;
```

**Relationship network analysis:**
```sql
WITH vpax AS (
    SELECT pbix2vpax('Adventure Works DW 2020.pbix') as analysis
)
SELECT 
    rel.FromTableName,
    rel.ToTableName,
    COUNT(*) as RelationshipCount,
    SUM(rel.UsedSize) / 1024.0 / 1024.0 as TotalRelationshipSizeMB
FROM (SELECT UNNEST(vpax.analysis.Relationships) as rel FROM vpax)
GROUP BY rel.FromTableName, rel.ToTableName
ORDER BY RelationshipCount DESC;
```

**Measure catalog with expressions:**
```sql
SELECT 
    meas.TableName,
    meas.MeasureName,
    meas.DataType,
    meas.MeasureExpression,
    meas.FormatString,
    meas.IsHidden
FROM (SELECT UNNEST(pbix2vpax('Adventure Works DW 2020.pbix').Measures) as meas)
WHERE NOT meas.IsHidden
ORDER BY meas.TableName, meas.MeasureName;
```

**Column type distribution:**
```sql
WITH vpax AS (
    SELECT pbix2vpax('Adventure Works DW 2020.pbix') as analysis
)
SELECT 
    col.DataType,
    COUNT(*) as ColumnCount,
    SUM(col.TotalSize) / 1024.0 / 1024.0 as TotalSizeMB,
    AVG(col.ColumnCardinality) as AvgCardinality
FROM (SELECT UNNEST(vpax.analysis.Columns) as col FROM vpax)
GROUP BY col.DataType
ORDER BY ColumnCount DESC;
```

**Export to JSON for external analysis:**
```sql
COPY (
    SELECT to_json(pbix2vpax('Adventure Works DW 2020.pbix'))
) TO 'model_analysis.json';
```

**Create a summary report:**
```sql
WITH vpax AS (
    SELECT pbix2vpax('Adventure Works DW 2020.pbix') as analysis
)
SELECT 
    'Model Summary' as Section,
    list_count(vpax.analysis.Tables) as TableCount,
    list_count(vpax.analysis.Columns) as ColumnCount,
    list_count(vpax.analysis.Measures) as MeasureCount,
    list_count(vpax.analysis.Relationships) as RelationshipCount,
    list_count(vpax.analysis.UserHierarchies) as HierarchyCount,
    (SELECT SUM(tab.TableSize) FROM (SELECT UNNEST(vpax.analysis.Tables) as tab)) / 1024.0 / 1024.0 as TotalModelSizeMB
FROM vpax;
```

**Compare multiple models:**
```sql
WITH 
model_a AS (
    SELECT 
        'Model A' as ModelName,
        UNNEST(pbix2vpax('model_a.pbix').Tables).TableName as TableName,
        UNNEST(pbix2vpax('model_a.pbix').Tables).RowsCount as RowCount,
        UNNEST(pbix2vpax('model_a.pbix').Tables).TableSize as TableSize
),
model_b AS (
    SELECT 
        'Model B' as ModelName,
        UNNEST(pbix2vpax('model_b.pbix').Tables).TableName as TableName,
        UNNEST(pbix2vpax('model_b.pbix').Tables).RowsCount as RowCount,
        UNNEST(pbix2vpax('model_b.pbix').Tables).TableSize as TableSize
)
SELECT * FROM model_a
UNION ALL
SELECT * FROM model_b
ORDER BY ModelName, TableSize DESC;
```

**Find columns that could benefit from optimization:**
```sql
WITH vpax AS (
    SELECT pbix2vpax('Adventure Works DW 2020.pbix') as analysis
)
SELECT 
    col.TableName,
    col.ColumnName,
    col.Encoding,
    col.ColumnCardinality,
    col.TotalSize / 1024.0 / 1024.0 as SizeMB,
    CASE 
        WHEN col.Encoding = 'HASH' AND col.ColumnCardinality < 1000 
        THEN 'Consider VALUE encoding'
        WHEN col.ColumnCardinality > 1000000 
        THEN 'Very high cardinality - review if needed'
        ELSE 'OK'
    END as Recommendation
FROM (SELECT UNNEST(vpax.analysis.Columns) as col FROM vpax)
WHERE col.TotalSize > 1048576  -- Larger than 1MB
ORDER BY col.TotalSize DESC
LIMIT 20;
```

**Relationship health check:**
```sql
SELECT 
    rel.FromTableName,
    rel.ToTableName,
    rel.RelationshipType,
    rel.IsActive,
    rel.CrossFilteringBehavior,
    CASE 
        WHEN NOT rel.IsActive THEN 'Inactive relationship'
        WHEN rel.CrossFilteringBehavior = 'BothDirections' THEN 'Bidirectional - review performance'
        WHEN NOT rel.RelyOnReferentialIntegrity THEN 'RI not assumed - may impact performance'
        ELSE 'OK'
    END as HealthCheck
FROM (SELECT UNNEST(pbix2vpax('Adventure Works DW 2020.pbix').Relationships) as rel)
ORDER BY rel.FromTableName, rel.ToTableName;
```

## Running the tests
Different tests can be created for DuckDB extensions. The primary way of testing DuckDB extensions should be the SQL tests in `./test/sql`. These SQL tests can be run using:
```sh
make test
```

### Installing the deployed binaries
You will need to do two things to install your extension binaries from S3. Firstly, DuckDB should be launched with the
`allow_unsigned_extensions` option set to true. How to set this will depend on the client you're using. Some examples:

CLI:
```shell
duckdb -unsigned
```

Python:
```python
con = duckdb.connect(':memory:', config={'allow_unsigned_extensions' : 'true'})
```

NodeJS:
```js
db = new duckdb.Database(':memory:', {"allow_unsigned_extensions": "true"});
```

Secondly, you must set the repository endpoint in DuckDB to the HTTP URL of your bucket + version of the extension
you want to install. To do this, run the following SQL query in DuckDB:
```sql
SET custom_extension_repository='https://duckdb.pbix.info';
```
After running these steps, you can install and load your extension using the regular INSTALL/LOAD commands in DuckDB:
```sql
INSTALL pbix
LOAD pbix
```

## Configuration Options

The extension provides several configuration options to customize its behavior:

### pbix_trailing_chunks_optimization
Controls the number of trailing chunks to read when parsing PBIX files. This affects parsing performance and success rates for different file formats.

**Default:** 15  
**Type:** INTEGER  
**Usage:**
```sql
SET pbix_trailing_chunks_optimization = 20;
```

### pbix_ignore_errors
When enabled, `pbix2vpax()` returns empty VPAX structures instead of throwing exceptions when files cannot be parsed. This is particularly useful for batch processing scenarios where you want to continue processing other files even if some fail.

**Default:** false  
**Type:** BOOLEAN  
**Usage:**
```sql
-- Enable error tolerance for batch processing
SET pbix_ignore_errors = true;

-- Process multiple files - failed files will return empty structures instead of breaking the query
SELECT 
    file, 
    list_count(pbix2vpax(file).Tables) as table_count,
    list_count(pbix2vpax(file).Columns) as column_count
FROM glob('path/to/pbix/files/**/*.pbix')
ORDER BY table_count DESC;

-- Reset to default behavior
SET pbix_ignore_errors = false;
```

**Common use cases:**
- Batch analysis of multiple PBIX files where some may be corrupted
- Data quality assessments across file collections
- Automated processing pipelines that should continue despite individual file failures

## Limitations/Bugs/Features/TODO
* The WASM version can't parse `https` hosted files
* ~~pbix_read() doesn't let you select only specific columns; you need to CTE to pick the output columns~~
* ~~pbix_read() currently limited to 2048 records~~
* pbix_read() will decompress the entire model in memory
* pbix2vpax() currently doesn't populate ColumnsSegments (segment-level detail)
* pbix2vpax() selectivity calculation is not yet implemented