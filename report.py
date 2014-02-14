#THIS IS A CLIENT
import socket
import sys
import getopt
import os

host = "128.193.37.168"  #raw_input("Server IP: ")
port = 42069

#list to hold the parsed report
parsed_report = [[]]

#kill flag used to send to the server
kill = 0
pids_to_kill = []

opts, args = getopt.getopt(sys.argv[1:], 'k')

for opt, arg in opts:
	if opt in ('-k'):
		kill = 1

def parse_xml(xml_string):
	if "<data>" in xml_string:
		xml_string = xml_string.replace("<data>", "")
		xml_string = xml_string.replace("</data>", "")
		a, b, c = xml_string.partition("compute:")
		c = c.replace("[", "")		
		c = c.replace("]", "")
		c = c.replace("'", "")
		c = c.replace(",", "")
		c = c.replace("630000000", "")
		c = c.replace(" ", "-")
		print a + b + c
		return c

#connecting to server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))
message = "<report>" + str(kill) + "</report>"
s.send(message)
data = s.recv(1024)
pids = parse_xml(data)

pids = pids.replace("-", " ")

for x in pids.split():
	pids_to_kill.append(int(x))
if kill == 1:
	for x in pids_to_kill:
		try:
			os.kill(x, 0)
			print "\nALL LIVE PROCESSES KILLED"
		except OSError:
			print "\nALL LIVE PROCESSES KILLED"
			break

s.close()












