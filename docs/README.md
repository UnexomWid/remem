# Documentation

This is the official documentation for [Remem](https://github.com/UnexomWid/remem).

[Introduction](#introduction)  
[Overloads](#overloads)  
[Macro](#macro)  
[Namespace](#namespace)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[AddressInfo](#addressinfo)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Constructors](#constructors)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Fields](#fields)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Memory Map](#memory-map)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[Memory Management](#memory-management)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[malloc](#malloc)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[alloc](#alloc)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[realloc](#realloc)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[expand](#expand)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[free](#free)  
[Options](#options)  

# Introduction

**Remem** is a library that provides memory allocation tracking via address mapping. That is, it stores information about allocated memory blocks,
such as the size of the block, the address, and a brief description about that block. It can also automatically scale the allocation size to powers of 2 for alignment,
as well as expand memory blocks exponentially.

**Remem** will only track memory blocks that are allocated via:

- the `new[]` overload (see below)
- the `re::malloc` and `re::alloc` functions

The behavior of Remem can be changed with definitions (see [Options](#options)).

# Overloads

Remem overloads both `new[]` and `delete[]` operators.

Here are the signatures of the overloads:

```cpp
void* operator new[](size_t size);

void* operator new[](size_t size,
                     const char* who,
                     const char* file,
                     size_t line);

void  operator delete[](void* ptr) noexcept;
```

The second `new[]` operator overload also takes other parameters, which provide extra information about the memory block. However, it's used by a macro (see below), and
shouldn't be manually used.

Note that there are no overloads for the `new` and `delete` operators. Therefore, if you need to allocate memory for a single object of a type, but still want to track it,
use `new type[1]`.

The differences between these overloads and the standard `new[]` are:

- these overloads will add the address to the memory map, along with relevant information such as the size of the block
- the size will automatically be rounded to the nearest power of 2 to favor alignment

## Macro

Remem defines a macro as follows:

```cpp
#define new(who) new(who, __FILE__, __LINE__)
```

Therefore, you can indirectly invoke the second `new[]` overload by just passing a string, like this:

```cpp
int* buffer = new("Buffer") int[255];
```

The `file` and `line` will automatically be provided by the macro.

# Namespace

All of the other functions are inside the `re` namespace.

## AddressInfo

A structure which contains information about a memory block.

### Constructors

```cpp
AddressInfo() { }
AddressInfo(const char* w, size_t sz) : who(w), size(sz) { }
```

### Fields

```cpp
std::string who;
size_t      size;
```

- **who** - a description of the memory block
- **size** - the size of the memory block

## Memory Map

To access the memory map, which contains information about all of the allocated memory blocks, use the following function:

```cpp
const std::unordered_map<void*, AddressInfo>& re::mem() noexcept;
```

The `re::mem()` function returns a map that stores `address`->`AddressInfo` pairs, where `address` is `void*`.

If you only want to print the memory map, use the following function:

```cpp
void re::memPrint() noexcept;
```

The `re::memPrint` function pretty-prints the contents of the memory map. Here's an example:

```
[memory] Map: 20 byte(s)
01151D18 -> unknown: 16 byte(s)
01151C68 -> testBuffer: 4 byte(s)
```

> Note: if no description is provided for a memory block, `unknown` will be printed.

To get the total size of all memory blocks in the map, use the following function:

```cpp
size_t re::memSize()  noexcept;
```

The `re::memSize` function returns the total size of all memory blocks that are present in the memory map.

## Memory Management

Remem provides the following functions for memory management:

### malloc

```cpp
void* re::malloc(size_t size,
                 const char* who = nullptr,
                 const char* file = nullptr,
                 size_t line = 0);
```

Like `malloc`, but acts like the `new[]` overload (i.e. stores the address in the map, rounds the size).

##### Params

- **size** - the size of the memory block
- **who** - a description of the memory block
- **file** - the file in which the allocation took place (can be **\_\_FILE__**)
- **line** - the line at which the allocation took place (can be **\_\_LINE__**)

### alloc

```cpp
void* re::alloc(size_t& size,
                const char* who = nullptr,
                const char* file = nullptr,
                size_t line = 0);
```

Like `re::malloc`, but takes the size by reference (i.e. the size will be updated to the new, rounded one).

##### Params

- **size** - the size of the memory block
- **who** - a description of the memory block
- **file** - the file in which the allocation took place (can be **\_\_FILE__**)
- **line** - the line at which the allocation took place (can be **\_\_LINE__**)

##### Remarks

You should use `re::alloc` instead of `re::malloc` when you want to allocate memory for a buffer that may grow in size.

Because the size will be rounded, it can be used for future calls to the `re::expand` function (see below).

### realloc

```cpp
void* re::realloc(void* ptr,
                  size_t size,
                  const char* file = nullptr,
                  size_t line = 0);
```

Like `realloc`, but acts like the `new[]` overload (i.e. stores the address in the map, rounds the size).

This function should only be used to reallocate memory that was previously allocated with either `re::malloc` or `re::alloc`.

##### Params

- **ptr** - the pointer to the memory block
- **size** - the new size of the memory block
- **file** - the file in which the reallocation took place (can be **\_\_FILE__**)
- **line** - the line at which the reallocation took place (can be **\_\_LINE__**)

### expand

```cpp
void* re::expand(void* ptr,
                 size_t& size,
                 const char* file = nullptr,
                 size_t line = 0);
```

Expands the memory by a factor of `2`. That is, the size of the memory block doubles, and also updates by reference.

##### Params

- **ptr** - the pointer to the memory block
- **size** - the size of the memory block
- **file** - the file in which the reallocation took place (can be **\_\_FILE__**)
- **line** - the line at which the reallocation took place (can be **\_\_LINE__**)

##### Remarks

This function should be used along with the `re::alloc` function. Here's an example:

```cpp
size_t bufferSize = 10;
int* buffer = re::alloc("Buffer", bufferSize, __FILE__, __LINE__); // bufferSize becomes 16
...
buffer = re::expand(buffer, bufferSize, __FILE__, __LINE__); // bufferSize becomes 32
```

### free

```cpp
void re::free(void* ptr) noexcept;
```

Like `free`, but deletes the address entry from the memory map.

##### Params

- **ptr** - the pointer to the memory block

# Options

By default, address mapping is disabled. You can enable it by defining the `REMEM_ENABLE_MAPPING` macro (e.g. in the CMakeLists.txt file).

If address mapping is enabled, it will only add the allocated memory blocks to the map (which can be accessed via `re::mem()`). However, if the `REMEM_ENABLE_LOGGING`
macro is defined, every memory operation will be logged (such as `new[]`, `re::malloc` and `re::free`). Note that if address mapping is disabled, logging will also be
disabled even if the `REMEM_ENABLE_LOGGING` macro is defined.

By default, Remem will round the memory block size to the nearest power of 2 that is >= size. However, if the `REMEM_DISABLE_ALIGNING` macro is defined,
this will no longer happen in any allocation function (`new[]`, `re::malloc`, `re::alloc`, `re::realloc` and `re::expand`).

It's also possible to disable aligning only for `re::malloc` and `re::realloc`, by using the `REMEM_DISABLE_MALLOC_ALIGNING` and `REMEM_DISABLE_REALLOC_ALIGNING`
macros. Note that if `REMEM_DISABLE_ALIGNING` is defined, it disables aligning for these functions even if their macros are not defined (i.e. these macros only
take effect when `REMEM_DISABLE_ALIGNING` is not defined).

The `re::expand` function will grow the memory block size by a factor of `2`. However, the factor can be changed by defining the `REMEM_EXPAND_FACTOR` macro.
(e.g. `REMEM_EXPAND_FACTOR=1.5`). If you change the factor to a value other than 2, you should also disable aliging with the `REMEM_DISABLE_ALIGNING` macro.