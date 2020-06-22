/**
 * Remem (https://github.com/UnexomWid/remem)
 *
 * This project is licensed under the MIT license.
 * Copyright (c) 2020 UnexomWid (https://uw.exom.dev)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef REMEM_HXX_GUARD
#define REMEM_HXX_GUARD

#include <string>
#include <cstddef>
#include <unordered_map>

void* operator new[](size_t size, const char* who, size_t line = 0, const char* file = nullptr);
void  operator delete[](void* ptr) noexcept;

#if defined(REMEM_ENABLE)
    #define new(who) new(who, __LINE__, __FILE__)
#else
    #define new(who) new
#endif

namespace re {
    struct AddressInfo {
        std::string who;
        size_t size;

        AddressInfo() { }
        AddressInfo(const char* w, size_t sz) : who(w), size(sz) { }
    };

    const std::unordered_map<void*, AddressInfo>& mem() noexcept;

    void   memPrint() noexcept;
    size_t memSize()  noexcept;

    void* alloc(size_t&  size, const char* who, size_t line = 0, const char* file = nullptr);
    void* alloc(size_t&& size, const char* who, size_t line = 0, const char* file = nullptr);

    void* expand(void* ptr, size_t& size, size_t line = 0, const char* file = nullptr);

    void  free(void* ptr) noexcept;
}

#endif