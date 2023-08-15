import sys, time, random, serial

''''
AAAA: BBBB CCCC
BBBB: AAAA
CCCC: AAAA BBBB
'''

PORT = 'COM11'
s = serial.Serial(PORT, baudrate=115200)

words = {}

for ln in open(sys.argv[1], 'rt'):
	ww = ln.split()
	words[ww[0]] = ww[1:]

period = int(sys.argv[2])
	
w = random.choice(list(words.keys()))
while 1:
	s.write(f'"{w} disp\r'.encode('ascii'))
	print(w)
	time.sleep(period)
	w = random.choice(words[w])
	