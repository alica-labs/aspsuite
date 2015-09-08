<?xml version="1.0" encoding="ASCII"?>
<alica:Plan xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:alica="http:///de.uni_kassel.vs.cn" id="1441701747919" name="MasterPlan" comment="" masterPlan="false" utilityFunction="" utilityThreshold="0.1" destinationPath="Plans" priority="0.0" minCardinality="0" maxCardinality="2147483647">
  <states id="1441701747920" name="RootState" comment="" entryPoint="1441701747921">
    <plans xsi:type="alica:Plan">UnconnectedState.pml#1441701808444</plans>
    <outTransitions>#1441701794000</outTransitions>
  </states>
  <states id="1441701781326" name="SecondRootState" comment="">
    <inTransitions>#1441701794000</inTransitions>
  </states>
  <transitions id="1441701794000" name="MISSING_NAME" comment="" msg="">
    <preCondition id="1441701794882" name="MISSING_NAME" comment="" conditionString="" pluginName="DefaultPlugin" enabled="true"/>
    <inState>#1441701747920</inState>
    <outState>#1441701781326</outState>
  </transitions>
  <entryPoints id="1441701747921" name="MISSING_NAME" comment="" successRequired="false" minCardinality="0" maxCardinality="2147483647">
    <task>../Misc/taskrepository.tsk#1225112227903</task>
    <state>#1441701747920</state>
  </entryPoints>
</alica:Plan>
