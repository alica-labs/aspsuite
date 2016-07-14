#program assistanceTestFacts.

%%%%% FACTS FOR TESTING %%%%%

% Domain Facts
book(harryPotter1; harryPotter2; harryPotter3).
agent(donatello; raphael; leonardo; michelangelo).
table(desk1).
on(harryPotter1, donatello).
on(harryPotter1, raphael).
on(harryPotter2, leonardo).
on(harryPotter3, leonardo).
on(desk1, michelangelo).

%%% Plan Base Facts for different tests

%For test AgentInTwoStatesOfSamePlan
in(donatello, rp14695971143742005608, tsk1225112227903, s1462889438750).
in(donatello, rp14695971143742005608, tsk1225112227903, s1463404627637).

%% For test ReusePlanFromPlantypeWithoutCycle_PlanBase
% in(donatello, ReusePlanFromPlantypeWithoutCycle, DefaultTask, PlantypeState)
in(donatello, rp11673136391232128661, tsk1225112227903, s1464355916689). 
% in(donatello, Min3AgentPlan, Attack, Min2AgentState) rp == Min3AgentPlan of PlanType
in(donatello, rp14663157318735871906, tsk1222613952469, s1453033506292).
% in(donatello, Min3AgentPlan, Defend, Min1AgentState) rp == Min3AgentPlan of State
in(donatello, rp14695979939835027958, tsk1225115406909, s1456483282661).

%For test multipleObjectCarry, overloaded, largeObject
in(raphael, rp18193619743855089282, tsk1225112227903, s1462889438750).
in(leonardo, rp18193619743855089282, tsk1225112227903, s1462889438750).
in(michelangelo, rp18250903200173254171, tsk1225112227903, s1462889438750).


% Facts for checking special plan properties

sound(p1462889118325).
