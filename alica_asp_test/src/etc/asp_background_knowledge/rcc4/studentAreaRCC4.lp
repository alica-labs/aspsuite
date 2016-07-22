#program studentArea_lp.

%%%% the studentArea consists of the rooms 1411 1411A and 1411C
properPart(r1411, studentArea).
properPart(r1411A, studentArea).
properPart(r1411C, studentArea).

%%%% the rooms which are part of the studentArea are in relation to each other
partialOverlapping(r1411, r1411C).
partialOverlapping(r1411A, r1411C).
disconnected(r1411, r1411A).
