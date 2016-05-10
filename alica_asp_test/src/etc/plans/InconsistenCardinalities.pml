<?xml version="1.0" encoding="ASCII"?>
<alica:Plan xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:alica="http:///de.uni_kassel.vs.cn" id="1462879046488" name="InconsistenCardinalities" comment="" masterPlan="true" utilityFunction="" utilityThreshold="0.1" destinationPath="Plans" priority="0.0" minCardinality="0" maxCardinality="2">
  <states id="1462879046489" name="PlanState" comment="" entryPoint="1462879046490">
    <plans xsi:type="alica:Plan">BrokenCardPlan.pml#1462879159773</plans>
    <plans xsi:type="alica:Plan">CorrectCardPlan.pml#1462879199379</plans>
  </states>
  <entryPoints id="1462879046490" name="MISSING_NAME" comment="" successRequired="false" minCardinality="0" maxCardinality="2">
    <task>../Misc/taskrepository.tsk#1225112227903</task>
    <state>#1462879046489</state>
  </entryPoints>
</alica:Plan>
