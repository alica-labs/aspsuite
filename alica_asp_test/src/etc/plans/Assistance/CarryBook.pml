<?xml version="1.0" encoding="ASCII"?>
<alica:Plan xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:alica="http:///de.uni_kassel.vs.cn" id="1462889426092" name="CarryBook" comment="" masterPlan="false" utilityFunction="" utilityThreshold="0.1" destinationPath="Plans/Assistance" priority="0.0" minCardinality="0" maxCardinality="2147483647">
  <conditions xsi:type="alica:PreCondition" id="1463404630976" name="NewPreCondition" comment="" conditionString="agent(A), in(A,p1462889426092, tsk1225112227903, S),  carries(A, harryPotter1)." pluginName="DefaultPlugin" enabled="true"/>
  <vars id="1468416183422" name="BookVarPlan" comment="" Type=""/>
  <states id="1462889438750" name="CarryBook" comment="" entryPoint="1462889438751">
    <parametrisation id="1468416185218" name="MISSING_NAME" comment="">
      <subplan xsi:type="alica:BehaviourConfiguration">Carry.beh#1463077955666</subplan>
      <subvar>Carry.beh#1468416170652</subvar>
      <var>#1468416183422</var>
    </parametrisation>
    <plans xsi:type="alica:BehaviourConfiguration">Carry.beh#1463077955666</plans>
    <outTransitions>#1463404653224</outTransitions>
  </states>
  <states id="1463404627637" name="DeliverBook" comment="">
    <plans xsi:type="alica:BehaviourConfiguration">HandOver.beh#1463404677134</plans>
    <inTransitions>#1463404653224</inTransitions>
  </states>
  <transitions id="1463404653224" name="MISSING_NAME" comment="" msg="">
    <preCondition id="1463404654846" name="MISSING_NAME" comment="" conditionString="" pluginName="DefaultPlugin" enabled="true"/>
    <inState>#1462889438750</inState>
    <outState>#1463404627637</outState>
  </transitions>
  <entryPoints id="1462889438751" name="MISSING_NAME" comment="" successRequired="false" minCardinality="0" maxCardinality="2147483647">
    <task>../../Misc/taskrepository.tsk#1225112227903</task>
    <state>#1462889438750</state>
  </entryPoints>
</alica:Plan>
