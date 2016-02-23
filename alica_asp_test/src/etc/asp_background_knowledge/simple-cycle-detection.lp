#program alicaBackground.

%%%%% General relations between ALICA program elements

% Transitivity of hasParentPlan
hasParentPlan(Child, Parent) :- plan(Parent), plan(Child), hasState(Parent, S), hasPlan(S, Child).
hasParentPlan(Child, Parent) :- hasParentPlan(Child, MidPlan), hasParentPlan(MidPlan, Parent), Child!=MidPlan, MidPlan!=Parent.

sorted(P) :- plan(P), sorted(Pi):hasParentPlan(P,Pi).
brokenPlan(P) :- plan(P), not sorted(P).