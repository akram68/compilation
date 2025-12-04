# Matrix Compiler - Bugs Fixed

## Summary of Issues Found and Fixed

### 1. **CRITICAL: Missing Matrix Inverse Implementation**
The compiler had no matrix inverse function at all. Added `matrix_inverse()` using Gauss-Jordan elimination with partial pivoting.

### 2. **CRITICAL: Global State Pollution**
Matrix construction buffers (`temp_matrix_data`, `temp_row_count`, `temp_col_count`, `current_row_size`) were NOT being reset between matrix literals, causing data from previous matrices to contaminate new ones.

**Symptoms:**
- First matrix works correctly
- Subsequent matrices contain stale data from previous matrices
- Multiplication and other operations produce incorrect results intermittently

**Fix:**
- Added `reset_matrix_builder()` function to clear all global state
- Called `reset_matrix_builder()` after every statement
- Reset `first_row_size` to -1 after each assignment

### 3. **NULL Pointer Issues**
Operations could receive matrices with NULL `name` fields, causing crashes in code generation.

**Fix:**
- Added NULL checks before all operations
- Use fallback name "tmp" when matrix name is NULL
- Properly allocate names for intermediate results

### 4. **Memory Management Issues**
- Matrix names weren't being properly allocated/freed
- Intermediate results could leak memory

**Fix:**
- Use `strdup()` for all name assignments
- Free matrix names in `free_matrix()`

## Files Modified

### semantic.h
- Added `matrix_inverse()` declaration
- Added `reset_matrix_builder()` declaration

### semantic.c
- Implemented `matrix_inverse()` using Gauss-Jordan elimination
- Implemented `reset_matrix_builder()` to clear global state
- Added `#include <math.h>` for `fabs()`
- Updated `start_new_matrix()` to call `reset_matrix_builder()`
- Clear temp buffers in `reset_matrix_builder()`

### lexer.l
- Added `INV` token for matrix inverse operation
- Syntax: `inv(A)` to compute inverse of matrix A

### parser.y
- Added `INV` token declaration
- Added matrix inverse rule: `INV '(' ID ')'`
- Added NULL checks in all operations (ADD, SUB, MULT, TRANSPOSE, INV)
- Added fallback names for matrices with NULL names
- Call `reset_matrix_builder()` after each statement
- Reset `first_row_size = -1` after assignment
- Added error checking for failed matrix operations
- Added parenthesized expressions: `'(' expr ')'`

### codegen.h / codegen.c
- Added `generate_inverse()` function
- Outputs `INV dest src` instructions

## Usage Examples

### Matrix Inverse
```c
A = [2, 1; 1, 1];
B = inv(A);
print(B);
```

### Multiple Operations (Now Fixed!)
```c
A = [1, 2; 3, 4];
B = [5, 6; 7, 8];
C = A * B;        // Now produces correct result every time
D = [9, 10; 11, 12];
E = C + D;        // No longer contaminated by previous matrices
print(E);
```

### Complex Expression
```c
A = [1, 2; 3, 4];
B = inv(A) * A';  // Should give identity-like result
print(B);
```

## Testing Recommendations

1. **Test Sequential Matrix Literals:**
   ```c
   A = [1, 2; 3, 4];
   B = [5, 6; 7, 8];
   C = [9, 10; 11, 12];
   print(A);
   print(B);
   print(C);
   ```
   Verify each matrix is correct (no contamination).

2. **Test Matrix Inverse:**
   ```c
   A = [4, 7; 2, 6];
   B = inv(A);
   C = A * B;    // Should be close to identity matrix
   print(C);
   ```

3. **Test Multiple Operations:**
   ```c
   A = [1, 2; 3, 4];
   B = [2, 0; 1, 2];
   C = A * B;
   D = C + A';
   E = inv(B);
   print(D);
   print(E);
   ```

## Rebuilding the Project

Since you're on Windows with win_flex_bison, you need to regenerate the parser and lexer:

```cmd
cd your_project_directory
make clean
make all
```

This will:
1. Delete old generated files
2. Run win_flex.exe on lexer.l → generates lex.yy.c, lex.yy.h
3. Run win_bison.exe on parser.y → generates parser.tab.c, parser.tab.h
4. Compile all source files together
5. Create matrix_compiler.exe

## Root Cause Analysis

The main bug was **global state pollution**. When you parsed:

```c
A = [1, 2; 3, 4];
B = [5, 6; 7, 8];
```

After parsing A:
- `temp_matrix_data[0][0] = 1, temp_matrix_data[0][1] = 2`
- `temp_matrix_data[1][0] = 3, temp_matrix_data[1][1] = 4`

When parsing B, the buffer was NOT cleared:
- Still contained: `[1, 2; 3, 4]`
- New numbers (5, 6, 7, 8) overwrote the same positions
- But if B had different dimensions, old data remained in unused cells
- Result: B was partially corrupted with data from A

The fix ensures `reset_matrix_builder()` is called after each statement, completely clearing the buffer before the next matrix literal.
