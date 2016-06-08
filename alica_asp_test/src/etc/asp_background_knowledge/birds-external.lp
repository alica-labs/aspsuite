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
	print("test1")
	prg.solve()
	print("test2")
	prg.assign_external(Fun("a", [2]), True)
	print("test3")
	prg.solve()
	print("test4")
#end.
