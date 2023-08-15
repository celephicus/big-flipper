#!/usr/bin/python3

import sys, time, random, serial

TEST_WORDS = '''\
AAAA BBBB CCCC
BBBB AAAA
CCCC AAAA BBBB
'''

PORT = sys.argv[1]
s = serial.Serial(PORT, baudrate=115200)

period = int(sys.argv[2])

raw_words = open(sys.argv[3], 'rt').read() if len(sys.argv) > 3 else TEST_WORDS

words = {}
for ln in raw_words.splitlines():
	ww = ln.split()
	words[ww[0]] = ww[1:]

w = random.choice(list(words.keys()))
while 1:
	s.write(f'"{w} disp\r'.encode('ascii'))
	print(w)
	time.sleep(period)
	w = random.choice(words[w])
