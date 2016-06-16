#program rcc8_composition_table.

%%%%% General relations between rcc8 relations

%%%%% Variables X, Y, Z

%%%disconnected(X, Y) to other relations 
1 { disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) ; equal(X, Z) } 1 :- disconnected(X, Y), disconnected(Y, Z), X != Z.
1 { disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) } 1 :- disconnected(X, Y), externallyConnected(Y, Z), X != Z.
1 { disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) } 1 :- disconnected(X, Y), partialOverlapping(Y, Z), X != Z.
1 { disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) } 1 :- disconnected(X, Y), tangentialProperPart(Y, Z), X != Z.
1 { disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) } 1 :- disconnected(X, Y), nonTangentialProperPart(Y, Z), X != Z.
1 { disconnected(X, Z) } 1 :- disconnected(X, Y), inverseTangentialProperPart(Y, Z), X != Z.
1 { disconnected(X, Z) } 1 :- disconnected(X, Y), inverseNonTangentialProperPart(Y, Z), X != Z.
1 { disconnected(X, Z) } 1 :- disconnected(X, Y), equal(Y, Z), X != Z.

%%%externallyConnected(X, Y) to other relations
1 { disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) } 1 :- externallyConnected(X, Y), disconnected(Y, Z), X != Z.
1 { disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; inverseTangentialProperPart(X, Z) ; equal(X, Z) } 1 :- externallyConnected(X, Y), externallyConnected(Y, Z), X != Z.
1 { disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) } 1 :- externallyConnected(X, Y), partialOverlapping(Y, Z), X != Z.
1 { externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) } 1 :- externallyConnected(X, Y), tangentialProperPart(Y, Z), X != Z.
1 { partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) } 1 :- externallyConnected(X, Y), nonTangentialProperPart(Y, Z), X != Z.
1 { disconnected(X, Z) ; externallyConnected(X, Z) } 1 :- externallyConnected(X, Y), inverseTangentialProperPart(Y, Z), X != Z.
1 { disconnected(X, Z) } 1 :- externallyConnected(X, Y), inverseNonTangentialProperPart(Y, Z), X != Z.
1 { externallyConnected(X, Z) } 1 :- externallyConnected(X, Y), equal(Y, Z), X != Z.

%%%partialOverlapping(X, Y) to other relations
1 { disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) } 1 :- partialOverlapping(X, Y), disconnected(Y, Z), X != Z.
1 { disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) } 1 :- partialOverlapping(X, Y), externallyConnected(Y, Z), X != Z.
1 { disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) ; equal(X, Z) } 1 :- partialOverlapping(X, Y), partialOverlapping(Y, Z), X != Z.
1 { partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) } 1 :- partialOverlapping(X, Y), tangentialProperPart(Y, Z), X != Z.
1 { partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) } 1 :- partialOverlapping(X, Y), nonTangentialProperPart(Y, Z), X != Z.
1 { disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) } 1 :- partialOverlapping(X, Y), inverseTangentialProperPart(Y, Z), X != Z.
1 { disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) } 1 :- partialOverlapping(X, Y), inverseNonTangentialProperPart(Y, Z), X != Z.
1 { partialOverlapping(X, Z) } 1 :- partialOverlapping(X, Y), equal(Y, Z), X != Z.

%%%tangentialProperPart(X, Y) to other relations
1 { disconnected(X, Z) } 1 :- tangentialProperPart(X, Y), disconnected(Y, Z), X != Z.
1 { disconnected(X, Z) ; externallyConnected(X, Z) } 1 :- tangentialProperPart(X, Y), externallyConnected(Y, Z), X != Z.
1 { disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z)  } 1:- tangentialProperPart(X, Y), partialOverlapping(Y, Z), X != Z.
1 { tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z)  } 1:- tangentialProperPart(X, Y), tangentialProperPart(Y, Z), X != Z.
1 { nonTangentialProperPart(X, Z)  } 1:- tangentialProperPart(X, Y), nonTangentialProperPart(Y, Z), X != Z.
1 { disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; inverseTangentialProperPart(X, Z) ; equal(X, Z) } 1 :- tangentialProperPart(X, Y), inverseTangentialProperPart(Y, Z), X != Z.
1 { disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) } 1:- tangentialProperPart(X, Y), inverseNonTangentialProperPart(Y, Z), X != Z.
1 { tangentialProperPart(X, Z) } 1:- tangentialProperPart(X, Y), equal(Y, Z), X != Z.

%%%nonTangentialProperPart(X, Y) to other relations
1 { disconnected(X, Z) } 1 :- nonTangentialProperPart(X, Y), disconnected(Y, Z), X != Z.
1 { disconnected(X, Z) } 1 :- nonTangentialProperPart(X, Y), externallyConnected(Y, Z), X != Z.
1 { disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) } 1 :- nonTangentialProperPart(X, Y), partialOverlapping(Y, Z), X != Z.
1 { nonTangentialProperPart(X, Z) } 1 :- nonTangentialProperPart(X, Y), tangentialProperPart(Y, Z), X != Z.
1 { nonTangentialProperPart(X, Z) } 1 :- nonTangentialProperPart(X, Y), nonTangentialProperPart(Y, Z), X != Z.
1 { disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) } 1 :- nonTangentialProperPart(X, Y), inverseTangentialProperPart(Y, Z), X != Z.
1 { disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) ; equal(X, Z) } 1 :- nonTangentialProperPart(X, Y), inverseNonTangentialProperPart(Y, Z), X != Z.
1 { nonTangentialProperPart(X, Z) } 1 :- nonTangentialProperPart(X, Y), equal(Y, Z), X != Z.

%%%inverseTangentialProperPart(X, Y) to other relations
1 { disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) } 1 :- inverseTangentialProperPart(X, Y), disconnected(Y, Z), X != Z.
1 { externallyConnected(X, Z) ; partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) } 1 :- inverseTangentialProperPart(X, Y), externallyConnected(Y, Z), X != Z.
1 { partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) } 1 :- inverseTangentialProperPart(X, Y), partialOverlapping(Y, Z), X != Z.
1 { partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; inverseTangentialProperPart(X, Z) ; equal(X, Z) } 1 :- inverseTangentialProperPart(X, Y), tangentialProperPart(Y, Z), X != Z.
1 { partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) } 1 :- inverseTangentialProperPart(X, Y), nonTangentialProperPart(Y, Z), X != Z.
1 { inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) } 1 :- inverseTangentialProperPart(X, Y), inverseTangentialProperPart(Y, Z), X != Z.
1 { inverseNonTangentialProperPart(X, Z) } 1 :- inverseTangentialProperPart(X, Y), inverseNonTangentialProperPart(Y, Z), X != Z.
1 { inverseTangentialProperPart(X, Z) } 1 :- inverseTangentialProperPart(X, Y), equal(Y, Z), X != Z.

%%%inverseNonTangentialProperPart(X, Y) to other relations
1 { disconnected(X, Z) ; externallyConnected(X, Z) ; partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) } 1 :- inverseNonTangentialProperPart(X, Y), disconnected(Y, Z), X != Z.
1 { partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) } 1 :- inverseNonTangentialProperPart(X, Y), externallyConnected(Y, Z), X != Z.
1 { partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) } 1 :- inverseNonTangentialProperPart(X, Y), partialOverlapping(Y, Z), X != Z.
1 { partialOverlapping(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) } 1 :- inverseNonTangentialProperPart(X, Y), tangentialProperPart(Y, Z), X != Z.
1 { partialOverlapping(X, Z) ; tangentialProperPart(X, Z) ; nonTangentialProperPart(X, Z) ; inverseTangentialProperPart(X, Z) ; inverseNonTangentialProperPart(X, Z) ; equal(X, Z)  } 1:- inverseNonTangentialProperPart(X, Y), nonTangentialProperPart(Y, Z), X != Z.
1 { inverseNonTangentialProperPart(X, Z) } 1 :- inverseNonTangentialProperPart(X, Y), inverseTangentialProperPart(Y, Z), X != Z.
1 { inverseNonTangentialProperPart(X, Z) } 1 :- inverseNonTangentialProperPart(X, Y), inverseNonTangentialProperPart(Y, Z), X != Z.
1 { inverseNonTangentialProperPart(X, Z) } 1 :- inverseNonTangentialProperPart(X, Y), equal(Y, Z), X != Z.

%%%equal(X, Y) to other relations
1 { disconnected(X, Z) } 1 :- equal(X, Y), disconnected(Y, Z), X != Z.
1 { externallyConnected(X, Z) } 1 :- equal(X, Y), externallyConnected(Y, Z), X != Z.
1 { partialOverlapping(X, Z) } 1 :- equal(X, Y), partialOverlapping(Y, Z), X != Z.
1 { tangentialProperPart(X, Z) } 1 :- equal(X, Y), tangentialProperPart(Y, Z), X != Z.
1 { nonTangentialProperPart(X, Z) } 1 :- equal(X, Y), nonTangentialProperPart(Y, Z), X != Z.
1 { inverseTangentialProperPart(X, Z) } 1 :- equal(X, Y), inverseTangentialProperPart(Y, Z), X != Z.
1 { inverseNonTangentialProperPart(X, Z) } 1 :- equal(X, Y), inverseNonTangentialProperPart(Y, Z), X != Z.
1 { equal(X, Z) } 1 :- equal(X, Y), equal(Y, Z), X != Z.