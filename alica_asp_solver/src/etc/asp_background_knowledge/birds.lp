#program backgroundKnowledge.
flies(X) :- bird(X), not -flies(X).

#program fact1.
bird(tweety).

#program fact2.
-flies(tweety).

#script (python)

import gringo
from gringo import Fun

def main(prg):
	prg.ground([("fact1", [])])
	print("test1")
	prg.ground([("backgroundKnowledge", [])])
	print("test2")
	prg.solve()
	print("test3")
	prg.ground([("fact2", [])])
	print("test4")
	prg.solve()
	print("test5")
#end.
