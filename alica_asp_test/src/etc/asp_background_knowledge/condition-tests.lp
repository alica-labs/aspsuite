condition(cond1;cond2;cond3).
plan(p1;p2;p3).
agent(a1;a2;a3).
state(s1;s2;s3).
task(t1;t2).

hasBall(a1).
pb(in(a1,p1,t1,s1)).
in(a2,p2,t1,s2).

-local(cond1).

% test for unbound in relations
%unboundIn(in(A,P,T,S)) :- in(A,P,T,S), agent(A), plan(P), task(T), state(S).

% things in the plan base should make conditions true
inCond(X,in(A,P,T,S)) :- pb(in(A,P,T,S)), agent(A), plan(P), task(T), state(S), condition(X).

%pre and runtime plan conditions
local(C) :- inCond(C,in(A,P,T,S)), hasPreCondition(P,C).

%transition pre condition
local(C) :- inCond(C,in(A,P,T,X)), hasTransition(P,T), hasPreCondition(T,C).

holds(cond1) :- agent(A), hasBall(A), inCond(cond1,in(A,p1,t1,s1)).
holds(cond2) :- agent(A), hasBall(A), inCond(cond2,in(A,p2,t1,s2)).

%cup(red; blue; green; yellow; black; white).
%on(red,shelf).
%in(blue, cupboard).
%on((green;black), table).
%in((yellow;white), hand).
%-free(X):-cup(X),on(X, table).
%-free(X):-cup(X),in(X, hand).
%free(X):-cup(X), not -free(X).
