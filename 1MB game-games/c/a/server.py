import http.server
import socketserver

PORT = 8080
DIRECTORY = "www"
ip = "0.0.0.0"
class SolarisHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=DIRECTORY, **kwargs)
    
    # Custom Security Mod: Send a fake Server header
    def end_headers(self):
        self.send_header("Server", "Floppy-games/v0.0.0-indev")
        super().end_headers()

with socketserver.TCPServer(("", PORT), SolarisHandler) as httpd:
    print(f"[W] floppy Web Server live at port {PORT}")
    print("-ip",ip)
    httpd.serve_forever()
