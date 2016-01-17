<?xml version="1.0" encoding="ASCII"?>
<alica:Plan xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:alica="http:///de.uni_kassel.vs.cn" id="1453033407259" name="HierarchicalInconsistentCardinalities" comment="" masterPlan="true" utilityFunction="" utilityThreshold="0.1" destinationPath="" priority="0.0" minCardinality="0" maxCardinality="2">
  <states id="1453033407260" name="Max2AgentState" comment="" entryPoint="1453033407261">
    <plans xsi:type="alica:Plan">Min3AgentPlan.pml#1453033506291</plans>
  </states>
  <entryPoints id="1453033407261" name="MISSING_NAME" comment="" successRequired="false" minCardinality="0" maxCardinality="2">
    <task>../Misc/taskrepository.tsk#1225112227903</task>
    <state>#1453033407260</state>
  </entryPoints>
</alica:Plan>
