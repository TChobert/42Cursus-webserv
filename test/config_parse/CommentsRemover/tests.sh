#!/bin/bash

make

before="======> Before removing:"
after="======> After removing:"

test_files=(
  "full_line_comments"
  "inline_comments"
  "no_comments"
  "test"
  "tricky"
  "tricky_with_semicolon"
  "test_edge_cases"
)

for file in "${test_files[@]}"; do
  echo -e "\n\n$before"
  cat -e "$file.txt"

  echo -e "\n$after"
  ./comments_remover_test "$file.txt" | cat -e

  echo -e "\nResult:"
  if diff <(./comments_remover_test "$file.txt") "$file.expected" > /dev/null; then
    echo "✅ $file test passed"
  else
    echo "❌ $file test failed"
    echo "🔍 Diff:"
    diff <(./comments_remover_test "$file.txt") "$file.expected"
  fi
done

echo -e "\n"
make fclean
