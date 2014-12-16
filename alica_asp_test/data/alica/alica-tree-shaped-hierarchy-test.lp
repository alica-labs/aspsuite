topLevelPlan(top).
plan(top; p1;p2;p3;p4;p5).
state(s1;s2;s3;s4;s5).
planType(pt1).

hasState(top, s1).
hasPlan(s1, p2).
hasState(p2, s2).
hasPlan(s2, p3).
hasState(p3, s3).
hasPlan(s3, p2).

hasState(top, s4).
hasPlanType(s4, pt1).
hasRealisation(pt1, p4).
hasState(p4, s5).
hasPlan(s5, p5).
hasState(p5, s6).
hasPlan(s6, p4).


% Reasons for brokenPlans
% plans p2 and p3 have a cycle
% plan p1 is not connected to the top-level plan
% plan p4 has a cycle over pt1
