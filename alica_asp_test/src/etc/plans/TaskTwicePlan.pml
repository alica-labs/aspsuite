<?xml version="1.0" encoding="ASCII"?>
<alica:Plan xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:alica="http:///de.uni_kassel.vs.cn" id="1453033761283" name="TaskTwicePlan" comment="" masterPlan="true" utilityFunction="" utilityThreshold="0.1" destinationPath="" priority="0.0" minCardinality="0" maxCardinality="2147483647">
  <states id="1453033761284" name="Task1State" comment="" entryPoint="1453033761285"/>
  <states id="1453033775911" name="Task2State" comment="" entryPoint="1453033782170"/>
  <entryPoints id="1453033761285" name="MISSING_NAME" comment="" successRequired="false" minCardinality="0" maxCardinality="2147483647">
    <task>../Misc/taskrepository.tsk#1225112227903</task>
    <state>#1453033761284</state>
  </entryPoints>
  <entryPoints id="1453033782170" name="NewEntryPoint" comment="" successRequired="false" minCardinality="0" maxCardinality="2147483647">
    <task>../Misc/taskrepository.tsk#1225112227903</task>
    <state>#1453033775911</state>
  </entryPoints>
</alica:Plan>
