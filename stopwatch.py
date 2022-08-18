import serial, time, msvcrt

PORT = 'COM11'

REGS = (
	( 4, 1),	# Pixel update interval.
	( 5, 0),	# Update wipe to right.
	( 7, 0),	# No animation.
)
s = serial.Serial(PORT, baudrate=115200)
run = False
r = None

def getkey():
	return msvcrt.getch() if msvcrt.kbhit() else ''

for r_idx, r_val in REGS:
	s.write(f'{r_val} {r_idx} v!\r'.encode('ascii'))

while 1:
	s.write(b'"0:00 disp\r')
	print("RESET")
		
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
			print(f'{mins}:{secs:02}', end='\r')
			secs = '"%d:%02d disp\r' % (mins, secs)
			s.write(secs.encode('ascii'))
		if getkey(): break
		
	print("\nSTOPPED")

	while not getkey(): pass

		
