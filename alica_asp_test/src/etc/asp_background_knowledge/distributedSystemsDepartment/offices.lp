#program offices_lp.

%%%% the offices consists of the rooms 1407 1408A 1408B 1408C 1406A 1406B 1406C
tangentialProperPart(r1407, offices).
tangentialProperPart(r1408A, offices).
tangentialProperPart(r1408B, offices).
tangentialProperPart(r1408C, offices).
tangentialProperPart(r1406A, offices).
tangentialProperPart(r1406B, offices).
tangentialProperPart(r1406C, offices).

%%%% the rooms which are part of the offices are in relation to each other
externallyConnected(r1407, r1408A).
externallyConnected(r1407, r1408B).
externallyConnected(r1407, r1408C).
externallyConnected(r1407, r1406A).
externallyConnected(r1407, r1406B).
externallyConnected(r1407, r1406C).
disconnected(r1408A, r1408B).
disconnected(r1408A, r1408C).
disconnected(r1408A, r1406A).
disconnected(r1408A, r1406B).
disconnected(r1408A, r1406C).
disconnected(r1408B, r1408C).
disconnected(r1408B, r1406A).
disconnected(r1408B, r1406B).
disconnected(r1408B, r1406C).
disconnected(r1408C, r1406A).
disconnected(r1408C, r1406B).
disconnected(r1408C, r1406C).
disconnected(r1406A, r1406B).
disconnected(r1406A, r1406C).
disconnected(r1406B, r1406C).