|Wait4rez.inc v2.21 
Sub Wait4Rez 
/declare zoneid int local ${Zone.ID}
/call consentNB
/echo Awaiting rez: auto-accepting, and looting my corpse
:rezmeffs 
/doevents 
/if (${Me.MaxMana} && !${Me.Gem[1].Name.Length} && !${Me.Gem[2].Name.Length}) {
		/ssm modbot2
		/delay 5s
	}
	/if (!${TitaniumBuild}) {
      /if (${Window[TradeWnd].Open} && !${Window[TradeWnd].MyTradeReady} && (${MasterList.Find[${Window[TradeWnd].Child[TRDW_HisName].Text}]} || ${NetBots.Client.Find[${Window[TradeWnd].Child[TRDW_HisName].Text}]})) /notify TradeWnd TRDW_Trade_Button leftmouseup
    }
/delay 1s ${Window[ConfirmationDialogBox].Open} 
/if (!${Window[ConfirmationDialogBox].Open}) /goto :rezmeffs 
/nomodkey /notify ConfirmationDialogBox Yes_Button leftmouseup 
/delay 2s 
/if (${Window[RespawnWnd].Open}) /nomodkey /notify RespawnWnd RW_OptionsList listselect 2  
/delay 1s 
/nomodkey /notify RespawnWnd RW_SelectButton leftmouseup 
/delay 2s 
:zoneffs 
/delay 5m ${Spawn[${Me}'s corps].ID} 
/delay 5s ${Spawn[${Me}'s corps].Distance}<100 
/if (${Spawn[${Me}'s corps].ID}) /target id ${Spawn[${Me}'s corps].ID}
/delay 5s 
/delay 5s ${Target.ID}==${Spawn[${Me}'s corps].ID} 
/if (!${Target.CleanName.Find[${Me}'s Corpse]}) /goto :zoneffs 
:corpsepull 
/echo corpsepull
/if (${Target.Distance}>100 || !${Spawn[${Me}'s corps].ID}) /return 
/if (${Target.Distance}>20) /corpse 
/delay 5s ${Target.Distance}<20 
/loot 
/delay 5s ${Me.State.Equal[BIND]} 
/if (${Me.State.NotEqual[BIND]}) /goto :corpsepull 
/declare loottotal int local 

:LootLag 
| /echo lootlag
/varset loottotal ${Corpse.Items} 
/delay 1s ${loottotal}!=${Corpse.Items} 
/if (${loottotal}!=${Corpse.Items}) /goto :LootLag 
/declare i int local 
/for i 1 to ${loottotal}
:lootagain 
|/echo lootagain
/itemnotify loot${i} leftmouseup 
/delay 2s !${Corpse.Item[${i}].ID} && ${Cursor.ID}
/if (${Corpse.Item[${i}].ID}) /goto :lootagain 
:InvItems
/if (${Cursor.ID}) /autoinventory
/delay 2 !${Cursor.ID}
/if (${Cursor.ID}) /goto :InvItems
/next i 
/nomodkey /notify LootWnd DoneButton leftmouseup 
/return

sub consentNB
	/declare x int local
	/delay 1
	/for x 1 to ${NetBots.Counts}
		/consent ${NetBots.Client.Arg[${x}]}
		/delay 1s
	/next x	
/return
