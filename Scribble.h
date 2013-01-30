
/// 
/// ------------------  AUTHOR/TITLE REQUIRES TEXT TAG?  ------------------
///

0x1994: *** DIFFERENCE in PageID:1265 StringID:203:
BEFORE: 
[author]Script Engine[/author][text]Infinite loop detected, script is terminated!

%s
%s[/text]**END**

AFTER: 
[author]Script Engine[/author]Infinite loop detected, script is terminated!

%s
%s**END**

/// 
/// ------------------  RANKING BEFORE AUTHOR/TITLE ------------------
///

0x1994: *** DIFFERENCE in PageID:1293 StringID:40:
BEFORE: 
[ranking type='trade' title='$TRADERANKINT$'/][author]$AUTHOR$[/author]**END**

AFTER: 
[author]$AUTHOR$[/author][ranking type='trade' title='$TRADERANKINT$'/]**END**


/// 
/// ------------------  AUTHOR/TITLE CAN BE FORMATTED ------------------
///

0x1994: *** DIFFERENCE in PageID:1311 StringID:100:
BEFORE: 
[author]Station Management System[/author]\033GSMS Mini started:\033X %s
\(to start the Standard-Version you have to adjust the ... using the SMS-Cofiguration.\)**END**

AFTER: 
[author]Station Management System[/author][green]SMS Mini started:[/green] %s
\(to start the Standard-Version you have to adjust the ... using the SMS-Cofiguration.\)**END**


/// 
/// ------------------  BUTTON ID CAN BE EMPTY  ------------------
///

0x1994: *** DIFFERENCE in PageID:1292 StringID:31:
BEFORE: 
[author]Pirate Scanner[/author][text cols='2' colwidth='200' colspacing='30']New pirate ship found:

%s
ID: %s
Distance: %d km
[center][select value='TargetPirateShip']Target Pirate Ship[/select]

[select]Ignore Pirate Ship[/select]

[select value='DisableScanner']Disable Pirate Scanner[/select][/center][/text]**END**

AFTER: 
[author]Pirate Scanner[/author][text cols='2' colwidth='200' colspacing='30']New pirate ship found:

%s
ID: %s
Distance: %d km
[center][select value='TargetPirateShip']Target Pirate Ship[/select]

[select value='']Ignore Pirate Ship[/select]

[select value='DisableScanner']Disable Pirate Scanner[/select][/center][/text]**END**

/// 
/// ------------------  COLOUR TAGS CHANGED ------------------
///

0x1994: *** DIFFERENCE in PageID:1311 StringID:104:
BEFORE: 
[author]Station Management System[/author]\033RAdvice:\033X SMS Deluxe could not be started, because there were problems with deducting the starting fee \(%s Credits\)!**END**

AFTER: 
[author]Station Management System[/author][red]Advice:[/red] SMS Deluxe could not be started, because there were problems with deducting the starting fee \(%s Credits\)!**END**


0x1a4c: *** Failed to generated RichText for PageID:1294 StringID:1
0x1a4c: *** DIFFERENCE in PageID:1311 StringID:100:
BEFORE: 
[author]Station Management System[/author]\033GSMS Mini started:\033X %s
\(to start the Standard-Version you have to adjust the ... using the SMS-Cofiguration.\)**END**

AFTER: 
[author]Station Management System[/author][green]SMS Mini started:[/green] %s
\(to start the Standard-Version you have to adjust the ... using the SMS-Cofiguration.\)**END**


/// 
/// ------------------  @xxxx PREFIX ------------------
///


0x1a4c: *** DIFFERENCE in PageID:1316 StringID:4001:
BEFORE: 
@ANDI[ranking type='fight' title='$FIGHTRANK$'/][author]$AUTHOR$[/author][title]Federation Betrayal[/title][text cols='2'][justify]You have been traced to this station by officers of an elite $ARACEN$ Intelligence squad operating in this area under a veil of secrecy. I am Commander $AUTHOR$. We are in the process of locating an individual who has been working under our authority as an intermediary between ourselves and various Pirate clans in an effort to cease hostilities. Unfortunately this individual, of $VRACEN$ origin, whom we have confirmed to hold an alias of $VICTIM$, has recently taken it upon himself to return to his home sector of $VSECFN$ along with several thousand terabytes of classified data we currently hold of Pirate clandestine operations. The disclosure of this material to the Pirate home sectors would no doubt cause an immediate rise in hostilities, something we cannot afford at this time due to our myriad enemies making inroads into all sectors that we consider to be of strategic importance.

We have been able to track $VICTIM$ for the past mizura; he was last seen flying out of a station in $VSECST$. We are expecting him to reach $VSECFN$ as an end-point in his journey at which time he will transfer the data by courier to another source we have not yet been able to identify. This must be avoided at all costs. We have been authorised to offer you $REWARD$ credits in order to terminate this individual. Please confirm your acceptance or rejection of this offer. Should you decline, you will be ordered to undertake a retinal scan and placed under surveillance in order that any information given to you here is not leaked.[/justify]

[center][select value='start']Contact $AUTHOR$[/select][/center][/text]**END**

AFTER: 
[ranking type='fight' title='$FIGHTRANK$'/][author]$AUTHOR$[/author][title]Federation Betrayal[/title][text cols='2'][justify]@ANDIYou have been traced to this station by officers of an elite $ARACEN$ Intelligence squad operating in this area under a veil of secrecy. I am Commander $AUTHOR$. We are in the process of locating an individual who has been working under our authority as an intermediary between ourselves and various Pirate clans in an effort to cease hostilities. Unfortunately this individual, of $VRACEN$ origin, whom we have confirmed to hold an alias of $VICTIM$, has recently taken it upon himself to return to his home sector of $VSECFN$ along with several thousand terabytes of classified data we currently hold of Pirate clandestine operations. The disclosure of this material to the Pirate home sectors would no doubt cause an immediate rise in hostilities, something we cannot afford at this time due to our myriad enemies making inroads into all sectors that we consider to be of strategic importance.

We have been able to track $VICTIM$ for the past mizura; he was last seen flying out of a station in $VSECST$. We are expecting him to reach $VSECFN$ as an end-point in his journey at which time he will transfer the data by courier to another source we have not yet been able to identify. This must be avoided at all costs. We have been authorised to offer you $REWARD$ credits in order to terminate this individual. Please confirm your acceptance or rejection of this offer. Should you decline, you will be ordered to undertake a retinal scan and placed under surveillance in order that any information given to you here is not leaked.

[/justify][center][select value='start']Contact $AUTHOR$[/select][/center][/text]**END**




/// 
/// ------------------  MISSED COLOUR ------------------
///


0x1fa8: *** DIFFERENCE in PageID:17 StringID:20097:
BEFORE: 

      A powerful ship only recently confirmed to be part of the Imperial Paranid Navy, the Poseidon is intended as a long-term replacement for the ageing Odysseus due to the ships' alarming increase in technical failures and recent poor combat performance against enemy incursions.


      As it was found that the Odysseus was unable to properly defend itself when attacked from above or beneath, the Poseidon made up for its predecessor's weaknesses by creating a smaller profile and more effective turret coverage. However, because of the greatly reduced size of the Poseidon, its power generators cannot maintain the same level of shielding. The Poseidon compensates for this through its state of the art weaponry and internal as well as external defence systems. Information regarding the power and nature of these defence systems is currently classified to halt the development of countermeasures by foreign powers, mercenaries and pirate clans.
    **END**

AFTER: 

      A powerful ship only recently confirmed to be part of the Imperial Paranid Navy, the Poseidon is intended as a long-term replacement for the ageing Odysseus due to the ships' alarming increase in technical failures and recent poor combat performance against enemy incursions.


      As it was found that the Odysseus was unable to properly defend itself when attacked from above or beneath, the Poseidon made up for its predecessor's weaknesses by creating a smaller profile and more effective turret coverage. However, because of the greatly reduced size of the Poseidon, its power generators cannot maintain the same level of shielding. The Poseidon compensates for this through its state of the art weaponry and internal as well as external defence systems. Information regarding the power and nature of these defence systems is currently classified to halt the development of countermeasures by foreign powers, mercenaries and pirate clans.
    **END**

__