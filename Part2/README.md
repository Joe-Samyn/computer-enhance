# Part 2: JSON Parsing for the Haversine Solver

A C++ project following Part 2 of Casey Muratori's **Computer Enhance** course, with a focus on building a JSON parser from scratch. The parser reads coordinate-pair data into a tree of JSON values, then converts that tree into entries for a future Haversine distance calculation.

The current `-solve` command exercises this parsing pipeline and prints the extracted coordinates. Haversine distance calculation and performance optimization are still planned.

## Build

Requirements: CMake 4.2 or newer and a C++ compiler. The generator uses a variable-length stack array, which requires compiler extension support (such as Clang or GCC).

From the `Part2` directory:

```sh
cmake -S . -B build
cmake --build build
```

## Parse input JSON

Use the included sample to exercise the parser:

```sh
./build/haversine -solve sample_data.json
```

Despite the command's name, it currently deserializes the file and prints each entry's `x0`, `y0`, `x1`, and `y1` fields; it does not calculate distances.

### Input format

The expected input is a top-level object whose first member is a `pairs` array of coordinate objects. Use decimal values and the multiline layout shown in `sample_data.json`:

```json
{
  "pairs": [
    {
      "x0": 37.000000,
      "y0": -122.000000,
      "x1": 40.000000,
      "y1": -74.000000
    }
  ]
}
```

Each entry describes two points, `(x0, y0)` and `(x1, y1)`, in degrees. The current generator uses `x0` and `x1` for latitudes in `[-90, 90]`, and `y0` and `y1` for longitudes in `[-180, 180]`. These values are stored as `float` fields in `Entry`.

### How the parser works

The implementation in `src/JsonParser.cpp` reads characters from a `FILE*` and recursively builds an abstract syntax tree (AST):

1. `DeserializeJson` opens the input file and starts parsing its first value.
2. `ParseJsonValue` dispatches to object, array, string, or number parsing based on the current character.
3. Objects store key/value pairs in a linked list; arrays store values in a linked list. Both lists start with a sentinel node.
4. `DeserializeCoordinatePairs` takes the first member of the root object as the coordinate array and converts its objects into a contiguous `Entry` array.
5. `PrintCoordinatePairs` prints the resulting entries for inspection.

`JsonValue` carries a `JsonType` tag and a union of pointers to the corresponding value structures. Numeric values have separate integer and decimal storage; coordinate extraction currently reads the decimal field.

### Parser API

| Function | Purpose |
| --- | --- |
| `DeserializeJson(const char* jsonFile)` | Reads a file and returns the root `JsonValue*`; returns `nullptr` if the file cannot be opened. |
| `DisplayAST(JsonValue* root)` | Prints the parsed tree for debugging; it is not called by default. |
| `DeserializeCoordinatePairs(const char* jsonFile)` | Converts the expected coordinate document into a `CoordinatePairs` containing an entry pointer and count. |

### Current limitations

This is a work-in-progress parser tailored to the coordinate dataset. Object, array, string, and number parsing are present; boolean and null dispatch are placeholders.

- Strings have a 10-byte buffer, allowing at most nine content characters plus the terminator. Escape sequences are not handled.
- Number scanning stops at a comma or newline, so compact JSON with a closing brace or bracket directly after a number is not handled reliably. The generator currently emits this compact layout; its output needs reformatting before use with the parser.
- Coordinate extraction expects decimal numeric values and assumes the first root member contains the coordinate array. It does not validate the `pairs` key, field types, or required fields.
- Some counters, numeric flags, and linked-list pointers are not initialized, so results are not yet reliable even for the expected format.
- Malformed input and end-of-file handling are incomplete. The coordinate deserializer does not check for a failed file open before accessing the tree.
- Allocated tree nodes and entries are not freed, and the parser does not close its input file.

## Generate coordinate JSON

```sh
./build/haversine -generate 100 -o generated_data.json
```

This writes 100 random coordinate pairs, replacing the output file if it already exists. The generator emits decimal values using `%f` and seeds each run with `std::random_device`; a user-supplied seed (`-s`) is not yet supported.

Use a positive sample size and provide an explicit output path. Argument handling still needs fixes, including an uninitialized pointer in the flag lookup. The generator stores all entries on the stack, so large sample sizes may exceed available stack space. Reformat generated JSON into the multiline input layout before parsing it.

## Project layout

| File | Purpose |
| --- | --- |
| `src/JsonParser.h` | JSON value types, tree structures, and deserialization API. |
| `src/JsonParser.cpp` | Recursive parsing, AST display, and coordinate extraction. |
| `src/Entry.h` | Coordinate entry structures and printing. |
| `src/Main.cpp` | Command-line handling and parsing/generation mode selection. |
| `src/JsonGenerator.h` | Generator declaration. |
| `src/JsonGenerator.cpp` | Random coordinate generation and JSON output. |
| `src/Unity.cpp` | Includes implementation files into a single translation unit. |
| `CMakeLists.txt` | Builds the `haversine` executable. |
| `sample_data.json` | Multiline example input for the parser. |

## Next steps

1. Initialize parser state and linked-list nodes, and add cleanup for allocations and file handles.
2. Handle number delimiters, whitespace, end-of-file, and malformed input consistently so generated JSON can be parsed directly.
3. Validate the coordinate schema and support both integer and decimal coordinate values.
4. Complete boolean, null, and escaped-string handling as the parser's scope expands.
5. Fix command-line validation and seed selection, then implement a baseline Haversine calculation.
6. Measure parsing and calculation performance and compare optimizations against the baseline.
