import socketserver
from http.server import SimpleHTTPRequestHandler
from urllib.parse import urlparse
from urllib.parse import parse_qs
import os, json

class MyHandler(SimpleHTTPRequestHandler):
  def translate_path(self, path):
    if os.path.exists(os.getcwd() + '\\data\\www\\' + path):
      return os.getcwd() + '\\data\\www\\' + path
    elif os.path.exists(os.getcwd() + '\\sd\\' + path):
      return os.getcwd() + '\\sd\\' + path
    else:
      return ''

  def json(self, json):
    self.send_response(200)
    self.send_header('Content-Type', 'application/json')
    self.end_headers()
    self.wfile.write(bytes(json, 'utf-8'))

  def do_GET(self):
    parsed = urlparse(self.path)
    path = parsed.path
  
    if path == '/':
      path = 'index.html'

    if path == '/locos':
      locos = []
      path = os.getcwd() + '\\sd\\locos'
      if os.path.exists(path):
        enum = os.scandir(path)
        for loco in enum:
          if loco.is_file() and loco.name.endswith(".json"):
            with open(loco.path, "r") as read_file:
              data = json.load(read_file)
              locos.append({ 'file': '/locos/' + loco.name, 'name': data['name'] })
      self.json(json.dumps(locos))
      return
    elif path == '/fns':
      fns = []
      path = os.getcwd() + '\\sd\\fns'
      if os.path.exists(path):
        enum = os.scandir(path)
        for fn in enum:
          if fn.is_file() and fn.name.endswith(".json"):
            with open(fn.path, "r") as read_file:
              data = json.load(read_file)
              fns.append({ 'file': '/fns/' + fn.name, 'name': data['name'] })
      self.json(json.dumps(fns))
      return
    elif path == '/icons':
      enum = os.scandir(os.getcwd() + "\\sd\\icons")
      icons = []
      for icon in enum:
        if icon.is_file() and icon.name.endswith('.bmp'):
          icons.append('icons/' + icon.name)
      enum.close()
      self.json(json.dumps(icons))
      return
    elif path == '/cs':
      self.json('{"ssid":"my ssid","password":"ssid pass","server":"dccex","port":2560}')
      return
    
    return SimpleHTTPRequestHandler.do_GET(self)

  def do_PUT(self):
    parsed = urlparse(self.path)
    path = parsed.path

    if path == '/cs':
      self.send_response(204)
      self.end_headers()
    else:
      content_length = int(self.headers.get('Content-Length'))
      body = self.rfile.read(content_length)

      path = os.getcwd() + '\\sd' + path
      os.makedirs(os.path.dirname(path), exist_ok=True)
    
      f = open(path, 'wb')
      f.write(body)
      f.close()

      self.send_response(204)
      self.end_headers()

  def do_HEAD(self):
    parsed = urlparse(self.path)
    path = parsed.path

    if path == '/cs': # Test to check if CS has settings, 204 for yes, 404 for no
      self.send_response(204)
      # self.send_response(404)
    elif os.path.exists(os.getcwd() + '\\sd' + path):
      self.send_response(204)
    else:
      self.send_response(404)
    self.end_headers()

  def do_DELETE(self):
    parsed = urlparse(self.path)
    path = parsed.path

    if not path.startswith('/icons/'): # avoid deleting icons
      deleted = True
      try:
        os.unlink(os.getcwd() + '\\sd' + path)
      except FileNotFoundError:
        deleted = False
      
      if deleted:
        self.send_response(204)
        self.end_headers()
        return

    self.send_response(404)
    self.end_headers()

httpd = socketserver.TCPServer(('', 8000), MyHandler)
httpd.serve_forever()
