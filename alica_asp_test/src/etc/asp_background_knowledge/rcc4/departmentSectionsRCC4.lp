#program department_sections.

partialOverlapping(studentArea, mainHallA).
partialOverlapping(mainHallA, studentArea).

partialOverlapping(workshop, mainHallA).
partialOverlapping(mainHallA, workshop).

partialOverlapping(offices, mainHallA).
partialOverlapping(mainHallA, offices).

partialOverlapping(offices, mainHallB).
partialOverlapping(mainHallB, offices).

partialOverlapping(offices, utility).
partialOverlapping(utility, offices).

partialOverlapping(mainHallA, mainHallB).
partialOverlapping(mainHallB, mainHallA).

partialOverlapping(mainHallB, utility).
partialOverlapping(utility, mainHallB).

partialOverlapping(mainHallB, organization).
partialOverlapping(organization, mainHallB).

partialOverlapping(utility, organization).
partialOverlapping(organization, utility).



disconnected(studentArea, mainHallB).
disconnected(mainHallB, studentArea).

disconnected(studentArea, workshop).
disconnected(workshop, studentArea).

disconnected(studentArea, offices).
disconnected(offices, studentArea).

disconnected(studentArea, utility).
disconnected(utility, studentArea).

disconnected(studentArea, organization).
disconnected(organization, studentArea).

disconnected(workshop, mainHallB).
disconnected(mainHallB, workshop).

disconnected(workshop, offices).
disconnected(offices, workshop).

disconnected(workshop, utility).
disconnected(utility, workshop).

disconnected(workshop, organization).
disconnected(organization, workshop).

disconnected(offices, organization).
disconnected(organization, offices).

disconnected(mainHallA, utility).
disconnected(utility, mainHallA).

disconnected(mainHallA, organization).
disconnected(organization, mainHallA).

