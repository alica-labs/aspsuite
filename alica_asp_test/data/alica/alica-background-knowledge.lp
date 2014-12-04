%% Conditions for Well-Formed ALICA Programs

% States belong to at most on plan.
1 {isStateOf(S, P) : plan(P)} 1 :- state(S).

% No transition connects states in different plans.
1 {isStateOf(SOut, P) : plan(P)} 1 :- transition(T), hasInState(T,SIn), hasOutState(T,SOut), isStateOf(SIn, P).
1 {isStateOf(SIn, P) : plan(P)} 1 :- transition(T), hasInState(T,SIn), hasOutState(T,SOut),  isStateOf(SOut, P).

% Synchronisations only happen within a plan.
hasTransition(P, T) :- synchronisation(Synch), synchedTransition(Synch, T), hasSynchronisation(P, Synch).
hasTransition(P, T1) :- synchronisation(Synch), synchedTransition(Synch, T1), synchedTransition(Synch, T2), hasSynchronisation(P, Synch), hasTransition(P, T2).

% The failure and success states of a plan are disjoint subsets of its states.
-failureState(S) :- successState(S).
-successState(S) :- failureState(S).

% Terminal states do not include sub-plans or behaviours
:- terminalState(S), hasSubPlan(S,P).

% There is a postcondition associated with each success and failure state. 
1{ hasPostCondition(S,PostC) : postCondition(PostC)}1 :- terminalState(S).


%% Relations between Plan Elements

% Success and Failure States are Terminal States, which are States.
terminalState(S) :- successState(S).
terminalState(S) :- failureState(S).
state(S) :- terminalState(S).