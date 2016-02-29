<?xml version="1.0" encoding="ASCII"?>
<alica:Plan xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:alica="http:///de.uni_kassel.vs.cn" id="1456730912737" name="NonLocalInRelation" comment="" masterPlan="true" utilityFunction="" utilityThreshold="0.1" destinationPath="Plans" priority="0.0" minCardinality="0" maxCardinality="2147483647">
  <states id="1456730912738" name="NonLocalInState" comment="" entryPoint="1456730912739">
    <plans xsi:type="alica:Plan">InRelationPlan.pml#1456731505906</plans>
  </states>
  <states id="1456731550573" name="ReferencedPlanState" comment="" entryPoint="1456731546744">
    <plans xsi:type="alica:Plan">ReferencedPlan.pml#1456731591075</plans>
  </states>
  <entryPoints id="1456730912739" name="Attack" comment="" successRequired="false" minCardinality="0" maxCardinality="2147483647">
    <task>../Misc/taskrepository.tsk#1222613952469</task>
    <state>#1456730912738</state>
  </entryPoints>
  <entryPoints id="1456731546744" name="Defend" comment="" successRequired="false" minCardinality="0" maxCardinality="2147483647">
    <task>../Misc/taskrepository.tsk#1225115406909</task>
    <state>#1456731550573</state>
  </entryPoints>
</alica:Plan>
