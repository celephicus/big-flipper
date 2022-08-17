import serial, time, msvcrt

PORT = 'COM11'

s = serial.Serial(PORT, baudrate=115200)
run = False
r = None

def getkey():
	return msvcrt.getch() if msvcrt.kbhit() else ''

print("STOPPED")
while 1:
	s.write(b'"0:00 disp\r')

	while not getkey(): pass
	print("RUNNING")
	
	tt = int(time.time())
	start = tt
	while 1:
		t = int(time.time())
		if t != tt:
			tt = t
			secs = (t - start) % 60
			mins = ((t - start - secs) // 60) % 10
			print(mins, secs)
			secs = '"%d:%02d disp\r' % (mins, secs)
			s.write(secs.encode('ascii'))
		if getkey(): break
		
	print("STOPPED")

	while not getkey(): pass

		
