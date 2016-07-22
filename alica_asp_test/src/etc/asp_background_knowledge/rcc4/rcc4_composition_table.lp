#program rcc4_composition_table.

%%%%% General relations between rcc4 relations
%%%%% rcc4 is a stripped down version of rcc8
%%% externallyConnected and partialOverlapping are replaced by partialOverlapping
%%% tangentialProperPart, nonTangentialProperPart and equal are replaced by properPart
%%% inverseNonTangentialProperPart and inverseTangentialProperPart are replaced by inverseProperPart
%%% disconnected is NOT changed
%%% source http://qsrlib.readthedocs.io/en/latest/rsts/handwritten/qsrs/rcc4.html


%%%%% Variables X, Y, Z

%%%disconnected(X, Y) to other relations 
1 { disconnected(X, Z) ; partialOverlapping(X, Z) ; properPart(X, Z) ; inverseProperPart(X, Z) } 1 :- disconnected(X, Y), disconnected(Y, Z), X != Z.
1 { disconnected(X, Z) ; partialOverlapping(X, Z) ; properPart(X, Z) } 1 :- disconnected(X, Y), partialOverlapping(Y, Z), X != Z.
1 { disconnected(X, Z) ; partialOverlapping(X, Z) ; properPart(X, Z) } 1 :- disconnected(X, Y), properPart(Y, Z), X != Z.
1 { disconnected(X, Z) } 1 :- disconnected(X, Y), inverseProperPart(Y, Z), X != Z.

%%%partialOverlapping(X, Y) to other relations
1 { disconnected(X, Z) ; partialOverlapping(X, Z) ; inverseProperPart(X, Z) } 1 :- partialOverlapping(X, Y), disconnected(Y, Z), X != Z.
1 { disconnected(X, Z) ; partialOverlapping(X, Z) ; properPart(Y, Z) ; inverseProperPart(X, Z) } 1 :- partialOverlapping(X, Y), partialOverlapping(Y, Z), X != Z.
1 { partialOverlapping(X, Z) ; properPart(X, Z) } 1 :- partialOverlapping(X, Y), properPart(Y, Z), X != Z.
1 { disconnected(X, Z) ; partialOverlapping(X, Z) ; inverseProperPart(X, Z) } 1 :- partialOverlapping(X, Y), inverseProperPart(Y, Z), X != Z.

%%%properPart(X, Y) to other relations
1 { disconnected(X, Z) } 1 :- properPart(X, Y), disconnected(Y, Z), X != Z.
1 { disconnected(X, Z) ; partialOverlapping(X, Z) ; properPart(X, Z) } 1 :- properPart(X, Y), partialOverlapping(Y, Z), X != Z.
1 { properPart(X, Z) } 1:- properPart(X, Y), properPart(Y, Z), X != Z.
1 { disconnected(X, Z) ; partialOverlapping(X, Z) ; properPart(X, Z) ; inverseProperPart(X, Z) } 1 :- properPart(X, Y), inverseProperPart(Y, Z), X != Z.

%%%inverseProperPart(X, Y) to other relations
1 { disconnected(X, Z) ; partialOverlapping(X, Z) ; inverseProperPart(X, Z) } 1 :- inverseProperPart(X, Y), disconnected(Y, Z), X != Z.
1 { partialOverlapping(X, Z) ; inverseProperPart(X, Z) } 1 :- inverseProperPart(X, Y), partialOverlapping(Y, Z), X != Z.
1 { partialOverlapping(X, Z) ; properPart(X, Z) ; inverseProperPart(X, Z) } 1 :- inverseProperPart(X, Y), properPart(Y, Z), X != Z.
1 { inverseProperPart(X, Z) } 1 :- inverseProperPart(X, Y), inverseProperPart(Y, Z), X != Z.
