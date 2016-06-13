#program distributed_systems_rooms.

%%%%% Discribes the rcc8 relations between the rooms in distributed systems

%%%%% rooms are par of a section

%%%% studentArea
externallyConnected(studentArea, mainHallA).
disconnected(studentArea, mainHallB).
disconnected(studentArea, workshop).
disconnected(studentArea, offices).
disconnected(studentArea, utility).
disconnected(studentArea, organization).

%%%% the studentArea consists of the rooms 1411 1411A and 1411C
tangentialProperPart(r1411, studentArea).
tangentialProperPart(r1411A, studentArea).
tangentialProperPart(r1411C, studentArea).

%%%% the rooms which are part of the studentArea are in relation to each other
externallyConnected(r1411, r1411C).
externallyConnected(r1411A, r1411C).
disconnected(r1411, r1411A).

%%%% workshop
externallyConnected(workshop, mainHallA).
disconnected(studentArea, mainHallB).
disconnected(workshop, studentArea).
disconnected(workshop, offices).
disconnected(workshop, utility).
disconnected(workshop, organization).

%%%% the workshop consists of the rooms 1410 1410A and 1410B
tangentialProperPart(r1410, workshop).
tangentialProperPart(r1410A, workshop).
tangentialProperPart(r1410B, workshop).

%%%% the rooms which are part of the workshop are in relation to each other
externallyConnected(r1410, r1410A).
externallyConnected(r1410, r1410B).
disconnected(r1410A, r1410B).

%%%% offices
externallyConnected(offices, mainHallA).
externallyConnected(offices, mainHallB).
externallyConnected(offices, utility).
disconnected(offices, workshop).
disconnected(offices, studentArea).
disconnected(offices, organization).

%%%% the offices consists of the rooms 1407 1408A 1408B 1408C 1406A 1406B 1406C
tangentialProperPart(r1407, offices).
tangentialProperPart(r1408A, offices).
tangentialProperPart(r1408B, offices).
tangentialProperPart(r1408C, offices).
tangentialProperPart(r1406A, offices).
tangentialProperPart(r1406B, offices).
tangentialProperPart(r1406C, offices).

%%%% the rooms which are part of the offices are in relation to each other
externallyConnected(r1407, r1408A).
externallyConnected(r1407, r1408B).
externallyConnected(r1407, r1408C).
externallyConnected(r1407, r1406A).
externallyConnected(r1407, r1406B).
externallyConnected(r1407, r1406C).
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
externallyConnected(mainHallA, studentArea).
externallyConnected(mainHallA, mainHallB).
externallyConnected(mainHallA, workshop).
externallyConnected(mainHallA, offices).
disconnected(mainHallA, utility).
disconnected(mainHallA, organization).

equal(r1401A, mainHallA).

%%%% mainHallB only consists of 1401B
externallyConnected(mainHallB, mainHallA).
externallyConnected(mainHallB, offices).
externallyConnected(mainHallB, utility).
externallyConnected(mainHallB, organization).
disconnected(mainHallB, studentArea).
disconnected(mainHallB, workshop).

equal(r1401B, mainHallB).

%%%% utility
externallyConnected(utility, mainHallB).
externallyConnected(utility, organization).
externallyConnected(utility, offices).
disconnected(utility, workshop).
disconnected(utility, studentArea).
disconnected(utility, mainHallA).

%%%% the utility area consists of the rooms 1405 1405A 1405B and 1403C
tangentialProperPart(r1405, utility).
tangentialProperPart(r1405A, utility).
tangentialProperPart(r1405B, utility).
tangentialProperPart(r1403C, utility).

%%%% the rooms which are part of the utility area are in relation to each other
externallyConnected(r1405, r1405A).
externallyConnected(r1405A, r1405B).
externallyConnected(r1405A, r1403C).
disconnected(r1410A, r1410B).
disconnected(r1410A, r1410B).
disconnected(r1410A, r1410B).

%%%% organization
externallyConnected(organization, mainHallB).
externallyConnected(organization, utility).
disconnected(organization, mainHallA).
disconnected(organization, workshop).
disconnected(organization, offices).
disconnected(organization, studentArea).

%%%% the organization area consists of the rooms 1403 1403A and 1403B
tangentialProperPart(r1403, organization).
tangentialProperPart(r1403A, organization).
tangentialProperPart(r1403B, organization).

%%%% the rooms which are part of the organization area are in relation to each other
externallyConnected(r1403A, r1403B).
externallyConnected(r1403B, r1403).
disconnected(r1403A, r1403).

%%%% relations between rooms and POIs

%%% 1411
nonTangentialProperPart(studentLab1, r1411).
nonTangentialProperPart(kicker, r1411).
nonTangentialProperPart(studentLab2, r1411).
nonTangentialProperPart(studentLabDoor1, r1411).

%%%% 1411C
nonTangentialProperPart(floor1, r1411C).
nonTangentialProperPart(studentLabDoor2, r1411C).
nonTangentialProperPart(hiWiDoor1, r1411C).

%%%% 1411A
nonTangentialProperPart(hiwiroom, r1411A).
nonTangentialProperPart(hiWiDoor2, r1411A).

%%%% 1401A
nonTangentialProperPart(floor1Door1, r1401A).
nonTangentialProperPart(floor1Door2, r1401A).
nonTangentialProperPart(floor1Door3, r1401A).
nonTangentialProperPart(floor1Door4, r1401A).

%%%% 1401B
nonTangentialProperPart(floor3Door1, r1401B).
nonTangentialProperPart(floor3Door2, r1401B).
nonTangentialProperPart(floor3Door3, r1401B).
nonTangentialProperPart(floor3Door4, r1401B).

%%%% 1410
nonTangentialProperPart(workShopDoor1, r1410).
nonTangentialProperPart(workShopDoor2, r1410).

%%%% 1407
nonTangentialProperPart(floor2Door1, r1407).
nonTangentialProperPart(floor2Door2, r1407).
nonTangentialProperPart(floor2Door3, r1407).
nonTangentialProperPart(floor2Door4, r1407).
nonTangentialProperPart(floor2Door5, r1407).
nonTangentialProperPart(floor2Door6, r1407).
nonTangentialProperPart(floor2Door7, r1407).
nonTangentialProperPart(floor2Door8, r1407).
nonTangentialProperPart(floor2Door9, r1407).

%%%% 1410A
nonTangentialProperPart(workShopDoor3, r1410A).
nonTangentialProperPart(workShopright, r1410A).

%%%% 1410B
nonTangentialProperPart(workShopDoor4, r1410B).
nonTangentialProperPart(workShopLeft, r1410B).

%%%% 1408A
nonTangentialProperPart(room1408A, r1408A).

%%%% 1408B
nonTangentialProperPart(room1408BFront, r1408B).
nonTangentialProperPart(room1408BWindow, r1408B).

%%%% 1408C
nonTangentialProperPart(room1408CFront, r1408C).
nonTangentialProperPart(room1408CWindow, r1408C).

%%%% 1406A
nonTangentialProperPart(room1406AFront, r1406A).
nonTangentialProperPart(room1406AWindow, r1406A).

%%%% 1406B
nonTangentialProperPart(room1406BFront, r1406B).
nonTangentialProperPart(room1406BWindow, r1406B).

%%%% 1406C
nonTangentialProperPart(room1406C, r1406C).

%%%% 1405B
nonTangentialProperPart(kitchen, r1405B).

%%%% 1405A
nonTangentialProperPart(floor4, r1405A).
nonTangentialProperPart(floor4Door1, r1405A).
nonTangentialProperPart(floor4Door2, r1405A).

%%%% 1405
nonTangentialProperPart(meetingroom, r1405).

%%%% 1403C
nonTangentialProperPart(copyroom, r1403C).
nonTangentialProperPart(copyroomDoor1, r1403C).
nonTangentialProperPart(copyroomDoor2, r1403C).

%%%% 1403A
nonTangentialProperPart(secreteriatDoor1, r1403A).
nonTangentialProperPart(secreteriatDoor2, r1403A).
nonTangentialProperPart(secreteriatDoor3, r1403A).
nonTangentialProperPart(secreteriat, r1403A).

%%%% 1403
nonTangentialProperPart(room1403Door, r1403).
nonTangentialProperPart(room1403, r1403).