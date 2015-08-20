topLevelPlan(p1).

%%%% Broken State (4.2)
state(s1).
plan(p1;p2).
isStateOf(s1,p1).
isStateOf(s1,p2).

%%%% Broken Transition (4.3)
state(sIn;sOut).
transition(t).
plan(pIn;pOut).
hasInState(t,sIn).
hasOutState(t,sOut).
isStateOf(sIn, pIn).
isStateOf(sOut, pOut).

%%%% Broken Synchronisation (4.4)
synchronisation(synch).
transition(t1;t2).
synchedTransition(synch, t1).
synchedTransition(synch, t2).
plan(p3;p4).
isTransitionOf(t1, p3).
isTransitionOf(t2, p4).

%%%% Broken Terminal States (4.5)
successState(ts1;ts3;ts4;ts5).
postCondition(pc1;pc2).
failureState(ts1).
hasTerminalState(pTs2, ts2).
hasPlan(ts3, pTs3).
hasPostCondition(ts5, pc1).
hasPostCondition(ts5, pc2).

% Reasons for breaking terminal states:
% ts1 is success and failure state
% ts2 is a Terminal State of plan pTs2, but not a State of plan pTs2
% ts3 has a subplan
% ts4 has no post condition
% ts5 has two post conditions

%%%% Broken Task-Plan-Pair (4.6)
plan(p5).
task(task1).
hasTask(p5,task1).
plan(p6).
task(task2).
hasTask(p6,task2).
entryPoint(ep1,ep2).
hasEntryPoint(p6,task2,ep1).
hasEntryPoint(p6,task2,ep2).

% Reasons for broken Task-Plan-Pair
% (p5,task1) has no entry point
% (p6,task2) has two entry points assigned

%%%% Broken EntryPoints
entryPoint(e1;e2;e3).
hasMaxCardinality(e1, 2).
hasMinCardinality(e1, 3).
hasMaxCardinality(e2, 1).
hasMinCardinality(e3, 1).

% Reasons for breaking entry points:
% e1 max is smaller than min cardinality
% e2 has no min cardinality
% e3 has no max cardinality

