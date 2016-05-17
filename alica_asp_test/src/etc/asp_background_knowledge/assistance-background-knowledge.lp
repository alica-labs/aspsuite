#program assistanceBackground.

% Facts for testing
book(harryPotter1).
book(harryPotter2).
book(harryPotter3).
agent(donatello).
agent(raphael).
agent(leonardo).
agent(michelangelo).
table(desk1).
on(harryPotter1, donatello).
on(harryPotter1, raphael).
on(harryPotter2, leonardo).
on(harryPotter3, leonardo).
on(desk1, michelangelo).

pb(in(donatello, p1462889426092, tsk1225112227903, s1462889438750)).
pb(in(donatello, p1462889426092, tsk1225112227903, s1463404627637)).
pb(in(raphael, p1462889426092, tsk1225112227903, s1462889438750)).
pb(in(leonardo, p1462889426092, tsk1225112227903, s1462889438750)).
pb(in(michelangelo, p1462889426092, tsk1225112227903, s1462889438750)).

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

