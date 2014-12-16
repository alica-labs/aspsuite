%%%%% General relations between ALICA program elements

% A top-level plan is a plan, too.
plan(P) :- topLevelPlan(P).

% The must be one top-level plan
:- TopLevelPlanCnt = #count{P : topLevelPlan(P)}, TopLevelPlanCnt != 1.

% Transitivity of hasChildPlan
hasChildPlan(Parent, Child) :- plan(Parent), plan(Child), hasState(Parent, S), hasPlan(S, Child).
hasChildPlan(Parent, Child) :- plan(Parent), plan(Child), planType(PT), hasState(Parent, S), hasPlanType(S, PT), hasRealisation(PT, Child).
hasChildPlan(Parent, Child) :- hasChildPlan(Parent, MidPlan), hasChildPlan(MidPlan, Child).

% hasChildPlan is inverse to hasParentPlan
hasParentPlan(Child, Parent) :- plan(Parent), plan(Child), hasChildPlan(Parent, Child).

% Success and Failure States are subsets of Terminal States, which are subsets of States.
terminalState(S) :- successState(S).
terminalState(S) :- failureState(S).
state(S) :- terminalState(S).

% Inverse relations between predicates.
hasState(P,S) :- isStateOf(S,P).
hasTransition(P,T) :- isTransitionOf(T,P).


%%%%% Conditions for well-formed ALICA programs

% States belong to at most on plan.
brokenState(S) :- state(S), ParentPlansCnt = #count {P : isStateOf(S,P), plan(P)}, ParentPlansCnt > 1.

% No transition connects states in different plans.
brokenTransition(T) :- transition(T), state(SIN), state (SOut), plan(PIn) ,plan(POut), hasInState(T,SIn), hasOutState(T,SOut), isStateOf(SIn, PIn), isStateOf(SOut, POut), PIn != POut.

% Synchronisations only happen within a plan.
brokenSynchronisation(Synch) :- synchronisation(Synch), synchedTransition(Synch, T1), synchedTransition(Synch, T2), hasTransition(P2, T2), hasTransition(P1,T1), P1 != P2.

% The failure and success states of a plan are disjoint (subsets of its states).
brokenTerminalState(S) :- successState(S), failureState(S).
brokenTerminalState(S) :- hasTerminalState(P, S), not hasState(P,S).

% Terminal states do not include sub-plans or behaviours
brokenTerminalState(S) :- terminalState(S), hasPlan(S,P).

% There is a postcondition associated with each success and failure state. 
brokenTerminalState(S) :- terminalState(S), PostConditionCnt = #count{PostC : hasPostCondition(S,PostC), postCondition(PostC)}, PostConditionCnt != 1.

% A task associated with a plan identifies an initial state within that plan
brokenPlanTaskPair(P, T) :- plan(P), task(T), hasTask(P,T), EntryPointCnt = #count{E : hasEntryPoint(P,T,E), entryPoint(E)}, EntryPointCnt != 1.

% All plan-task pairs have a valid cardinality interval associated. 
brokenEntryPoint(E) :- entryPoint(E), MinCardinalityCnt = #count{Min : hasMinCardinality(E,Min)}, MinCardinalityCnt != 1.
brokenEntryPoint(E) :- entryPoint(E), MaxCardinalityCnt = #count{Max : hasMaxCardinality(E,Max)}, MaxCardinalityCnt != 1.
brokenEntryPoint(E) :- entryPoint(E), hasMinCardinality(E, Min), hasMaxCardinality(E, Max), Min > Max.

% The top-level plan is transitively connected to all other plans.
brokenPlan(P) :- plan(P), not topLevelPlan(P), HasChildCnt = #count{TOP: hasChildPlan(TOP,P), topLevelPlan(TOP)}, HasChildCnt != 1.

% Every plan has at most one parent.
brokenPlan(P) :- plan(P), ParentPlanCnt = #count{Parent : hasParentPlan(P,Parent), plan(Parent)}, ParentPlanCnt > 1.

% The transitive closure of the parent-child relationship is asymmetric.
brokenPlan(P) :- plan(P), hasChildPlan(P,P).

% The top level plan has no parent plans.
brokenPlan(P) :- topLevelPlan(P), ParentPlanCnt = #count{Parent : hasParentPlan(P,Parent), plan(Parent)}, ParentPlanCnt > 0. 










