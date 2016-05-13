#program assistanceBackground.

% Facts for testing
book(harryPotter1).
agent(donatello, raphael, leonardo).
on(harryPotter1, a).

% General Knowlege in an Robotic Assistance Domain (Service Robotic)

% Each entity is a thing
thing(O) :- largeThing(O).
thing(O) :- smallThing(O).
thing(O) :- book(O).
thing(O) :- cup(O).
thing(O) :- glas(O).
thing(O) :- table(O).
largeThing(O) :- table(O).

% An agent carries an object when its on top of it
carries(A, O) :- on(O, A).

% An agent can carry at most one object
overloaded(A) :- agent(A), ThingCount = #count{O : on(O, A)}, ThingCount > 1.

% By default a thing is a small thing
smallThing(O) :- thing(O), non largeThing(O).

% A small object can only be carried by one agent.
inconsistent(O) :- smallThing(O), carries(A1, O), carries(A2, O), A1 != A2.
