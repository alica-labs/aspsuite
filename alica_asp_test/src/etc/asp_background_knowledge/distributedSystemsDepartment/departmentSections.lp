#program department_sections.

externallyConnected(studentArea, mainHallA).
%externallyConnected(mainHallA, studentArea).

externallyConnected(workshop, mainHallA).
%externallyConnected(mainHallA, workshop).

externallyConnected(offices, mainHallA).
%externallyConnected(mainHallA, offices).

externallyConnected(offices, mainHallB).
%externallyConnected(mainHallB, offices).

externallyConnected(offices, utility).
%externallyConnected(utility, offices).

externallyConnected(mainHallA, mainHallB).
%externallyConnected(mainHallB, mainHallA).

externallyConnected(mainHallB, utility).
%externallyConnected(utility, mainHallB).

externallyConnected(mainHallB, organization).
%externallyConnected(organization, mainHallB).

externallyConnected(utility, organization).
%externallyConnected(organization, utility).



disconnected(studentArea, mainHallB).
%disconnected(mainHallB, studentArea).

disconnected(studentArea, workshop).
%disconnected(workshop, studentArea).

disconnected(studentArea, offices).
%disconnected(offices, studentArea).

disconnected(studentArea, utility).
%disconnected(utility, studentArea).

disconnected(studentArea, organization).
%disconnected(organization, studentArea).

disconnected(workshop, mainHallB).
%disconnected(mainHallB, workshop).

disconnected(workshop, offices).
%disconnected(offices, workshop).

disconnected(workshop, utility).
%disconnected(utility, workshop).

disconnected(workshop, organization).
%disconnected(organization, workshop).

disconnected(offices, organization).
%disconnected(organization, offices).

disconnected(mainHallA, utility).
%disconnected(utility, mainHallA).

disconnected(mainHallA, organization).
%disconnected(organization, mainHallA).

