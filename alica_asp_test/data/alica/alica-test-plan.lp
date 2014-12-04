% Declare atoms
state(s1;s2).
plan(p1;p2).
transition(t1;t2).

% Declare relations
isStateOf(s1,p1).
isStateOf(s2,p1).
hasInState(t1, s1).
hasOutState(t1, s2).

