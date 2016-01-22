#script (python)

import gringo
from gringo import Fun

def main(prg):
	prg.conf.solve.models = 0
	prg.ground([("firstKnowledge", [])])
	prg.ground([("rule", [])])
	prg.solve()
	prg.ground([("addKnowledge", [])])
	prg.solve()
	#prg.ground([("rule", [])])
	#prg.solve()
	#prg.ground([("test1", [])])
	#prg.ground([("alicaBackground", [])])
	#prg.solve()
	#prg.ground([("test2", [])])
	#prg.solve()
#end.
