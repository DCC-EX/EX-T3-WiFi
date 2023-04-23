import socket
import random
import re
import time
from threading import Thread

def throttle(c: socket):
  while True:
    data = c.recv(1024)
    if not data:
      c.close()
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
        continue
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
        continue
      case "s":
        c.send(b"<iDCC-EX V-3.0.4 / MEGA / STANDARD_MOTOR_SHIELD G-75ab2ab><H 1 0><H 2 0><H 3 0><H 4 0><Y 52 0><q 53><q 50>\n")
        continue
      case "W":
        c.send(b"<w 4321>\n")
        continue
      case "R":
        c.send(b"<r 1423>\n")
        continue
      case "B":
        c.send(b"<r12345|32767|3 2 1 >\n")
        continue
      case "F":
        c.send(b"<l 1 1 146 7>\n")
        continue
      case "l":
        c.send(b"<l 1 1 0 0>\n")
        continue
      case "t":
        r = re.search("<t (?P<l>\d+) (?P<s>\d+) (?P<d>\d+)>", cmd)
        # r = re.search("<t (?P<l>\d+)>", str)
        if r:
          s = "<l " + r.group("l") + " 0 " + str((int(r.group("s")) + 1 & 0x7F) + int(r.group("d")) * 128) + " 0>\n"
          print(">> " + s)
          c.send(bytes(s, "raw_unicode_escape"))
          # c.send(bytes("<T " + r.group("l") + " " + r.group("s") + " " + r.group("d") + ">\n", "raw_unicode_escape"))
        else:
          c.send(b"<l 1 1 0 1>\n")
        continue
      case "U":
        c.send(b"<p1 MAIN>")
        continue
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
        continue
          # c.send(bytes("cab={cab}, speed={speed}, dir={dir}\n".format(cab=random.randint(0, 1024), speed=random.randint(0, 128), dir=random.choice(["F", "R"])), "raw_unicode_escape"))
        # c.send(bytes("Used={used}, max=50\n".format(used = r), "raw_unicode_escape"))

    c.send(b"<X>")

s = socket.socket()        
print("Socket successfully created")
 
port = 2560
 
s.bind(('', port))
print("socket binded to %s" %(port))

s.listen(5)    
print("socket is listening")           

while True:
  c, addr = s.accept()    
  print("Got connection from", addr)

  Thread(target = throttle, args = (c,)).start()
 