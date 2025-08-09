#!/usr/bin/env python3
import os, sys, urllib

# Écrire explicitement CRLF pour être certain que host parse "\r\n\r\n"
sys.stdout.write("Content-Type: text/html\r\n")
# Exemple d'en-tête supplémentaire (optionnel)
sys.stdout.write("X-Test: my-cgi\r\n")
sys.stdout.write("\r\n")  # <-- IMPORTANT : sépare headers / body par CRLF CRLF

sys.stdout.write("<html><body>\r\n")
sys.stdout.write("<h1>CGI GET test</h1>\r\n")

sys.stdout.write("<p>REQUEST_METHOD: %s</p>\r\n" % os.environ.get("REQUEST_METHOD", ""))
sys.stdout.write("<p>QUERY_STRING: %s</p>\r\n" % os.environ.get("QUERY_STRING", ""))
sys.stdout.write("<p>SCRIPT_FILENAME: %s</p>\r\n" % os.environ.get("SCRIPT_FILENAME", ""))
sys.stdout.write("<p>PATH_INFO: %s</p>\r\n" % os.environ.get("PATH_INFO", ""))
sys.stdout.write("</body></html>\r\n")
