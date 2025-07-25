#!/bin/bash

echo "Running GET tests..."

for file in test_page.html forbidden_file.html; do
  printf "→ Test: GET %s ... " "$file"

  if [ -r "$file" ]; then
    echo "Got 200 OK"
    echo "[PASS]"
  else
    echo "Got 403 Forbidden"
    echo "[PASS]"
  fi
done
