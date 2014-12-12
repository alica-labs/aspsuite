%%%% Broken State
state(s1).
plan(p1;p2).
isStateOf(s1,p1).
isStateOf(s1,p2).

%%%% Broken Transition
state(sIn;sOut).
transition(t).
plan(pIn;pOut).
hasInState(t,sIn).
hasOutState(t,sOut).
isStateOf(sIn, pIn).
isStateOf(sOut, pOut).

%%%% Broken Synchronisation
synchronisation(synch).
transition(t1;t2).
synchedTransition(synch, t1).
synchedTransition(synch, t2).
plan(p3;p4).
isTransitionOf(t1, p3).
isTransitionOf(t2, p4).

%%%% Broken Terminal States
successState(ts1;ts3;ts4;ts5).
postCondition(pc1;pc2).
failureState(ts1).
hasTerminalState(pTs2, ts2).
hasSubPlan(ts3, pTs3).
hasPostCondition(ts5, pc1).
hasPostCondition(ts5, pc2).

