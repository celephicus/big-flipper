#!/usr/bin/python3

W = 'CLIT ARSE COCK CRAP CUNT DICK FUCK PISS SHIT FART KNOB MUFF BALL SHAG TOSS BUTT POOP SLAG SLUT TURD'.split()

f = open('rude.txt', 'wt')
for w in W:
	print(w, ' '.join([x for x in W if x != w]), file=f)


