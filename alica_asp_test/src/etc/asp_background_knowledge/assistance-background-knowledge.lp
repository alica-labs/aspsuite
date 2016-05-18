#program assistanceBackground.

% General Knowlege in an Robotic Assistance Domain (Service Robotic)

% Each entity is a thing
thing(O) :- largeThing(O).
thing(O) :- smallThing(O).
smallThing(O) :- book(O).
smallThing(O) :- cup(O).
smallThing(O) :- glas(O).
largeThing(O) :- table(O).

% An agent carries an object when its on top of it and its not too big to carry
carries(A, O) :- on(O, A), not largeThing(O).

% An agent can carry at most one object
overloaded(A) :- agent(A), ThingCount = #count{O : on(O, A)}, ThingCount > 1.

%An Agwnt can't carry a large object
overloaded(A) :- agent(A), on(O, A), largeThing(O).

% By default a thing is a small thing
smallThing(O) :- thing(O), not largeThing(O).

% A small object can only be carried by one agent.
inconsistent(O) :- smallThing(O), carries(A1, O), carries(A2, O), A1 != A2.

