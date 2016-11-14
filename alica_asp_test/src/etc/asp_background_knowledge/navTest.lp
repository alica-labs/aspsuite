#program navTest.

#external externalqueryExternal1.

goalReachable(X) :- reachable(X, Y), goal(X), start(Y), room(X), room(Y), externalqueryExternal1.
goal(r1405B) :- externalqueryExternal1.
start(r1411) :- externalqueryExternal1.
room(r1405B).
room(r1411).
reachable(r1405B, r1411).

#program navTest2.

#external externalqueryExternal2.

goalReachable(X) :- reachable(X, Y), goal(X), start(Y), room(X), room(Y), externalqueryExternal2.
goal(r1405B) :- externalqueryExternal2.
start(r1411) :- externalqueryExternal2.
