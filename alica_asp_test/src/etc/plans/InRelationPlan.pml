<?xml version="1.0" encoding="ASCII"?>
<alica:Plan xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:alica="http:///de.uni_kassel.vs.cn" id="1456731505906" name="InRelationPlan" comment="" masterPlan="false" utilityFunction="" utilityThreshold="0.1" destinationPath="Plans" priority="0.0" minCardinality="0" maxCardinality="2147483647">
  <conditions xsi:type="alica:PreCondition" id="1456731822708" name="InRelationPreCondition" comment="" conditionString="NumAgents = #count{A : agent(A),  in(A,p1456731591075, tsk1225112227903, S)}, NumAgents > 1." pluginName="DefaultPlugin" enabled="true"/>
  <states id="1456731537186" name="NewState" comment="" entryPoint="1456731537187"/>
  <entryPoints id="1456731537187" name="MISSING_NAME" comment="" successRequired="false" minCardinality="0" maxCardinality="2147483647">
    <task>../Misc/taskrepository.tsk#1225112227903</task>
    <state>#1456731537186</state>
  </entryPoints>
</alica:Plan>
