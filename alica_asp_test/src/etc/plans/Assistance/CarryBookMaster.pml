<?xml version="1.0" encoding="ASCII"?>
<alica:Plan xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:alica="http:///de.uni_kassel.vs.cn" id="1462889118325" name="CarryBookMaster" comment="" masterPlan="true" utilityFunction="" utilityThreshold="0.1" destinationPath="Plans/Assistance" priority="0.0" minCardinality="2" maxCardinality="2147483647">
  <states id="1462889118326" name="CarryBookState1" comment="" entryPoint="1462889118327">
    <plans xsi:type="alica:Plan">CarryBook.pml#1462889426092</plans>
  </states>
  <states id="1462889170425" name="CarryBookState2" comment="" entryPoint="1462889160670">
    <plans xsi:type="alica:Plan">CarryBook.pml#1462889426092</plans>
  </states>
  <entryPoints id="1462889118327" name="Attack" comment="" successRequired="false" minCardinality="1" maxCardinality="1">
    <task>../../Misc/taskrepository.tsk#1222613952469</task>
    <state>#1462889118326</state>
  </entryPoints>
  <entryPoints id="1462889160670" name="Defend" comment="" successRequired="false" minCardinality="1" maxCardinality="2147483647">
    <task>../../Misc/taskrepository.tsk#1225115406909</task>
    <state>#1462889170425</state>
  </entryPoints>
</alica:Plan>
