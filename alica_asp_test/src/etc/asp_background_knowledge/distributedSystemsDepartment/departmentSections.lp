#program department_sections.

%%%% studentArea
externallyConnected(studentArea, mainHallA).
disconnected(studentArea, mainHallB).
disconnected(studentArea, workshop).
disconnected(studentArea, offices).
disconnected(studentArea, utility).
disconnected(studentArea, organization).

%%%% workshop
externallyConnected(workshop, mainHallA).
disconnected(studentArea, mainHallB).
disconnected(workshop, studentArea).
disconnected(workshop, offices).
disconnected(workshop, utility).
disconnected(workshop, organization).

%%%% offices
externallyConnected(offices, mainHallA).
externallyConnected(offices, mainHallB).
externallyConnected(offices, utility).
disconnected(offices, workshop).
disconnected(offices, studentArea).
disconnected(offices, organization).

%%%% mainHallA 
externallyConnected(mainHallA, studentArea).
externallyConnected(mainHallA, mainHallB).
externallyConnected(mainHallA, workshop).
externallyConnected(mainHallA, offices).
disconnected(mainHallA, utility).
disconnected(mainHallA, organization).

%%%% mainHallB 
externallyConnected(mainHallB, mainHallA).
externallyConnected(mainHallB, offices).
externallyConnected(mainHallB, utility).
externallyConnected(mainHallB, organization).
disconnected(mainHallB, studentArea).
disconnected(mainHallB, workshop).

%%%% utility
externallyConnected(utility, mainHallB).
externallyConnected(utility, organization).
externallyConnected(utility, offices).
disconnected(utility, workshop).
disconnected(utility, studentArea).
disconnected(utility, mainHallA).

%%%% organization
externallyConnected(organization, mainHallB).
externallyConnected(organization, utility).
disconnected(organization, mainHallA).
disconnected(organization, workshop).
disconnected(organization, offices).
disconnected(organization, studentArea).