<?xml version="1.0" encoding="ASCII"?>
<alica:Plan xmi:version="2.0" xmlns:xmi="http://www.omg.org/XMI" xmlns:alica="http:///de.uni_kassel.vs.cn" id="1453033506291" name="Min3AgentPlan" comment="" masterPlan="false" utilityFunction="" utilityThreshold="0.1" destinationPath="Plans" priority="0.0" minCardinality="3" maxCardinality="2147483647">
  <states id="1453033506292" name="Min2AgentState" comment="" entryPoint="1453033506293"/>
  <states id="1456483282661" name="Min1AgentState" comment="" entryPoint="1456483257411"/>
  <entryPoints id="1453033506293" name="Attack" comment="" successRequired="false" minCardinality="2" maxCardinality="2147483647">
    <task>../Misc/taskrepository.tsk#1222613952469</task>
    <state>#1453033506292</state>
  </entryPoints>
  <entryPoints id="1456483257411" name="Defend" comment="" successRequired="false" minCardinality="1" maxCardinality="2147483647">
    <task>../Misc/taskrepository.tsk#1225115406909</task>
    <state>#1456483282661</state>
  </entryPoints>
</alica:Plan>
