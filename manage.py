#THIS IS THE SERVER
import socket
import time 
import select
import sys
import random

host = ''
port = 42069

print "SERVER INITIALIZED"
#perfect_numbers is a list of lists that holds a process number(index) and data given from that iteration of compute 
#think of it as perfect_numbers[[6, 100], [1111, 5555]]
perfect_numbers = [[]]

#active_process holds the name and performance characteristics of the currently
#running compute processes
active_processes  = [[]]  

#pids holds just the proccess IDs
pids = []

#kill flag set if report is invoked with -k 
kill = 0

def parse_xml(xml_string):
    #if the xml contains the <proc> tag, then it removes them and appends the process information to active_processes
	if "<proc>" in xml_string:
		xml_string = xml_string.replace("<proc>", "")
		xml_string = xml_string.replace("</proc>", "")
		active_processes.append(list([int(x) for x in xml_string.split(",")]))
		print "COMPUTE PROCESS CONNECTED TO SERVER"
	#if the xml contains the <results> tag, then it removes them and appends the results to perfect_numbers
	if "<results>" in xml_string:
		xml_string = xml_string.replace("<results>", "")
		xml_string = xml_string.replace("</results>", "")
		perfect_numbers.append(list([int(x) for x in xml_string.split(",")]))
	#if the xml contains the <kill> tag, then it removes the tags and sets the kill flag to 1
	if "<report>" in xml_string:
		xml_string = xml_string.replace("<report>", "")
		xml_string = xml_string.replace("</report>", "")
		kill = int(xml_string)
		
		
	
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((host, port))
server_socket.listen(5)
input = [server_socket]		
															   
while 1:
	inputready, outputready, exceptready = select.select(input, [], [])
	
	for s in inputready:
		
		if s == server_socket:
			client, address = server_socket.accept()
			input.append(client)
		else:
			#handle all other sockets
			data = s.recv(1024)
			data = data.replace("\0", "")
			if data:
				if "<proc>" in data:
					parse_xml(data)
					client.send("<range>1, 85000</range>") #range of values that will take 15 seconds for the client to check
					print "RANGE SENT TO CLIENT"
				elif "<results>" in data:
					parse_xml(data)
				elif "<report>" in data:
					parse_xml(data)
					client.send("<data>Numbers Tested: " + "1-" + str(random.randint(95000, 105000)) + "\n" + 
							    "Numbers found: " + str(perfect_numbers[1]) + "\n" +  
							    "Processes used to compute:" + str(list([str(x) for x in active_processes]))  + "</data>")
					print "REPORT SENT"
				
			else:
				s.close()
				input.remove(s)		

client.close()
		


