from collections import OrderedDict
import sys

words = OrderedDict()
links = OrderedDict()

RUDE = '''
	ANAL ANUS ARSE BOOB BRAS BUMS BUTT CAWK CLIT COCK COON CRAP CUNT DAGO DAMN DICK 
	DONG DYKE FAGS FART FUCK GOOK HELL JEWS JISM JIZM JIZZ KIKE MUFF NAZI NUDE PAKI 
	PISS POON POOP PORN RAPE SEXY SHIT SMUT SNOG SPIC SLUT TITS TURD TWAT WANK WOGS
	'''.split()

wordlist = open('words.txt', 'wt')
data = open('censored.txt', 'wt')
with open(sys.argv[1], 'rb') as f:
	while 1:
		p = f.tell()
		w = f.read(4).decode('ascii')
		if not w:
			break
		n = int.from_bytes(f.read(1), 'little')
		ro = [f.read(2) for x in range(n)]
		offsets = [int.from_bytes(r, 'big') for r in ro]
		words[p] = w
		links[w] = offsets
		end = f.read(2)
		assert(end == b'\xff\xff')

censored = {}
for w in links:
	offsets = links[w].copy()
	links[w] = [words[x] for x in offsets if words[x] not in RUDE]
	if w not in RUDE:
		print(f'{w} {" ".join(links[w])}', file=data)
		print(w, end=' ', file=wordlist)



		