<?xml version="1.0" encoding="ASCII"?>
<alica:Plan xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:alica="http:///de.uni_kassel.vs.cn" id="1452783558494" name="UnconnectedStateMachine" comment="" masterPlan="true" utilityFunction="" utilityThreshold="0.1" destinationPath="" priority="0.0" minCardinality="0" maxCardinality="2147483647">
  <states id="1452783558495" name="State1" comment="">
    <inTransitions>#1452783597993</inTransitions>
    <outTransitions>#1452783592558</outTransitions>
    <outTransitions>#1452783601103</outTransitions>
  </states>
  <states id="1452783576711" name="State2" comment="">
    <inTransitions>#1452783592558</inTransitions>
    <inTransitions>#1452783594385</inTransitions>
    <outTransitions>#1452783596070</outTransitions>
  </states>
  <states id="1452783579086" name="State3" comment="">
    <inTransitions>#1452783596070</inTransitions>
    <outTransitions>#1452783597993</outTransitions>
    <outTransitions>#1452783604345</outTransitions>
  </states>
  <states id="1452783583119" name="State4" comment="">
    <inTransitions>#1452783601103</inTransitions>
    <inTransitions>#1452783604345</inTransitions>
    <outTransitions>#1452783594385</outTransitions>
  </states>
  <transitions id="1452783592558" name="MISSING_NAME" comment="" msg="">
    <preCondition id="1452783594228" name="MISSING_NAME" comment="" conditionString="" pluginName="DefaultPlugin" enabled="true"/>
    <inState>#1452783558495</inState>
    <outState>#1452783576711</outState>
  </transitions>
  <transitions id="1452783594385" name="MISSING_NAME" comment="" msg="">
    <preCondition id="1452783595930" name="MISSING_NAME" comment="" conditionString="" pluginName="DefaultPlugin" enabled="true"/>
    <inState>#1452783583119</inState>
    <outState>#1452783576711</outState>
  </transitions>
  <transitions id="1452783596070" name="MISSING_NAME" comment="" msg="">
    <preCondition id="1452783597877" name="MISSING_NAME" comment="" conditionString="" pluginName="DefaultPlugin" enabled="true"/>
    <inState>#1452783576711</inState>
    <outState>#1452783579086</outState>
  </transitions>
  <transitions id="1452783597993" name="MISSING_NAME" comment="" msg="">
    <preCondition id="1452783600822" name="MISSING_NAME" comment="" conditionString="" pluginName="DefaultPlugin" enabled="true"/>
    <inState>#1452783579086</inState>
    <outState>#1452783558495</outState>
  </transitions>
  <transitions id="1452783601103" name="MISSING_NAME" comment="" msg="">
    <preCondition id="1452783604156" name="MISSING_NAME" comment="" conditionString="" pluginName="DefaultPlugin" enabled="true"/>
    <inState>#1452783558495</inState>
    <outState>#1452783583119</outState>
  </transitions>
  <transitions id="1452783604345" name="MISSING_NAME" comment="" msg="">
    <preCondition id="1452783605843" name="MISSING_NAME" comment="" conditionString="" pluginName="DefaultPlugin" enabled="true"/>
    <inState>#1452783579086</inState>
    <outState>#1452783583119</outState>
  </transitions>
  <entryPoints id="1452783558496" name="MISSING_NAME" comment="" successRequired="false" minCardinality="0" maxCardinality="2147483647">
    <task>../Misc/taskrepository.tsk#1225112227903</task>
  </entryPoints>
</alica:Plan>
