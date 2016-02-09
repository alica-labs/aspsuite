<?xml version="1.0" encoding="ASCII"?>
<alica:Plan xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:alica="http:///de.uni_kassel.vs.cn" id="1455035766972" name="UnconnectedSynchronisations" comment="" masterPlan="true" utilityFunction="" utilityThreshold="0.1" destinationPath="Plans" priority="0.0" minCardinality="0" maxCardinality="2147483647">
  <states id="1455035766973" name="StateABefore" comment="" entryPoint="1455035766974">
    <outTransitions>#1455035816681</outTransitions>
  </states>
  <states id="1455035790843" name="StateAAfter" comment="">
    <inTransitions>#1455035816681</inTransitions>
  </states>
  <states id="1455035838383" name="StateDBefore" comment="" entryPoint="1455035846736">
    <outTransitions>#1455035906949</outTransitions>
  </states>
  <states id="1455035874846" name="StateDAfter" comment="">
    <inTransitions>#1455035906949</inTransitions>
  </states>
  <transitions id="1455035816681" name="MISSING_NAME" comment="" msg="">
    <preCondition id="1455035817820" name="MISSING_NAME" comment="" conditionString="" pluginName="DefaultPlugin" enabled="true"/>
    <inState>#1455035766973</inState>
    <outState>#1455035790843</outState>
    <synchronisation>#1455035803877</synchronisation>
  </transitions>
  <transitions id="1455035906949" name="MISSING_NAME" comment="" msg="">
    <preCondition id="1455035907819" name="MISSING_NAME" comment="" conditionString="" pluginName="DefaultPlugin" enabled="true"/>
    <inState>#1455035838383</inState>
    <outState>#1455035874846</outState>
  </transitions>
  <synchronisations id="1455035803877" name="SingleConnectedSynchronisation" comment="" synchedTransitions="1455035816681" talkTimeout="30" syncTimeout="10000" failOnSyncTimeOut="false"/>
  <synchronisations id="1455037378756" name="UnconnectedSynchronisation" comment="" talkTimeout="30" syncTimeout="10000" failOnSyncTimeOut="false"/>
  <entryPoints id="1455035766974" name="Attack" comment="" successRequired="false" minCardinality="0" maxCardinality="2147483647">
    <task>../Misc/taskrepository.tsk#1222613952469</task>
    <state>#1455035766973</state>
  </entryPoints>
  <entryPoints id="1455035846736" name="Defend" comment="" successRequired="false" minCardinality="0" maxCardinality="2147483647">
    <task>../Misc/taskrepository.tsk#1225115406909</task>
    <state>#1455035838383</state>
  </entryPoints>
</alica:Plan>
