import socketserver
from http.server import SimpleHTTPRequestHandler
from urllib.parse import urlparse
from urllib.parse import parse_qs
import os, json

class MyHandler(SimpleHTTPRequestHandler):
  # def __init__(self, *args, **kwargs):
    # super().__init__(*args, directory=os.getcwd(), **kwargs)

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
      enum = os.scandir(os.getcwd() + '\\sd\\locos')
      fns = []
      for fn in enum:
        if fn.is_file() and fn.name.endswith(".json"):
          with open(fn.path, "r") as read_file:
            data = json.load(read_file)
            fns.append({ 'file': '/locos/' + fn.name, 'name': data['name'] })
      self.json(json.dumps(fns))
      return
    elif path == '/fns':
      enum = os.scandir(os.getcwd() + '\\sd\\fns')
      fns = []
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

      f = open(os.getcwd() + '\\sd' + path, 'wb')
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
