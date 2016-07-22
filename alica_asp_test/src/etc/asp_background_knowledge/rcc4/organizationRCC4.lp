#program organization_lp.

%%%% the organization area consists of the rooms 1403 1403A and 1403B
properPart(r1403, organization).
properPart(r1403A, organization).
properPart(r1403B, organization).

%%%% the rooms which are part of the organization area are in relation to each other
properPart(r1403A, r1403B).
properPart(r1403B, r1403).
disconnected(r1403A, r1403).
