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
	prg.ground([("backgroundKnowledge", [])])
	prg.solve()
	prg.ground([("fact2", [])])
	prg.solve()
#end.
