#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
build="$(mktemp -d "${TMPDIR:-/tmp}/cs106l-examples.XXXXXX")"
trap 'rm -rf "$build"' EXIT
compiler="${CXX:-c++}"
count=0
for source in "$root"/examples/*.cpp; do
    name="$(basename "$source" .cpp)"
    "$compiler" -std=c++17 -Wall -Wextra -Wpedantic -pthread "$source" -o "$build/$name"
    "$build/$name"
    printf 'PASS %s\n' "$name"
    count=$((count + 1))
done
printf 'All %s examples passed.\n' "$count"
