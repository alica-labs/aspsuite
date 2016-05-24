<?xml version="1.0" encoding="ASCII"?>
<alica:Plan xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:alica="http:///de.uni_kassel.vs.cn" id="1464091819666" name="ThirdInnerPlan" comment="" masterPlan="false" utilityFunction="" utilityThreshold="0.1" destinationPath="Plans/Assistance" priority="0.0" minCardinality="0" maxCardinality="2147483647">
  <conditions xsi:type="alica:RuntimeCondition" id="1464096415101" name="NewRuntimeCondition" comment="" conditionString="agent(A), in(A,p1464096415101, tsk1225112227903, S)." pluginName="DefaultPlugin"/>
  <states id="1464091833199" name="NewState" comment="" entryPoint="1464091833200"/>
  <entryPoints id="1464091833200" name="MISSING_NAME" comment="" successRequired="false" minCardinality="0" maxCardinality="2147483647">
    <task>../../Misc/taskrepository.tsk#1225112227903</task>
    <state>#1464091833199</state>
  </entryPoints>
</alica:Plan>
