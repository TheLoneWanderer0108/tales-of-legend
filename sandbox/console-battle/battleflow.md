StartBattle: The fight begins, the battle as of right now will be a party of 3 characters vs 2 enemie characters
RoundStart: Here the round begins, it shows the characters being idle waiting for a command
DecideTurnOrder: The spd stat is the factor that says who will perform an action first
ActorTurn: This is the turn of a single actor
    CommandSelect: The player will choose an actions available (Attack, magick, defend) run is not used since its for a pure fight
    TargetSelect: Here the player selects an enemie or ally or perform the action
    Executing: Damage or healing is applied based on the actions taken
    TurnEnd: this actor turns end and continues to the next one
RoundEnd: Status ticks here
CheckEnd: This checks if all enemies or all allies are dead, checks the condition for the fight to end or continue it, checks the hp after all damage ahs been applied


1. Recompute turn order each round, or lock at StartBattle? Why?
For this example at the end of each round we will compute spd to see who starts first
2. Statuses tick at TurnEnd or RoundEnd?
At RoundEnd that way all posion checks happen after the round ends for everybody
3. Side wiped mid-round → end now or finish the round?
End one if they are end, the fight ends
4. Last hero and last enemy die same action → Victory or Defeat?
It should be defeat, the enemies goal is to defeat you
5. Actor already dead when their slot arrives → ?
Will check if they are alive before that happens, and skip there turn


Stats{
    MaxHp;
    Maxfp;
    CurrentHp;
    CurrentFp;
    Attack;
    Defense;
    Spd;
}
StatusEffect{
    kind;
    turnsleft;
}

Actor{
    BattleId;
    name;
    Stats;
    Type; //friend or foe
    Statuses;
    defending;
    skillIds;
}

SkillDef{
    Name;
    BasePow;
    Element;
    FpCost;
    Targetkind;
}

itemDef{
    Name;
    itemType;
}

Battle{
    std::<Actor> actors;
    currentPhase;
    TurnId;
    inventory;
}