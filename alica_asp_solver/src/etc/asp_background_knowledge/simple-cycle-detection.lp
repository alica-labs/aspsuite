#program alicaBackground.

%%%%% General relations between ALICA program elements

% Transitivity of hasParentPlan
hasParentPlan(Child, Parent) :- plan(Parent), plan(Child), hasState(Parent, S), hasPlan(S, Child).
hasParentPlan(Child, Parent) :- hasParentPlan(Child, MidPlan), hasParentPlan(MidPlan, Parent), Child!=MidPlan, MidPlan!=Parent.

cycleFree(P) :- plan(P), cycleFree(Parent):hasParentPlan(P,Parent).
cyclicPlan(P) :- plan(P), not cycleFree(P).