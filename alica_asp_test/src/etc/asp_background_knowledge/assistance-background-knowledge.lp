#program alicaBackground.

% General Knowlege in an Assistance Domain

%Each thing is an object.
carriableObject(O) :- book(O).
carriableObject(O) :- cup(O).
carriableObject(O) :- glas(O).
object(O) :- carriableObject(O).
object(O) :- table(O).

%An agent carries an object when its on top of it
carries(A, O) :- agent(A), carriableObject(O), on(O, A).

%An agent can carry at most one object
overload(A) :- agent(A), ObjectCount = #count{O : on(O, A), carriableObject(O)}, ObjectCount > 1.

%An object can be carried by at most one agent.
multipleCarry(O) :- carriableObject(O), AgentCount = #count{A : on(O, A), agent(A)}, AgentCount > 1.

% Facts for testing
book(b).
agent(a).
on(b, a).

