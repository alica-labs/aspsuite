#program organization_lp.

%%%% the organization area consists of the rooms 1403 1403A and 1403B
tangentialProperPart(r1403, organization).
tangentialProperPart(r1403A, organization).
tangentialProperPart(r1403B, organization).

%%%% the rooms which are part of the organization area are in relation to each other
externallyConnected(r1403A, r1403B).
externallyConnected(r1403B, r1403).
disconnected(r1403A, r1403).