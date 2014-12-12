%%%%% General relations between ALICA program elements

% Success and Failure States are subsets of Terminal States, which are subsets of States.
terminalState(S) :- successState(S).
terminalState(S) :- failureState(S).
state(S) :- terminalState(S).

% Inverse relations between predicates.
hasState(P,S) :- isStateOf(S,P).
hasTransition(P,T) :- isTransitionOf(T,P).


%%%%% Conditions for well-formed ALICA programs

% States belong to at most on plan.
brokenState(S) :-state(S), ParentPlansCnt = #count {P : isStateOf(S,P), plan(P)}, ParentPlansCnt > 1.

% No transition connects states in different plans.
brokenTransition(T) :- transition(T), state(SIN), state (SOut), plan(PIn) ,plan(POut), hasInState(T,SIn), hasOutState(T,SOut), isStateOf(SIn, PIn), isStateOf(SOut, POut), PIn != POut.

% Synchronisations only happen within a plan.
brokenSynchronisation(Synch) :- synchronisation(Synch), synchedTransition(Synch, T1), synchedTransition(Synch, T2), hasTransition(P2, T2), hasTransition(P1,T1), P1 != P2.


% The failure and success states of a plan are disjoint (subsets of its states).
brokenTerminalState(S) :- successState(S), failureState(S).
brokenTerminalState(S) :- hasTerminalState(P, S), not hasState(P,S).

% Terminal states do not include sub-plans or behaviours
brokenTerminalState(S) :- terminalState(S), hasSubPlan(S,P).

% There is a postcondition associated with each success and failure state. 
brokenTerminalState(S) :- postCondition(PostC1), postCondition(PostC2), terminalState(S), hasPostCondition(S, PostC1), hasPostCondition(S,PostC2), PostC1 != PostC2.
%borkenTerminalState(S) :- terminalState(S), postCondition(PostC), not hasPostCondition(S, PostC).
borkenTerminalState(S) :- terminalState(S), 0 #count{PostC : hasPostCondition(S,PostC), postCondition(PostC)} 0.

%*

% A task associated with a plan identifies an initial state within that plan
1 { hasEntryPoint(P,T,E) : entryPoint(E) } 1 :- plan(P), task(T), hasTask(P,T).

% All plan-task pairs have a valid cardinality interval associated. 
:- entryPoint(E), hasMinCardinality(E, Min), hasMaxCardinality(E, Max), Min > Max.
:- entryPoint(E), COUNT = #count{Min : hasMinCardinality(E,Min)}, COUNT != 1.
:- entryPoint(E), COUNT = #count{Max : hasMaxCardinality(E,Max)}, COUNT != 1.

*%
