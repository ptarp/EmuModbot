
#Event MobProb "#*#Your target is too far away#*#"
#Event MobProb "#*#You cannot see your target#*#"
#Event MobProb "#*#You can't hit them from here#*#"
#Event PetTargetMez "|${Me.Pet.CleanName}| tells you, 'I am unable to wake#*#"
#event EnrageStart "#*# has become ENRAGED."
#event EnrageEnd "#*# is no longer enraged"

Sub Event_MobProb
	/if (!${ACMobProbT} && ${Melee.Combat} && ${Me.Standing} && ${Target.ID}==${ACMATarget} && ${Me.Combat} && (!${Me.Casting.ID} || ${Me.Class.ShortName.Equal[brd]})) {
      /if (${DoRanged}) {
        /face fast ${Target}
        /varset ACMobProbT 2s
        /return
      }
      /if (${Target.Distance3D}<=${ACLeash} && ${Target.Distance3D}>${Target.MaxRangeTo} && !${Stick.Status.NotEqual[on]}) /squelch /stick ${Math.Calc[${Target.MaxRangeTo}-4]}
      /if (!${Melee.Combat} && ${Plugin[mq2melee].Name.Length} && !${DoRanged}) /killthis
      /if (${Melee.Combat} && ${Stick.Status.NotEqual[on]} && ${Target.Distance}<=25) /squelch /stick 10 id ${Target.ID}
      /if (${Melee.Combat} && ${Stick.Status.Equal[on]} && ${Target.Distance}<=25) {
      	/if (${Me.Levitating}) /face ${Target}
		/stick hold ${If[${Target.Height}<5,9,${Math.Calc[${Target.Height}+3].Int}]} ${If[!${Me.GroupSize} || ${Melee.AggroMode},moveback,${If[${Melee.BackStabbing},behind,!front]}]} ${If[${Me.Underwater},uw,]} 
      	| /squelch /stick -2
      }
      /varset ACMobProbT 2s
      |/echo MobProb
    }
/return

Sub Event_PetTargetMez
  /if (${Me.Pet.ID} && ${PetAssist}) /varset PetAssist 1
/return


Sub LoadACINIVals
  /declare ACStartX int outer
  /declare ACStartY int outer
  /declare ACStartHeading int outer
  /declare ACMATarget int outer
  /declare ACState int outer
  /declare ACBeginHP int outer
  /declare ACEndHP int outer
  /declare ACTarID int outer
  /declare ACMobProbT timer outer 5s
  /declare ReTargetT timer outer 5s
  /call LoadINIVar Melee OffTank FALSE OffTank "${IniFile}" bool
  /call LoadINIVar Melee ACLeash 50 ACLeash "${IniFile}" int
  /call LoadINIVar Melee ACAssistPct 95 ACAssistPct "${IniFile}" int
  /call LoadINIVar Melee ACManaPct 20 ACManaPct "${IniFile}" int
  /call LoadINIVar Melee ACAnnounce " " ACAnnounce "${IniFile}" string
  /call LoadINIVar Melee ACMeleeCmd "/melee plugin=1" ACMeleeCmd "${IniFile}" string
  /call LoadINIVar Melee ACBefore " " ACBefore "${IniFile}" string
  /call LoadINIVar Melee ACAfter " " ACAfter "${IniFile}" string
  /call LoadINIVar Melee Puller " " Puller "${IniFile}" string  
  /call LoadINIVar Melee TankList " " TankList "${IniFile}" string 
  /call LoadINIVar Melee DoRanged FALSE DoRanged "${IniFile}" bool
  
  /if (${ACMeleeCmd.Length}>10) /docommand ${ACMeleeCmd}
  /assist off
/return

Sub ACGetAssist
  /declare tTargetID int local 0
  /if (!${Defined[Param0]} || !${Spawn[pc ${Param0}].ID} && !${Spawn[mercenary ${Param0}].ID}) /return
|  /echo ACGetAssist - Assisting ${Param0}
  /if (${Spawn[pc ${Param0}].Distance}>=200) /return
  /assist ${Param0}
  /delay 1s ${Target.ID}!=${Spawn[${Param0}].ID}
  /docommand ${If[${Target.ID}!=${Spawn[${Param0}].ID} && ${Target.Type.NotEqual[PC]} && ${Target.Standing},/varset tTargetID ${Target.ID},/varset tTargetID 0]}
/return ${tTargetID}

Sub AdvCombat
/if (${MBPause}) /return
/if (${MBDebug}) /multiline ; /echo Macro.RunTime:${Macro.RunTime} - Sub:AdvCombat
/if (!${DoMelee} && !${DoRanged} && ${TankList.Find[${Me.Name}]}) {
  |TODO: This may need to be expanded for taunting in raids
  /if (!${Melee.Combat} && ${TankList.Find[${Me.Name}]} && ${ACMATarget} && ${Zone.ShortName.Equal[ikkinz]} && (${Target.ID}==${ACMATarget} || ${Target.ID}==${ACTarID})) {
    /if (${Me.CombatAbilityReady[Incite]}) /disc Incite
    /if (${Me.AbilityReady[Taunt]}) /doability taunt    
  }
  /return
}
/if (${DoRanged} && ${Spawn[id ${ACMATarget}].ID}) {
  /if (${Spawn[id ${ACMATarget}].ID} && ${Spawn[id ${ACMATarget}].Type.NotEqual[corpse]}) {
    /if (${Target.ID}!=${ACMATarget}) /target id ${ACMATarget}
    /delay 5 ${Target.ID}==${ACMATarget}
    /face fast ${Target}
    /if (!${Stick.Status.Equal[on]}) /stick hold {Math.Calc[40+{Target.Height}]} moveback
    /if (!${Me.AutoFire}) /autofire
    /return
  }
}
/if (${DoMelee} && !${Melee.Enable} && (${MakeCampY} && ${MakeCampX} && ${Math.Distance[${Me.Y},${Me.X}:${MakeCampY},${MakeCampX}]}<40 || ${Ini["${MBWayPnt}",${Section},WP1].Length}>2 && ${Math.Distance[${Me.Y},${Me.X}:${Ini["${MBWayPnt}",${Section},WP1].Arg[1]},${Ini["${MBWayPnt}",${Section},WP1].Arg[2]}]}<40)) /squelch /melee plugin=1
/if (${TankName.Equal[${Me.Name}]} && ${Target.Type.Equal[npc]} && ${ACMATarget} && ${Spawn[${ACMATarget}].ID} && !${Spawn[${ACMATarget}].Type.Equal[corpse]}) {
	/if (${ReTargetT} || ${Me.Combat}) /return
  /varset ReTargetT 10s
}
/if (${TankName.Equal[${Me.Name}]} && ${Target.Type.Equal[npc]} && (!${ACMATarget} || !${Spawn[${ACMATarget}].ID}) && ${APTargetID}) {
	/varset ACMATarget ${APTargetID}
	|/melee plugin=1
	|/killthis
	|/return
}
/if (${Me.State.Equal[feign]}) /return
| && ${Group.Puller.ID}==${Me.ID}
/declare a int local
/declare x int local
/declare sVarC string local
/declare sVarL string local
/declare sNBClnt string local
/for x 0 to ${NetBots.Count}
  /varset sNBClnt ${sNBClnt} ${NetBots.Client[${x}]}
/next x

/declare OTID int local ${Target.ID}

/call ADSpawnCheck
/for a 0 to ${Group.Members}
  /if ( !${NetBots[${Group.Member[${a}].Name}].ID} ) /varset sNBClnt ${sNBClnt} ${Group.Member[${a}].Name}
/next a
/if (${AHCount}) /call AHHealCheck
| /if (${OffTank} && ${Target.ID}==${NetBots.TankName[${Target.ID}]} && ${ADMobCount}>1) /goto :ChangeTarget
/if (${ACState}==100 || !${ACState} || ${OffTank} && !${ReTargetT} && ${ADMobCount}>1 || ${OffTank} && ${Param0.Length}) {
| /if (${OffTank} && ${ADMobCount}>1) /varset ACMATarget 0
 /if (${TankName.Equal[${Me.CleanName}]} && ${DoPull} && !${ACMATarget} && ${APTargetID}) /varset ACMATarget ${APTargetID}
| /if (!${ACMATarget} && ${TankName.Equal[${Me.CleanName}]} && ${Group.Puller.ID} && ${Group.Puller.ID}!=${Me.ID} && ${Spawn[npc id ${NetBots[${Group.Puller}].TargetID}].Distance}<${ACLeash} && !${ExcludeList.Find[|${Spawn[${NetBots[${Group.Puller}].TargetID}].CleanName}|]} && ${Spawn[${NetBots[${Group.Puller}].TargetID}].PctHPs}<=${ACAssistPct} && !${DoPull}) /multiline ; /varset ACMATarget ${NetBots[${Group.Puller}].TargetID};/goto :GotTarget
/if (${OffTank} && ${ADMobCount}>1 && ${TankName.NotEqual[${Me.Name}]}) {
  | Check if other toons have other mobs tanked..
  | Or already have a mob as tanking target, but mob is targetting another tank..  check other mobs if more than 2.
  /if ((${Target.ID} && ${Target.ID}==${NetBots[${TankName}].TargetID}) || (${Target.ID}!=${NetBots[${TankName}].TargetID} && ${ACTarID} && ${TankList.Find[${Me.TargetOfTarget.CleanName}]} && ${Me.TargetOfTarget.CleanName.NotEqual[${Me.Name}]} && ${ADMobCount}>2)) {
    /varset ACTarID 0
    /for a ${ADMobCount} downto 1
      /if (${ADMobList[${a}]} && ${ADMobList[${a}]}!=${NetBots[${TankName}].TargetID} && ${Spawn[${ADMobList[${a}]}].Type.NotEqual[corpse]} && ${Spawn[${ADMobList[${a}]}].Distance}<${ACLeash} && !${ExcludeList.Find[|${Spawn[${ACTarID}].CleanName}|]}) /varset ACTarID ${ADMobList[${a}]}
      |Check if other tanks are attacking this target.
      /for x 1 to ${Math.Calc[${TankList.Count[ ]}+1]}
        /if (${NetBots[${TankList.Arg[${x}]}].TargetID}==${ACTarID}) /varset ACTarID 0
      /next x
      /if (${ACTarID}) /goto :GotOTTarget
    /next a 
  }
:GotOTTarget
  /if (${ACTarID} && (${ACTarID}!=${Target.ID} || !${Target.ID})) { |If we get here, no other tank should be attacking this mob.
      /if (${ACMATarget}!=${ACTarID}) /bcaa Switching targets to offtank << ${Spawn[${ACTarID}].Name} >>
      /varset ACMATarget ${ACTarID}
      /target id ${ACTarID}
      /delay 1s ${Target.ID}==${ACTarID}
      /killthis
      /delay 1s ${Me.Combat}
  }
  | If I already have a offtank target, but it's targeting a netbots member that's not a tank.. Taunt it off.
  /if (${NetBots[${TankName}].TargetID} && ${ACMATarget} && ${ACTarID} && ${Spawn[${ACTarID}].Type.NotEqual[corpse]} && ${Target.ID}==${ACTarID} && ${NetBots[${TankName}].TargetID}!=${Target.ID} && ${Melee.Combat} && !${TankList.Find[${Me.TargetOfTarget.CleanName}]} && ${NetBots[${Me.TargetOfTarget.CleanName}].ID}) {
      /bc Tauntiong a ${Spawn[${ACTarID}].Name} off of ${Me.TargetOfTarget.CleanName}
      /if (${Me.CombatAbilityReady[Incite]}) /disc Incite
      /if (${Me.AbilityReady[Taunt]}) /doability taunt
      |/delay 1s ${Me.TargetOfTarget.CleanName.Equal[${Me.Name}]}
      |/if (${Me.TargetOfTarget.CleanName.Equal[${Me.Name}]}) /return
      /return
  }
  /if (${ACTarID} && ${Melee.Combat} && ${ACMATarget}) /return
}

/if (!${ACMATarget} && ${TankName.Equal[${Me.CleanName}]} && ${Puller.Length}>1 && ${Spawn[${Puller}].ID} && ${Spawn[${Puller}].ID}!=${Me.ID}) {
	/if ( ${NetBots[${Puller}].ID} && ${NetBots[${Puller}].TargetID} && ${Spawn[npc id ${NetBots[${Puller}].TargetID}].Distance}<${ACLeash} && !${ExcludeList.Find[|${Spawn[id ${NetBots[${Puller}].TargetID}].CleanName}|]} && ${Spawn[${NetBots[${Puller}].TargetID}].PctHPs}<=${ACAssistPct} && !${DoPull}) {
    /if (${Spawn[id ${NetBots[${Puller}].TargetID}].Type.Equal[PC]}) /return
    /echo /if (${Spawn[${NetBots[${Puller}].TargetID}].Type.Equal[PC]}) /return
    /echo excludelist ${Spawn[${NetBots[${Puller}].TargetID}].CleanName} - ${ExcludeList.Find[|${Spawn[${NetBots[${Puller}].TargetID}].CleanName}|]}
	
    /multiline ; /varset ACMATarget ${NetBots[${Puller}].TargetID};/goto :GotTarget

  }
  /if (${ExcludeList.Find[|${Spawn[id ${NetBots[${Puller}].TargetID}].CleanName}|]} > 0) {
    /squelch /target clear
    /return
  } 
	/if (${Target.ID} && ${Target.Type.Equal[Corpse]}) {
		/target clear
		/delay 1s !${Target.ID}
	}
	/if (${Spawn[pc ${Puller}].ID} && ${Spawn[pc ${Puller}].Distance}<${ACLeash} && !${NetBots[${Puller}].ID} ) /assist ${Puller}
	/delay 1s ${Target.ID}!=${Spawn[pc ${Puller}].ID}
	/if (${Target.ID} && ${Target.ID}!=${Spawn[pc ${Puller}].ID} && !${ExcludeList.Find[|${Target.CleanName}|]} ${Target.PctHPs}<=${ACAssistPct} && !${DoPull} ) /multiline ; /varset ACMATarget ${Target.ID};/goto :GotTarget
  /return
}

/if (!${OffTank} || ${ADMobCount}<2) {
  /if (${Spawn[pc ${TankName}].Distance}<200) {
   /if (${NetBots[${TankName}].ID}) {
    /if (!${ExcludeList.Find[|${Spawn[${NetBots[${TankName}].TargetID}].CleanName}|]} && ${Spawn[${NetBots[${TankName}].TargetID}].Standing}) /varset ACMATarget ${NetBots[${TankName}].TargetID}
   } else /if (!${NetBots[${TankName}].ID}) {
    /call ACGetAssist ${TankName}
    /if (!${ExcludeList.Find[|${Spawn[${Macro.Return}].CleanName}|]}) /varset ACMATarget ${Macro.Return}
   }
  }
  /if (${ACMATarget} && ${ACState}!=100 && ${If[${NetBots[${TankName}].ID},${NetBots[${TankName}].TargetHP},${Spawn[${ACMATarget}].PctHPs}]}>${ACAssistPct} || !${Spawn[${ACMATarget}].Standing}) /varset ACMATarget 0
} else /if (${OffTank}) {
|   	/varset ACTarID 0
    /if (${Defined[Param0]}) {
    	/if (${Spawn[npc radius 100 ${Param0}].ID}) /varset ACTarID ${Spawn[${Param0}].ID}
    	/if (!${ACTarID}) {
        /if (${sNBClnt.Find[${Param0}]} && ${Param0.NotEqual[${Me.CleanName}]}) {
          /varset ACTarID ${NetBots[${Param0}].TargetID}
        } else {
          /call ACGetAssist ${Param0}
          /varset ACTarID ${Macro.Return}
        }
      }
        /varset ReTargetT 5s
      	/docommand ${If[${Defined[Param1]},/varset ACBeginHP ${Param1},/varset ACBeginHP 100]}
      	/docommand ${If[${Defined[Param2]},/varset ACEndHP ${Param2},/varset ACEndHP 0]}
	  } else {
:ChangeTarget	    	
        /if (!${TankName.Equal[${Me.CleanName}]} && ${ADMobCount}) {
          /if (!${NetBots[${TankName}].TargetID} && ${Spawn[pc ${TankName}].Distance}<${ACLeash} && ${Group.Puller.Name.Equal[${Me.Name}]} && ${Target.ID}) /delay 4s ${NetBots[${TankName}].TargetID}
        /if (${NetBots[${TankName}].TargetID} && ${ACMATarget} && ${ACTarID} && ${Spawn[${ACTarID}].Type.NotEqual[corpse]} && ${Target.ID}==${ACTarID} && ${NetBots[${TankName}].TargetID}!=${Target.ID} && ${Melee.Combat} && (${ReTargetT} || ${Me.TargetOfTarget.CleanName.Equal[${Me.Name}]})) /return
          /if (${Target.ID}==${NetBots[${TankName}].TargetID} && ${ADMobCount}>1) /multiline ; /varset ACTarID 0;/varset ACMATarget 0
          /for a ${ADMobMax} downto 1
            /if (${ADMobList[${a}]} && ${ADMobList[${a}]}!=${NetBots[${TankName}].TargetID} && ${Spawn[${ADMobList[${a}]}].Type.NotEqual[corpse]} && ${Spawn[${ADMobList[${a}]}].Distance}<${ACLeash} && !${ExcludeList.Find[|${Spawn[${ACTarID}].CleanName}|]}) /varset ACTarID ${ADMobList[${a}]}
            /target id ${ACTarID}
            /delay 1s ${Target.ID}==${ACTarID}
            /if (${TankList.Find[${Me.TargetOfTarget.CleanName}]} && ${Me.TargetOfTarget.CleanName.NotEqual[${Me.Name}]} && ${ADMobCount}>2 && ${a} > 1) /multiline ; /varset ACTarID 0;/squelch /target clear
          /next a
					/if (${ACTarID}) {
            |/if (${ACTarID} && ${ACTarID}!=${Target.ID} && ${Spawn[${ACTarID}].Type.NotEqual[corpse]}) /multiline ; /squelch /target id ${ACTarID};/delay 1s ${Target.ID}==${ACTarID}
            /if (${ACTarID} && ${ACTarID}!=${Me.ID} && ${Spawn[${ACTarID}].Type.NotEqual[corpse]} && ${ACTarID}!=${NetBots[${TankName}].TargetID}) /bc Off-tanking a ${Spawn[id ${ACTarID}].CleanName}
            /docommand ${If[${Defined[Param1]},/varset ACBeginHP ${Param1},/varset ACBeginHP 100]}
       	    /docommand ${If[${Defined[Param2]},/varset ACEndHP ${Param2},/varset ACEndHP 0]}
             /varset ReTargetT 5s
     	  	}
          /if (!${ACTarID}) /varset ACTarID ${NetBots[${TankName}].TargetID}
     	    
      	} else /if (${TankName.Equal[${Me.CleanName}]} && ${ADMobCount} && !${NetBots[${Puller}].TargetID}) {
          /for a ${ADMobMax} downto 1
            /if (${ADMobList[${a}]} && ${ADMobList[${a}]}!=${NetBots[${TankName}].TargetID} && ${Spawn[${ADMobList[${a}]}].Type.NotEqual[corpse]} && ${Spawn[${ADMobList[${a}]}].Distance}<${ACLeash} && !${ExcludeList.Find[|${Spawn[${ACTarID}].CleanName}|]}) /varset ACTarID ${ADMobList[${a}]}
          /next a
        } 
     }
  /if ((!${ACTarID} || ${ACState}==100) && !${ExcludeList.Find[|${Spawn[${NetBots[${TankName}].TargetID}].CleanName}|]} && ${Spawn[${NetBots[${TankName}].TargetID}].Standing}) /multiline ; /varset ACTarID ${NetBots[${TankName}].TargetID};/varset ACMATarget ${NetBots[${TankName}].TargetID}
  /if (${ACTarID} && ${ACBeginHP}) {
   /if ((${Spawn[${ACTarID}].PctHPs}<${ACBeginHP} || ${Spawn[${ACTarID}].PctHPs}>=${ACEndHP}) && ${Spawn[${ACTarID}].Standing}) /varset ACMATarget ${ACTarID}
  } else {
   /varset ACMATarget 0
   /if (!${TankName.Equal[${Me.CleanName}]} && ${ACTarID} && ${Target.ID}==${ACTarID}) /multiline ; /varset ACTarID ${Target.ID};/varset ACMATarget ${Target.ID}
  }
  /if (${TankName.Equal[${Me.CleanName}]} && ${DoPull} && !${ACMATarget} && ${APTargetID}) /varset ACMATarget ${APTargetID}
 }
:GotTarget
/if (${ExcludList.Find[${Spawn[id ${ACMATarget}].CleanName}]}) {
  /varset ACMATarget 0
  /return
}
/varset ReTargetT 5s
  /if (${ACMATarget} && ${Select[${Spawn[${ACMATarget}].Type},npc,pet]} && !${Spawn[${ACMATarget}].Master.Type.Equal[pc]} && ${Spawn[${ACMATarget}].Distance3D}<=${ACLeash}) {
    /if (${ACState}==100 && ${Target.ID}!=${ACMATarget}) {
      /squelch /tar id ${ACMATarget}
      /delay 2s ${Target.ID}==${ACMATarget}
    }
|    /echo /if (!${OTID} && ${ACMATarget}) /varset OTID ${ACMATarget}
/if ((!${OTID} || !${Spawn[${OTID}].Type.Equal[npc]}) && ${ACMATarget}) /varset OTID ${ACMATarget}
    /if (${ACBefore.Length}>3 && ${OTID}==${ACMATarget}) /call IniCommand "${ACBefore}"

|    /if (${ACAnnounce.Length}) /docommand ${ACAnnounce} Meleeing: -[ ${Spawn[${ACMATarget}].CleanName} ]-
  	/if (${ACAnnounce.Length}>5 && ${OTID}==${ACMATarget}) {
  		/call ParseAnnounce "${ACAnnounce}" "${Spawn[${ACMATarget}].CleanName}" " "
  	} else /if (${ACAnnounce.Length}>1 && ${ACAnnounce.Length}<6) {
  		/docommand ${ACAnnounce} Meleeing: -[ ${Spawn[${ACMATarget}].CleanName} ]-
  	}
    /if (${FollowFlag}) {
      /multiline ; /varset ACState 2;/varset FollowFlag 1;/docommand ${If[${Defined[CurrLoc]},/call StopFunction,${If[${${FollowPlug}.Active} && ${AdvPlug},/call AdvPlugCall stop,/squelch /stick off]}]}
|      /multiline ; /varset ACState 2;/docommand ${If[${Defined[CurrLoc]},/call StopFunction,${If[${${FollowPlug}.Active} && ${AdvPlug},/call AdvPlugCall pause,/squelch /stick off]}]}
      } else {
      /multiline ; /docommand ${If[${ACState}==100 && (${Math.Distance[${Me.Y},${Me.X}:${ACStartY},${ACStartX}]}>50 || !${ACStartY}),/varset ACStartX ${Spawn[${TankName}].X},/varset ACStartX ${Me.X}]};/docommand ${If[${ACState}==100,/varset ACStartY ${Spawn[${TankName}].Y},/varset ACStartY ${Me.Y}]};/varset ACStartHeading ${Me.Heading.DegreesCCW}
      /varset ACState 1
    }
  } else {
    /multiline ; /varset ACState 0;/varset ACMATarget 0;/varset ACTarID 0;/varset ACBeginHP 0
  }
}
/if (${ACState}) {
  /doevents MobProb
  /if (!${ACMATarget} || ${ACState}>2 || ${Spawn[${ACMATarget}].Type.Equal[corpse]} || !${Spawn[${ACMATarget}].ID} || ${Math.Distance[${Me.Y},${Me.X}:${ACStartY},${ACStartX}]}>${ACLeash} && ${Spawn[pc ${TankName}].Distance}>30) {
    /if (${Me.Class.ShortName.Equal[BRD]} || ${Me.Casting.ID}) /multiline ; /if (!${Me.Mount.ID}) /interrupt;/squelch /stoptwist
    /squelch /tar clear
    /if (${Melee.Combat}) /attack off
    /if (${APTargetID}) /multiline ; /varset APTargetID 0;/varset APAggro 0
    /delay 1s !${Melee.Combat}
    /squelch /stick off
    /keypress centerview
    /if (${Me.Pet.ID} && ${PetAssist}) /multiline ; /varset PetAssist 1;/pet back off
    /if (${Select[${ACState},2,4]} && (!${Me.Combat} && !${Melee.Combat}||!${ACMATarget}||${Spawn[FollowName].Distance}>${ACLeash}||!${CurrentMobCount})) {
      /docommand ${If[${Defined[CurrLoc]},/call FollowFunction "${FollowName}",/varset FollowFlag 1]}
      /docommand ${If[${${FollowPlug}.Active} && ${AdvPlug},/call AdvPlugCall "${FollowName}",/varset FollowFlag 1]}
      /varset FollowFlag 1
      } else {
      /docommand ${If[${CampStatus} && !${FollowFlag} && !${ADMobCount},/varset CampStatus 1,${If[${Math.Distance[${Me.Y},${Me.X}:${ACStartY},${ACStartX}]}>10 && !${FollowFlag} && !${ADMobCount},/squelch /moveto loc ${ACStartY} ${ACStartX},/face Heading ${ACStartHeading}]}]}
      /if (${CampStatus} && !${FollowFlag} && !${ADMobCount}) {
        /delay 2s ${Math.Distance[${Me.Y},${Me.X}:${ACStartY},${ACStartX}]}<=10
        /face Heading ${ACStartHeading}
|        /multiline ; /face Heading ${ACStartHeading};/varset ACStartX ${Me.X};/varset ACStartY ${Me.Y}

      } else /if (!${CampStatus} && (${FollowFlag} || ${FollowName.Length}) && !${ADMobCount}) {
         /docommand ${If[${Defined[CurrLoc]},/call FollowFunction "${FollowName}",/varset FollowFlag 1]}
        /docommand ${If[${${FollowPlug}.Active} && ${AdvPlug},/call AdvPlugCall "${FollowName}",/varset FollowFlag 1]}
      }
    }
    /call ADSpawnCheck
    /if (${ACAfter.Length}>3) /call IniCommand "${ACAfter}"
    /if (${DoLoot} && ${LootMobs} && (!${ADMobCount} || ${MobsTooClose}<=5)) {
        /if (${SpawnCount[corpse radius ${CorpseRadius} zradius 100 noalert 25]}) {
          /call LootMobs
        }
     }
     
    /multiline ; /varset ACMATarget 0;/varset ACState 0;/varset ACTarID 0;/varset ACBeginHP 0;/return
  }
  /if (!${OffTank}) {
    /if (!${ReTargetT}) {
      /varset ReTargetT 3s
|      /echo ${ACMATarget}
      /if (${NetBots[${TankName}].ID} && ${Target.ID}!=${NetBots[${TankName}].TargetID} && !${sNBClnt.Find[${Spawn[${NetBots[${TankName}].TargetID}].CleanName}]}) {
        /if (!${ExcludeList.Find[|${Spawn[${NetBots[${TankName}].TargetID}].CleanName}|]} && ${Spawn[${NetBots[${TankName}].TargetID}].Standing}) /varset ACMATarget ${NetBots[${TankName}].TargetID}
        } else /if (!${NetBots[${TankName}].ID} && ${Target.ID}!=${ACMATarget}) {
/echo Assisting again to get target
        /call ACGetAssist ${TankName}
        /if (!${ExcludeList.Find[|${Spawn[${Macro.Return}].CleanName}|]} && ${Spawn[${Macro.Return}].Standing}) /varset ACMATarget ${Macro.Return}
      }
    }
  }
  /if (${OffTank} && ${Target.ID}!=${ACMATarget} && ${ACTarID}) {
  	/echo 188; ${Target.ID}  : ${ACTarID}
    /varset ACMATarget ${ACTarID}
    /if (${ACTarID} && ${ACBeginHP}) {
      /if  (${Spawn[${ACTarID}].PctHPs}>${ACBeginHP} || ${Spawn[${ACTarID}].PctHPs}<=${ACEndHP}) /varset ACMATarget 0
     } else {
      /varset ACMATarget 0
    }
  }
  /if (${Target.ID}!=${ACMATarget} && ${ACMATarget} && (${Me.PctMana}>${ACManaPct} || !${Me.PctMana} || ${ACManaPct}==101)) {
    /squelch /tar id ${ACMATarget}
    /delay 2s ${Target.ID}==${ACMATarget}
  }
  /if (${Target.Distance3D}<=${ACLeash} && ${ACMATarget}) {
    /if (${Me.Class.ShortName.Equal[BRD]} && ${Twist.List.NotEqual[${BardCombatGems} ]} && ${BardCombatGems.Length} > 0) /squelch /twist ${BardCombatGems}
    /if (${Me.Pet.ID} && (${PetAssist}==1 || ${PetAssist} && ${PetAssist}!=${ACMATarget})) {
      /multiline ; /pet attack;/varset PetAssist ${ACMATarget}
    }
    /if (${Me.PctMana}>${ACManaPct} || ${Me.MaxMana}==0) {
      /if (!${Me.Mount.ID} && !${Me.Standing} && (!${Melee.Combat} || !${Me.Combat})) /stand
      /if (${DoMelee} && ${Me.Standing} && ${Target.ID}==${ACMATarget} && (!${Me.AutoFire} || !${Melee.Combat} || !${Me.Combat}) && !${DoRanged}) /docommand ${If[${Plugin[mq2melee].Name.Length},/multiline ; /killthis;/varset ACMobProbT 3s,/call NoMQ2Melee]}
      /if (${DoRanged} && ${Me.Standing} && ${Target.ID}==${ACMATarget} && (!${Me.AutoFire} || !${Melee.Combat} || !${Me.Combat})) {
        /if (${FollowName.Length}) /docommand /${FollowCommand} off
        /face fast ${Target}
        /stick hold {Math.Calc[40+{Target.Height}]} moveback
        /multiline ; /autofire on;/varset ACMobProbT 3s
      }
    } else /if (${Melee.Combat}) {
      /echo Low mana..  Turning off melee.
        /squelch /tar clear
        /if (${Melee.Combat}) /attack off
        /delay 1s !${Melee.Combat}
        /squelch /stick off
    }
  }
  | /if (!${ACMaTarget} || ${Spawn[id ${ACMaTarget}].Type.Equal[corpse]} && ${FollowName.Length} && ${FollowID}) {
      |  /if (${Target.ID}!=${FollowID}) {
       	| /tar id ${FollowID}
       	| /delay 5 ${Target.ID}==${FollowID}
        |  /docommand /${FollowCommand} on
      |  }
  | }
  
}
/return

Sub NoMQ2Melee
	/if (${Target.ID}==${ACMATarget}) {
    /if (${Select[${Me.Class.ShortName},rng]} && ${Me.Autofire}) /return
    /if (${DoRanged} && !${Me.Autofire}) /multiline ; /stick hold {Math.Calc[40+{Target.Height}]} moveback;/autofire on;/face fast ${Target};/return
		/if (!${Select[${Me.Class.ShortName},war,pal,rog,mnk,rng,brd]}) /return
		/if (!${Stick.Status.Equal[on]}) /stick hold ${If[${Target.Height}<5,10,${Math.Calc[${Target.Height}+5].Int}]} ${If[!${Group.Members} || ${TankName.Equal[${Me.Name}]},moveback,${If[${Me.Class.ShortName.Equal[rog]},behind,!front]}]} ${If[${Me.Underwater},uw,]}
		/if (${Stick.Status.Equal[on]}) /attack on
	}
/return

Sub Event_EnrageStart()
	/if (${Me.Pet.ID}) /multiline ; /pet back;/echo Backing off Pet
/return

Sub Event_EnrageEnd()
	/if (${Target.ID}==${ACMATarget} && ${Me.Pet.ID}) /multiline ; /pet attack;/echo Enrage ended, re-sending Pet.
/return
