#program workshop_lp.

%%%% the workshop consists of the rooms 1410 1410A and 1410B
properPart(r1410, workshop).
properPart(r1410A, workshop).
properPart(r1410B, workshop).

%%%% the rooms which are part of the workshop are in relation to each other
partialOverlapping(r1410, r1410A).
partialOverlapping(r1410, r1410B).
disconnected(r1410A, r1410B).
