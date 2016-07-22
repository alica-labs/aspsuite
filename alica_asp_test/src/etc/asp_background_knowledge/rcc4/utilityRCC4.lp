#program utility_lp.

%%%% the utility area consists of the rooms 1405 1405A 1405B and 1403C
properPart(r1405, utility).
properPart(r1405A, utility).
properPart(r1405B, utility).
properPart(r1403C, utility).

%%%% the rooms which are part of the utility area are in relation to each other
partialOverlapping(r1405, r1405A).
partialOverlapping(r1405A, r1405B).
partialOverlapping(r1405A, r1403C).
disconnected(r1410A, r1410B).
disconnected(r1410A, r1410B).
disconnected(r1410A, r1410B).
