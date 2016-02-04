#program bird(n).
bird(n).
flies(X) :- bird(X), not -flies(X).
#external a(n).
-flies(X) :- a(X).

#script (python)

import gringo
from gringo import Fun

def main(prg):
	prg.ground([("bird", [2])])
	prg.ground([("backgroundKnowledge", [])])
	prg.solve()
	prg.assign_external(Fun("a", [2]), True)
	prg.solve()
#end.