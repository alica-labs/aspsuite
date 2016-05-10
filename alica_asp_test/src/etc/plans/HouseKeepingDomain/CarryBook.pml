<?xml version="1.0" encoding="ASCII"?>
<alica:Plan xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:alica="http:///de.uni_kassel.vs.cn" id="1462889426092" name="CarryBook" comment="" masterPlan="false" utilityFunction="" utilityThreshold="0.1" destinationPath="" priority="0.0" minCardinality="0" maxCardinality="2147483647">
  <conditions xsi:type="alica:PreCondition" id="1462889683880" name="NewPreCondition" comment="" conditionString="agent(A),  in(A,p1462889426092, T, S),  carries(A, b), book(b)" pluginName="DefaultPlugin" enabled="true"/>
  <states id="1462889438750" name="NewState" comment="" entryPoint="1462889438751"/>
  <entryPoints id="1462889438751" name="MISSING_NAME" comment="" successRequired="false" minCardinality="0" maxCardinality="2147483647">
    <task>../../Misc/taskrepository.tsk#1225112227903</task>
    <state>#1462889438750</state>
  </entryPoints>
</alica:Plan>
