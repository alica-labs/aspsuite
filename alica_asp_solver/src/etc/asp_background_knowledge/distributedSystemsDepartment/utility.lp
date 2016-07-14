#program utility_lp.

%%%% the utility area consists of the rooms 1405 1405A 1405B and 1403C
tangentialProperPart(r1405, utility).
tangentialProperPart(r1405A, utility).
tangentialProperPart(r1405B, utility).
tangentialProperPart(r1403C, utility).

%%%% the rooms which are part of the utility area are in relation to each other
externallyConnected(r1405, r1405A).
externallyConnected(r1405A, r1405B).
externallyConnected(r1405A, r1403C).
disconnected(r1410A, r1410B).
disconnected(r1410A, r1410B).
disconnected(r1410A, r1410B).