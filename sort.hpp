/* BSD-2-Clause
 * Copyright 2018 Jason Dark
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

// C++17 implementation of compile-time ranked-order sorting, inspired by
// https://stackoverflow.com/questions/2786899/fastest-sort-of-fixed-length-6-int-array
//
// Usage: auto sorted = sort<N>(list_like_thing_of_at_least_size_N);
//
// Example:
//
// #include <iostream>
// #include "sort.h"
// using namespace std;
//
// int main(int argc, char **argv) {
//
//     constexpr int known[] = { 5, 7, 3, 1, -5, 9};
//     auto sorted = sort<6>(known);
//
//     int i = atoi(argv[1]);
//     auto result = sorted[i];
//
//     cout << result << endl;
//
// }

#include <array>
#include <type_traits>
using namespace std;

namespace {
    // Given a "list"-like type T (that implements operator[]), get the type of each element
    template <typename T> 
    using Atom = decay_t<decltype(declval<T>()[0])>;
    
    // Lower triangular portion of rank, e.g. i > j, where j in {0, 1, ..., i-1}
    template <size_t I, typename T, size_t... J>
    constexpr size_t sort_rank_l(T list, index_sequence<J...>) { return ((list[I] >= list[J]) + ... + 0); }

    // Upper triangular portion of rank, e.g. i < 1+i+j, where 1+i+j in {i+1, i+2, ..., N-1}
    template <size_t I, typename T, size_t... J>
    constexpr size_t sort_rank_u(T list, index_sequence<J...>) { return ((list[I] > list[1+I+J]) + ... + 0); }

    // Actual implementation of compile-time sort
    template <size_t N, typename T, size_t... I >
    constexpr auto sort_impl(T list, index_sequence<I...>) {
        // Compute the ranks of each index
        size_t rank[] = { (sort_rank_l<I>(list, make_index_sequence<I>{}) + sort_rank_u<I>(list, make_index_sequence<N-I-1>{}))... };

        // Declare the output variable
        array<Atom<T>, N> response = {};

        // Copy the input into the output using rank-ordering
        ((response[rank[I]] = list[I]), ...);

        return response;
    }
}

// Entry point into the sort: we have to generate the index_sequence
template <size_t N, typename T>
constexpr auto sort(T list) { return sort_impl<N>(list, make_index_sequence<N>{}); }

