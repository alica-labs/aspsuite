<?xml version="1.0" encoding="ASCII"?>
<alica:Plan xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:alica="http:///de.uni_kassel.vs.cn" id="1462886292442" name="InconsistentCardinalities" comment="" masterPlan="true" utilityFunction="" utilityThreshold="0.1" destinationPath="Plans" priority="0.0" minCardinality="0" maxCardinality="5">
  <states id="1462886292443" name="StateWithBrokenCard" comment="" entryPoint="1462886292444">
    <plans xsi:type="alica:Plan">BrokenCardPlan.pml#1462879159773</plans>
  </states>
  <states id="1462886315314" name="StateWithRightCard" comment="" entryPoint="1462886310757">
    <plans xsi:type="alica:Plan">BrokenCardPlan.pml#1462879159773</plans>
  </states>
  <entryPoints id="1462886292444" name="Attack" comment="" successRequired="false" minCardinality="0" maxCardinality="2">
    <task>../Misc/taskrepository.tsk#1222613952469</task>
    <state>#1462886292443</state>
  </entryPoints>
  <entryPoints id="1462886310757" name="Defend" comment="" successRequired="false" minCardinality="0" maxCardinality="3">
    <task>../Misc/taskrepository.tsk#1225115406909</task>
    <state>#1462886315314</state>
  </entryPoints>
</alica:Plan>
