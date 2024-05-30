# pbix

This repository is based on https://github.com/duckdb/extension-template, check it out if you want to build and ship your own DuckDB extension.

---

This duckdb extension, pbix, allows you to parse the data model embedded in PowerBI (pbix) files.

For a pure Python implementation of the pbix parser, check out this library 👉 [PBIXray](https://github.com/Hugoberry/pbixray).

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
## Limitations/Bugs/Features/TODO
* The WASM version can't parse `https` hosted files
* ~~pbix_read() doesn't let you select only specific columns; you need to CTE to pick the output columns~~
* pbix_read() currently limited to 2048 records
* pbix_read() will decompress the entire model in memory
