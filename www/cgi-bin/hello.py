#!/usr/bin/env python3
import os, sys, cgi, cgitb

cgitb.enable()  # pour voir les erreurs CGI dans le navigateur

# Lire les paramètres GET
form = cgi.FieldStorage()
name = form.getfirst("name", "Anonymous")  # si aucun paramètre, on met "Anonymous"

# Headers CGI
sys.stdout.write("Content-Type: text/html\r\n")
sys.stdout.write("\r\n")  # séparation headers / body

# Corps HTML
sys.stdout.write("""<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <title>Hello CGI</title>
  <link rel="stylesheet" href="/style.css">
</head>
<body>
  <h1>Hello {name}!</h1>
</body>
</html>
""".format(name=name))
