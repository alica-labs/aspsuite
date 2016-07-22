#program distributed_systems_rooms.

%%%%% Discribes the rcc8 relations between the rooms in distributed systems

%%%%% rooms are par of a section

%%%% studentArea
partialOverlapping(studentArea, mainHallA).
disconnected(studentArea, mainHallB).
disconnected(studentArea, workshop).
disconnected(studentArea, offices).
disconnected(studentArea, utility).
disconnected(studentArea, organization).

%%%% the studentArea consists of the rooms 1411 1411A and 1411C
properPart(r1411, studentArea).
properPart(r1411A, studentArea).
properPart(r1411C, studentArea).

%%%% the rooms which are part of the studentArea are in relation to each other
partialOverlapping(r1411, r1411C).
partialOverlapping(r1411A, r1411C).
disconnected(r1411, r1411A).

%%%% workshop
partialOverlapping(workshop, mainHallA).
disconnected(studentArea, mainHallB).
disconnected(workshop, studentArea).
disconnected(workshop, offices).
disconnected(workshop, utility).
disconnected(workshop, organization).

%%%% the workshop consists of the rooms 1410 1410A and 1410B
properPart(r1410, workshop).
properPart(r1410A, workshop).
properPart(r1410B, workshop).

%%%% the rooms which are part of the workshop are in relation to each other
partialOverlapping(r1410, r1410A).
partialOverlapping(r1410, r1410B).
disconnected(r1410A, r1410B).

%%%% offices
partialOverlapping(offices, mainHallA).
partialOverlapping(offices, mainHallB).
partialOverlapping(offices, utility).
disconnected(offices, workshop).
disconnected(offices, studentArea).
disconnected(offices, organization).

%%%% the offices consists of the rooms 1407 1408A 1408B 1408C 1406A 1406B 1406C
properPart(r1407, offices).
properPart(r1408A, offices).
properPart(r1408B, offices).
properPart(r1408C, offices).
properPart(r1406A, offices).
properPart(r1406B, offices).
properPart(r1406C, offices).

%%%% the rooms which are part of the offices are in relation to each other
partialOverlapping(r1407, r1408A).
partialOverlapping(r1407, r1408B).
partialOverlapping(r1407, r1408C).
partialOverlapping(r1407, r1406A).
partialOverlapping(r1407, r1406B).
partialOverlapping(r1407, r1406C).
disconnected(r1408A, r1408B).
disconnected(r1408A, r1408C).
disconnected(r1408A, r1406A).
disconnected(r1408A, r1406B).
disconnected(r1408A, r1406C).
disconnected(r1408B, r1408C).
disconnected(r1408B, r1406A).
disconnected(r1408B, r1406B).
disconnected(r1408B, r1406C).
disconnected(r1408C, r1406A).
disconnected(r1408C, r1406B).
disconnected(r1408C, r1406C).
disconnected(r1406A, r1406B).
disconnected(r1406A, r1406C).
disconnected(r1406B, r1406C).

%%%% mainHallA only consists of room 1401A
partialOverlapping(mainHallA, studentArea).
partialOverlapping(mainHallA, mainHallB).
partialOverlapping(mainHallA, workshop).
partialOverlapping(mainHallA, offices).
disconnected(mainHallA, utility).
disconnected(mainHallA, organization).

equal(r1401A, mainHallA).

%%%% mainHallB only consists of 1401B
partialOverlapping(mainHallB, mainHallA).
partialOverlapping(mainHallB, offices).
partialOverlapping(mainHallB, utility).
partialOverlapping(mainHallB, organization).
disconnected(mainHallB, studentArea).
disconnected(mainHallB, workshop).

equal(r1401B, mainHallB).

%%%% utility
partialOverlapping(utility, mainHallB).
partialOverlapping(utility, organization).
partialOverlapping(utility, offices).
disconnected(utility, workshop).
disconnected(utility, studentArea).
disconnected(utility, mainHallA).

%%%% the utility area consists of the rooms 1405 1405A 1405B and 1403C
properPart(r1405, utility).
properPart(r1405A, utility).
properPart(r1405B, utility).
properPart(r1403C, utility).

%%%% the rooms which are part of the utility area are in relation to each other
partialOverlapping(r1405, r1405A).
partialOverlapping(r1405A, r1405B).
partialOverlapping(r1405A, r1403C).
disconnected(r1410A, r1410B).
disconnected(r1410A, r1410B).
disconnected(r1410A, r1410B).

%%%% organization
partialOverlapping(organization, mainHallB).
partialOverlapping(organization, utility).
disconnected(organization, mainHallA).
disconnected(organization, workshop).
disconnected(organization, offices).
disconnected(organization, studentArea).

%%%% the organization area consists of the rooms 1403 1403A and 1403B
properPart(r1403, organization).
properPart(r1403A, organization).
properPart(r1403B, organization).

%%%% the rooms which are part of the organization area are in relation to each other
partialOverlapping(r1403A, r1403B).
partialOverlapping(r1403B, r1403).
disconnected(r1403A, r1403).

%%%% relations between rooms and POIs

%%% 1411
properPart(studentLab1, r1411).
properPart(kicker, r1411).
properPart(studentLab2, r1411).
properPart(studentLabDoor1, r1411).

%%%% 1411C
properPart(floor1, r1411C).
properPart(studentLabDoor2, r1411C).
properPart(hiWiDoor1, r1411C).

%%%% 1411A
properPart(hiwiroom, r1411A).
properPart(hiWiDoor2, r1411A).

%%%% 1401A
properPart(floor1Door1, r1401A).
properPart(floor1Door2, r1401A).
properPart(floor1Door3, r1401A).
properPart(floor1Door4, r1401A).

%%%% 1401B
properPart(floor3Door1, r1401B).
properPart(floor3Door2, r1401B).
properPart(floor3Door3, r1401B).
properPart(floor3Door4, r1401B).

%%%% 1410
properPart(workShopDoor1, r1410).
properPart(workShopDoor2, r1410).

%%%% 1407
properPart(floor2Door1, r1407).
properPart(floor2Door2, r1407).
properPart(floor2Door3, r1407).
properPart(floor2Door4, r1407).
properPart(floor2Door5, r1407).
properPart(floor2Door6, r1407).
properPart(floor2Door7, r1407).
properPart(floor2Door8, r1407).
properPart(floor2Door9, r1407).

%%%% 1410A
properPart(workShopDoor3, r1410A).
properPart(workShopright, r1410A).

%%%% 1410B
properPart(workShopDoor4, r1410B).
properPart(workShopLeft, r1410B).

%%%% 1408A
properPart(room1408A, r1408A).

%%%% 1408B
properPart(room1408BFront, r1408B).
properPart(room1408BWindow, r1408B).

%%%% 1408C
properPart(room1408CFront, r1408C).
properPart(room1408CWindow, r1408C).

%%%% 1406A
properPart(room1406AFront, r1406A).
properPart(room1406AWindow, r1406A).

%%%% 1406B
properPart(room1406BFront, r1406B).
properPart(room1406BWindow, r1406B).

%%%% 1406C
properPart(room1406C, r1406C).

%%%% 1405B
properPart(kitchen, r1405B).

%%%% 1405A
properPart(floor4, r1405A).
properPart(floor4Door1, r1405A).
properPart(floor4Door2, r1405A).

%%%% 1405
properPart(meetingroom, r1405).

%%%% 1403C
properPart(copyroom, r1403C).
properPart(copyroomDoor1, r1403C).
properPart(copyroomDoor2, r1403C).

%%%% 1403A
properPart(secreteriatDoor1, r1403A).
properPart(secreteriatDoor2, r1403A).
properPart(secreteriatDoor3, r1403A).
properPart(secreteriat, r1403A).

%%%% 1403
properPart(room1403Door, r1403).
properPart(room1403, r1403).
