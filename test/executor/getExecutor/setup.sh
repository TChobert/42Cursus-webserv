#!/bin/bash

# Reset permissions
chmod 644 forbidden_file.html 2>/dev/null

# Clean up test files
rm -f test_page.html
rm -f forbidden_file.html
rm -rf directory_test

# Re-create structure
echo "[INFO] Creating test files..."

mkdir -p directory_test

# Create test_page.html
cat > test_page.html <<EOF
<!DOCTYPE html>
<html>
<head><title>Test OK</title></head>
<body>
<h1>This is a test page</h1>
<p>Status: 200 OK</p>
</body>
</html>
EOF

# Create forbidden_file.html
cat > forbidden_file.html <<EOF
<!DOCTYPE html>
<html>
<head><title>Forbidden</title></head>
<body>
<h1>You should not read this</h1>
<p>Status: 403 Forbidden</p>
</body>
</html>
EOF

chmod 000 forbidden_file.html

echo "[INFO] Done. Test files are ready."
