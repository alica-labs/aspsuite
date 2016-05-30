#program rcc8_composition_table.

%%%%% General relations between rcc8 relations

%%%%% Variables X, Y, Z

%%%disconnected(X, Y) to other relations 
disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) ; equal(X, Z) :- disconnected(X, Y), disconnected(Y, Z).
disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) :- disconnected(X, Y), externallyConnected(Y, Z).
disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) :- disconnected(X, Y), partialOverlapping(Y, Z).
disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) :- disconnected(X, Y), tangentialProperPart(Y, Z).
disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) :- disconnected(X, Y), nonTangentialProperPart(Y, Z).
disconnected(X, Z) :- disconnected(X, Y), inverseTangentialProperPart(Y, Z).
disconnected(X, Z) :- disconnected(X, Y), inverseNonTangentialProperPart(Y, Z).
disconnected(X, Z) :- disconnected(X, Y), equal(Y, Z).

%%%externallyConnected(X, Y) to other relations
disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) :- externallyConnected(X, Y), disconnected(Y, Z).
disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; inverseTangentialProperPart(X, Z) ; equal(X, Z) :- externallyConnected(X, Y), externallyConnected(Y, Z).
disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) :- externallyConnected(X, Y), partialOverlapping(Y, Z).
externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) :- externallyConnected(X, Y), tangentialProperPart(Y, Z).
partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) :- externallyConnected(X, Y), nonTangentialProperPart(Y, Z).
disconnected(X, Z) ; externallyConnected(X, Z) :- externallyConnected(X, Y), inverseTangentialProperPart(Y, Z).
disconnected(X, Z) :- externallyConnected(X, Y), inverseNonTangentialProperPart(Y, Z).
externallyConnected(X, Z) :- externallyConnected(X, Y), equal(Y, Z).

%%%partialOverlapping(X, Y) to other relations
disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) :- partialOverlapping(X, Y), disconnected(Y, Z).
disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) :- partialOverlapping(X, Y), externallyConnected(Y, Z).
disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) ; equal(X, Z) :- partialOverlapping(X, Y), partialOverlapping(Y, Z).
partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) :- partialOverlapping(X, Y), tangentialProperPart(Y, Z).
partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) :- partialOverlapping(X, Y), nonTangentialProperPart(Y, Z).
disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) :- partialOverlapping(X, Y), inverseTangentialProperPart(Y, Z).
disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) :- partialOverlapping(X, Y), inverseNonTangentialProperPart(Y, Z).
partialOverlapping(X, Z) :- partialOverlapping(X, Y), equal(Y, Z).

%%%tangentialProperPart(X, Y) to other relations
disconnected(X, Z) :- tangentialProperPart(X, Y), disconnected(Y, Z).
disconnected(X, Z) ; externallyConnected(X, Z) :- tangentialProperPart(X, Y), externallyConnected(Y, Z).
disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) :- tangentialProperPart(X, Y), partialOverlapping(Y, Z).
tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) :- tangentialProperPart(X, Y), tangentialProperPart(Y, Z).
nonTangentialProperPart(X, Z) :- tangentialProperPart(X, Y), nonTangentialProperPart(Y, Z).
disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; inverseTangentialProperPart(X, Z) ; equal(X, Z) :- tangentialProperPart(X, Y), inverseTangentialProperPart(Y, Z).
disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) :- tangentialProperPart(X, Y), inverseNonTangentialProperPart(Y, Z).
tangentialProperPart(X, Z) :- tangentialProperPart(X, Y), equal(Y, Z).

%%%nonTangentialProperPart(X, Y) to other relations
disconnected(X, Z) :- nonTangentialProperPart(X, Y), disconnected(Y, Z).
disconnected(X, Z) :- nonTangentialProperPart(X, Y), externallyConnected(Y, Z).
disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) :- nonTangentialProperPart(X, Y), partialOverlapping(Y, Z).
nonTangentialProperPart(X, Z) :- nonTangentialProperPart(X, Y), tangentialProperPart(Y, Z).
nonTangentialProperPart(X, Z) :- nonTangentialProperPart(X, Y), nonTangentialProperPart(Y, Z).
disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) :- nonTangentialProperPart(X, Y), inverseTangentialProperPart(Y, Z).
disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) ; equal(X, Z) :- nonTangentialProperPart(X, Y), inverseNonTangentialProperPart(Y, Z).
nonTangentialProperPart(X, Z) :- nonTangentialProperPart(X, Y), equal(Y, Z).

%%%inverseTangentialProperPart(X, Y) to other relations
disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) :- inverseTangentialProperPart(X, Y), disconnected(Y, Z).
externallyConnected(X, Z) ; partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) :- inverseTangentialProperPart(X, Y), externallyConnected(Y, Z).
partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) :- inverseTangentialProperPart(X, Y), partialOverlapping(Y, Z).
partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; inverseTangentialProperPart(X, Z) ; equal(X, Z) :- inverseTangentialProperPart(X, Y), tangentialProperPart(Y, Z).
partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) :- inverseTangentialProperPart(X, Y), nonTangentialProperPart(Y, Z).
inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) :- inverseTangentialProperPart(X, Y), inverseTangentialProperPart(Y, Z).
inverseNonTangentialProperPart(X, Z) :- inverseTangentialProperPart(X, Y), inverseNonTangentialProperPart(Y, Z).
inverseTangentialProperPart(X, Z) :- inverseTangentialProperPart(X, Y), equal(Y, Z).

%%%inverseNonTangentialProperPart(X, Y) to other relations
disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) :- inverseNonTangentialProperPart(X, Y), disconnected(Y, Z).
partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) :- inverseNonTangentialProperPart(X, Y), externallyConnected(Y, Z).
partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) :- inverseNonTangentialProperPart(X, Y), partialOverlapping(Y, Z).
partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) :- inverseNonTangentialProperPart(X, Y), tangentialProperPart(Y, Z).
partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) ; equal(X, Z):- inverseNonTangentialProperPart(X, Y), nonTangentialProperPart(Y, Z).
inverseNonTangentialProperPart(X, Z) :- inverseNonTangentialProperPart(X, Y), inverseTangentialProperPart(Y, Z).
inverseNonTangentialProperPart(X, Z) :- inverseNonTangentialProperPart(X, Y), inverseNonTangentialProperPart(Y, Z).
inverseNonTangentialProperPart(X, Z) :- inverseNonTangentialProperPart(X, Y), equal(Y, Z).

%%%equal(X, Y) to other relations
disconnected(X, Z) :- equal(X, Y), disconnected(Y, Z).
externallyConnected(X, Z) :- equal(X, Y), externallyConnected(Y, Z).
partialOverlapping(X, Z) :- equal(X, Y), partialOverlapping(Y, Z).
tangentialProperPart(X, Z) :- equal(X, Y), tangentialProperPart(Y, Z).
nonTangentialProperPart(X, Z) :- equal(X, Y), nonTangentialProperPart(Y, Z).
inverseTangentialProperPart(X, Z) :- equal(X, Y), inverseTangentialProperPart(Y, Z).
inverseNonTangentialProperPart(X, Z) :- equal(X, Y), inverseNonTangentialProperPart(Y, Z).
equal(X, Z) :- equal(X, Y), equal(Y, Z).