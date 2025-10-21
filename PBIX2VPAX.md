# Advanced Analysis Examples using pbix2vpax()

## Comparing Model Versions

**Track changes between model versions:**
```sql
WITH 
current_version AS (
    SELECT pbix2vpax('Sales_Model_v2.pbix') as analysis
),
previous_version AS (
    SELECT pbix2vpax('Sales_Model_v1.pbix') as analysis
),
current_measures AS (
    SELECT 
        meas.MeasureName,
        meas.TableName,
        meas.MeasureExpression,
        meas.DataType
    FROM (SELECT UNNEST((SELECT analysis FROM current_version).Measures) as meas)
),
previous_measures AS (
    SELECT 
        meas.MeasureName,
        meas.TableName,
        meas.MeasureExpression,
        meas.DataType
    FROM (SELECT UNNEST((SELECT analysis FROM previous_version).Measures) as meas)
),
current_tables AS (
    SELECT 
        tab.TableName,
        tab.RowsCount,
        tab.TableSize,
        part.RefreshedTime
    FROM (SELECT UNNEST((SELECT analysis FROM current_version).Tables) as tab),
         (SELECT UNNEST((SELECT analysis FROM current_version).Partitions) as part)
    WHERE tab.TableName = part.TableName
),
previous_tables AS (
    SELECT 
        tab.TableName,
        tab.RowsCount,
        tab.TableSize,
        part.RefreshedTime
    FROM (SELECT UNNEST((SELECT analysis FROM previous_version).Tables) as tab),
         (SELECT UNNEST((SELECT analysis FROM previous_version).Partitions) as part)
    WHERE tab.TableName = part.TableName
)
SELECT 
    'Measure Changes' as ChangeType,
    c.MeasureName as Name,
    c.TableName,
    CASE 
        WHEN p.MeasureName IS NULL THEN 'Added'
        WHEN c.MeasureExpression != p.MeasureExpression THEN 'Modified'
        ELSE 'Unchanged'
    END as Status,
    c.MeasureExpression as CurrentDefinition,
    p.MeasureExpression as PreviousDefinition
FROM current_measures c
LEFT JOIN previous_measures p ON c.MeasureName = p.MeasureName AND c.TableName = p.TableName
WHERE p.MeasureName IS NULL OR c.MeasureExpression != p.MeasureExpression

UNION ALL

SELECT 
    'Table Size Changes' as ChangeType,
    c.TableName as Name,
    NULL as TableName,
    'Modified' as Status,
    {
        'rows': c.RowsCount,  
        'rows_delta': c.RowsCount - p.RowsCount,
        'sizeMB': ROUND(c.TableSize / 1024.0 / 1024.0, 2), 
        'sizeMB_delta': ROUND((c.TableSize - p.TableSize) / 1024.0 / 1024.0, 2),
        'last_refresh': c.RefreshedTime
    } as CurrentDefinition,
    {
        'rows': p.RowsCount,
        'sizeMB': ROUND(p.TableSize / 1024.0 / 1024.0, 2),
        'last-refresh': p.RefreshedTime,
    } as PreviousDefinition
FROM current_tables c
INNER JOIN previous_tables p ON c.TableName = p.TableName
WHERE c.RowsCount != p.RowsCount OR ABS(c.TableSize - p.TableSize) > 1024
ORDER BY ChangeType, Status DESC, Name;
```

## Batch Analysis Across Multiple Files

**Search for DAX functions across all models:**
```sql
WITH model_measures AS (
    SELECT 
        file,
        UNNEST(pbix2vpax(file).Measures) as meas
    FROM (SELECT unnest(glob('data/reports/**/*.pbix')) as file)
)
SELECT 
    file,
    meas.TableName,
    meas.MeasureName,
    meas.MeasureExpression,
    CASE 
        WHEN meas.MeasureExpression ILIKE '%USERELATIONSHIP%' THEN 'USERELATIONSHIP'
        WHEN meas.MeasureExpression ILIKE '%CALCULATE%' THEN 'CALCULATE'
        WHEN meas.MeasureExpression ILIKE '%TREATAS%' THEN 'TREATAS'
        WHEN meas.MeasureExpression ILIKE '%CROSSFILTER%' THEN 'CROSSFILTER'
    END as DAXFunction
FROM model_measures
WHERE meas.MeasureExpression ILIKE '%USERELATIONSHIP%'
   OR meas.MeasureExpression ILIKE '%TREATAS%'
   OR meas.MeasureExpression ILIKE '%CROSSFILTER%'
ORDER BY DAXFunction, file, meas.MeasureName;
```

**Find models with specific table structures:**
```sql
-- Find all models that have a 'Date' table with specific columns
WITH model_columns AS (
    SELECT 
        REGEXP_EXTRACT(file, '([^/]+)\.pbix$', 1) as ModelName,
        file,
        col.TableName,
        col.ColumnName,
        col.DataType
    FROM (SELECT unnest(glob('data/reports/**/*.pbix')) as file),
    LATERAL (SELECT UNNEST(pbix2vpax(file).Columns) as col)
    WHERE col.TableName = 'Date'
)
SELECT 
    ModelName,
    file,
    STRING_AGG(ColumnName, ', ' ORDER BY ColumnName) as DateColumns,
    COUNT(*) as ColumnCount
FROM model_columns
GROUP BY ModelName, file
HAVING COUNT(*) >= 5  -- Models with at least 5 date columns
ORDER BY ColumnCount DESC;
```
