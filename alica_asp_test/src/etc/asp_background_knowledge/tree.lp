#program firstKnowledge.
node(1).

#program rule.
rootNode(N) :- node(N), node(M), not hasParent(N,_).
node(N) :- rootNode(N).
%hasParent(N,M) :- node(N), node(M), hasChild(M,N).


#program addKnowledge.
node(2).
hasParent(1,2).

#script (python)

import gringo
from gringo import Fun

def main(prg):
	prg.ground([("firstKnowledge", [])])
	prg.ground([("rule", [])])
	prg.solve()
	prg.ground([("addKnowledge", [])])
	prg.solve()
#end.
