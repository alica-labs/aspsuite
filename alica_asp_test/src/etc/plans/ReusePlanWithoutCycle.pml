<?xml version="1.0" encoding="ASCII"?>
<alica:Plan xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:alica="http:///de.uni_kassel.vs.cn" id="1455093185652" name="ReusePlanWithoutCycle" comment="" masterPlan="true" utilityFunction="" utilityThreshold="0.1" destinationPath="Plans" priority="0.0" minCardinality="0" maxCardinality="2147483647">
  <states id="1455093185653" name="1stTime" comment="" entryPoint="1455093185654">
    <plans xsi:type="alica:Plan">Min3AgentPlan.pml#1453033506291</plans>
  </states>
  <states id="1455093214694" name="2ndTime" comment="" entryPoint="1455093221781">
    <plans xsi:type="alica:Plan">Min3AgentPlan.pml#1453033506291</plans>
  </states>
  <entryPoints id="1455093185654" name="Attack" comment="" successRequired="false" minCardinality="0" maxCardinality="2147483647">
    <task>../Misc/taskrepository.tsk#1222613952469</task>
    <state>#1455093185653</state>
  </entryPoints>
  <entryPoints id="1455093221781" name="Defend" comment="" successRequired="false" minCardinality="0" maxCardinality="2147483647">
    <task>../Misc/taskrepository.tsk#1225115406909</task>
    <state>#1455093214694</state>
  </entryPoints>
</alica:Plan>
