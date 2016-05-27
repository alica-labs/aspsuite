<?xml version="1.0" encoding="ASCII"?>
<alica:Plan xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:alica="http:///de.uni_kassel.vs.cn" id="1464355916688" name="ReusePlanFromPlantypeWithoutCycle" comment="" masterPlan="true" utilityFunction="" utilityThreshold="0.1" destinationPath="Plans" priority="0.0" minCardinality="0" maxCardinality="2147483647">
  <states id="1464355916689" name="PlantypeState" comment="" entryPoint="1464355916690">
    <plans xsi:type="alica:Plan">Min3AgentPlan.pml#1453033506291</plans>
    <plans xsi:type="alica:PlanType">PlantypeWithoutCycle.pty#1464355946605</plans>
  </states>
  <entryPoints id="1464355916690" name="MISSING_NAME" comment="" successRequired="false" minCardinality="0" maxCardinality="2147483647">
    <task>../Misc/taskrepository.tsk#1225112227903</task>
    <state>#1464355916689</state>
  </entryPoints>
</alica:Plan>
