#  Symbol Table with Scoping – C Project

This C project implements a **symbol table** that supports **multiple scopes**, variable assignment, and lookup operations using **hashing with quadratic probing**. The system processes commands from a file (`read.txt`) and supports nested blocks using a scope stack.

---

## 📁 File Structure

```

├── main.c          # Core implementation of the symbol table with scoping
├── read.txt        # Input command file for execution
```

---

## 🧾 Features

* Symbol table for managing variables with their values
* Supports **nested scoping** with parent-child relationships
* Uses **hash table with quadratic probing** to resolve collisions
* Commands like `assign`, `print`, `begin`, and `end` to simulate code blocks
* Clean output for variable access and memory address
* Graceful memory cleanup

---

## 🚀 How to Compile and Run

### 🧰 Prerequisites

* GCC or any C compiler

### 🔧 Compilation

```bash
gcc main.c -o symbol_table
```

### ▶️ Execution

Make sure you have a valid `read.txt` file in the same directory.

```bash
./symbol_table
```

---

## 📄 `read.txt` Format

The input file should contain commands like:

```
begin
assign x 10
assign y 20
print x
begin
assign x 99
print x
end
print x
end
```

### ✅ Supported Commands

| Command      | Description                            |
| ------------ | -------------------------------------- |
| `begin`      | Start a new inner scope                |
| `end`        | Exit current scope                     |
| `assign x 5` | Assign variable `x` with value `5`     |
| `print x`    | Lookup and print value of variable `x` |

---

## 📦 Output Sample

```
[Scope Level 1] Assigned x = 10
[Scope Level 1] Assigned y = 20
[Scope Level 1] Looked up x from [Scope 1] -- Found x = 10
[Scope Level 2] Assigned x = 99
[Scope Level 2] Looked up x from [Scope 2] -- Found x = 99
[Scope Level 2] Exited scope.
[Scope Level 1] Looked up x from [Scope 1] -- Found x = 10
```

---

## 🧹 Memory Management

* All scopes and entries are dynamically allocated
* Frees all memory at the end to avoid leaks

---

## 📌 Notes

* Scope level 0 is treated as the **global scope**
* A maximum of 101 scopes and 101 entries per scope are supported
* Variable lookup searches parent scopes if not found locally

---

Feel free to customize or enhance the symbol table with additional types, expressions, or error handling!
# Symbol-Table-with-Scoping-C-Project
