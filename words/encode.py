from collections import OrderedDict
import sys

''''
AAAA: BBBB CCCC
BBBB: AAAA
CCCC: AAAA BBBB

0000:	AAAA \02 \000b \0014 \ffff	(11)
000b:	BBBB \01 \0000 \ffff		{9)
0014:	CCCC \02 \0000 \000b \ffff	(9)
'''

words = OrderedDict()

for ln in open(sys.argv[1], 'rt'):
	ww = ln.split()
	words[ww[0]] = ww[1:]
	
offset = 0
offsets = OrderedDict()
for w, ws in words.items():
	offsets[w] = offset
	offset += 4 + 1 + 2 * (len(ws) + 1)

print(offsets)
f_out = open('censored.dat', 'wb')
for w, ws in words.items():
	f_out.write(w.encode('ascii'))
	f_out.write(len(ws).to_bytes(1, 'big'))
	for w in ws:
		f_out.write(offsets[w].to_bytes(2, 'big'))
	f_out.write((int(0xffff)).to_bytes(2, 'big'))
	
		