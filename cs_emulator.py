import socket
import random
import re
import time

s = socket.socket()        
print("Socket successfully created")
 
port = 2560
 
s.bind(('', port))
print("socket binded to %s" %(port))

s.listen(5)    
print("socket is listening")           
 
c, addr = s.accept()    
print("Got connection from", addr)
 
while True:
  # TODO, has disconnected?
  data = c.recv(1024)
  cmd = data.decode()
  print("<< " + cmd)

  match cmd[1]:
    case "0":
      if len(cmd) > 3:
        if cmd[3] == "M":
          c.send(b"<p0 MAIN>\n")
        elif cmd[3] == "P":
          c.send(b"<p0 PROG>\n")
      else:
        c.send(b"<p0>\n")
    case "1":
      if len(cmd) > 3:
        if cmd[3] == "M":
          c.send(b"<p1 MAIN>\n")
        elif cmd[3] == "P":
          c.send(b"<p1 PROG>\n")
        elif cmd[3] == "J":
          c.send(b"<p1 JOIN>\n")
      else:
        c.send(b"<p1>\n")
    case "s":
      c.send(b"<iDCC-EX V-3.0.4 / MEGA / STANDARD_MOTOR_SHIELD G-75ab2ab><H 1 0><H 2 0><H 3 0><H 4 0><Y 52 0><q 53><q 50>\n")
    case "W":
      c.send(b"<w 4321>\n")
    case "R":
      c.send(b"<r 1423>\n")
    case "B":
      c.send(b"<r12345|32767|3 2 1 >\n")
    case "F":
      c.send(b"<l 1 1 146 7>\n")
    case "l":
      c.send(b"<l 1 1 0 0>\n")
    case "t":
      r = re.search("<t (?P<l>\d+) (?P<s>\d+) (?P<d>\d+)>", cmd)
      # r = re.search("<t (?P<l>\d+)>", str)
      if r:
        s = "<l " + r.group("l") + " 0 " + str((int(r.group("s")) + 1 & 0x7F) + int(r.group("d")) * 128) + " 0>\n"
        print(">> " + s)
        c.send(bytes(s, "raw_unicode_escape"))
        # c.send(bytes("<T " + r.group("l") + " " + r.group("s") + " " + r.group("d") + ">\n", "raw_unicode_escape"))
      else:
        c.send(b"<l 1 1 0 0>\n")
    case "U":
      c.send(b"<p1 MAIN>")
    case "D":
      r = random.randint(0, 50)
      r = 50
      print(r)
      print("\n")
      c.send(b"<##")
      for i in range(r):
        c.send(b" ")
        c.send(bytes("{cab}".format(cab=random.randint(0, 1024)), "raw_unicode_escape"))
      c.send(b">\n")
        # c.send(bytes("cab={cab}, speed={speed}, dir={dir}\n".format(cab=random.randint(0, 1024), speed=random.randint(0, 128), dir=random.choice(["F", "R"])), "raw_unicode_escape"))
      # c.send(bytes("Used={used}, max=50\n".format(used = r), "raw_unicode_escape"))

  c.send(b"<X>")
