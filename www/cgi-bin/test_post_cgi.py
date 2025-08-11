#!/usr/bin/env python3
import sys
import os  # <-- il faut importer os

def main():
    # Lire tout le body POST depuis stdin
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))  # <-- os.environ ici
    if content_length > 0:
        post_body = sys.stdin.read(content_length)
    else:
        post_body = ''

    # Envoyer la réponse
    sys.stdout.write("Content-Type: text/html\r\n\r\n")
    sys.stdout.write("<html><body><h1>CGI Output Test</h1>\n")
    sys.stdout.write(f"<p>Received POST data length: {len(post_body)}</p>\n")
    sys.stdout.write("</body></html>\n")
    sys.stdout.flush()

if __name__ == "__main__":
    main()


# #!/usr/bin/env python3
# import sys

# def main():
#     sys.stdout.write("Content-Type: text/html\r\n\r\n")
#     sys.stdout.write("<html><body><h1>CGI Output Test</h1>\n")
#     for i in range(5):
#         sys.stdout.write(f"<p>Chunk {i+1}</p>\n")
#     sys.stdout.write("</body></html>\n")
#     sys.stdout.flush()

# if __name__ == "__main__":
#     main()

# #!/usr/bin/env python3
# import sys
# import time

# def main():
#     # Header HTTP doit terminer par \r\n (car CGI est basé sur HTTP)
#     # et la séparation header/body doit être \r\n\r\n
#     # print ajoute par défaut \n, donc on le remplace par \r\n explicitement.

#     sys.stdout.write("Content-Type: text/html\r\n\r\n")
#     sys.stdout.flush()

#     sys.stdout.write("<html><body><h1>CGI Output Streaming Test</h1>\n")
#     sys.stdout.flush()

#     # Écrire par morceaux avec pause pour simuler streaming
#     for i in range(5):
#         sys.stdout.write(f"<p>Chunk {i+1}</p>\n")
#         sys.stdout.flush()
#         time.sleep(1)  # pause 1s

#     sys.stdout.write("</body></html>\n")
#     sys.stdout.flush()

# if __name__ == "__main__":
#     main()
