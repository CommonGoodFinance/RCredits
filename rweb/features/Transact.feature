Feature: Transact
AS a member
I WANT to transfer rCredits to or from another member (acting on their own behalf)
SO I can buy and sell stuff.
# We will eventually need variants or separate feature files for neighbor (member of different community within the region) to member, etc.
# And foreigner (member on a different server) to member, etc.

Setup:
  Given members:
  | id      | full_name  | phone  | email         | city  | state  | country       | 
  | NEW.ZZA | Abe One    | +20001 | a@example.com | Atown | Alaska | United States |
  | NEW.ZZB | Bea Two    | +20002 | b@example.com | Btown | Utah   | United States |
  | NEW.ZZC | Corner Pub | +20003 | c@example.com | Ctown | Corse  | France        |
  And relations:
  | id      | main    | agent   | permission        |
  | NEW:ZZA | NEW.ZZA | NEW.ZZB | buy and sell      |
  | NEW:ZZB | NEW.ZZB | NEW.ZZA | read transactions |
  | NEW:ZZC | NEW.ZZC | NEW.ZZB | buy and sell      |
  | NEW:ZZD | NEW.ZZC | NEW.ZZA | sell              |
  And transactions: 
  | tx_id    | created   | type       | amount | from      | to      | purpose | taking |
  | NEW.AAAB | %today-6m | %TX_SIGNUP |    250 | community | NEW.ZZA | signup  | 0      |
  | NEW.AAAC | %today-6m | %TX_SIGNUP |    250 | community | NEW.ZZB | signup  | 0      |
  | NEW.AAAD | %today-6m | %TX_SIGNUP |    250 | community | NEW.ZZC | signup  | 0      |
  Then balances:
  | id        | balance |
  | community |    -750 |
  | NEW.ZZA   |     250 |
  | NEW.ZZB   |     250 |
  | NEW.ZZC   |     250 |

#Variants: with/without an agent
#  | "NEW.ZZA" asks device "codeA" | "NEW.ZZC" asks device "codeC" | "NEW.ZZA" $ | "NEW.ZZC" $ | # member to member (pro se) |
#  | "NEW.ZZB" asks device "codeA" | "NEW.ZZB" asks device "codeC" | "NEW.ZZA" $ | "NEW.ZZC" $ | # agent to member           |
#  | "NEW.ZZA" asks device "codeA" | "NEW.ZZC" asks device "codeC" | "NEW:ZZA" $ | "NEW:ZZC" $ | # member to agent           |
#  | "NEW.ZZB" asks device "codeA" | "NEW.ZZB" asks device "codeC" | "NEW:ZZA" $ | "NEW:ZZC" $ | # agent to agent            |

Scenario: A member asks to charge another member
  When member "NEW.ZZA" completes form "tx" with values:
  | op     | who     | amount | goods | purpose |
  | Charge | Bea Two | 100    | 1     | labor   |
  Then we show "confirm charge" with subs:
  | amount | other_name |
  | $100   | Bea Two    |
  
Scenario: A member confirms request to charge another member
  When member "NEW.ZZA" confirms form "tx" with values:
  | op     | who     | amount | goods | purpose |
  | Charge | Bea Two | 100    | 1     | labor   |
  Then we say "status": "report invoice" with subs:
  | action  | other_name | amount | tid |
  | charged | Bea Two    | $100   | 2   |
  And we email "new-invoice" to member "b@example.com" with subs:
  | created | full_name | other_name | amount | payer_purpose |
  | %today  | Bea Two   | Abe One    | $100   | labor         |
  And we show "tx" with subs:
  | arg1   |
  | charge |
  And transactions:
  | tx_id    | created   | type      | state       | amount | from      | to      | purpose | taking |
  | NEW.AAAE | %today | %TX_TRANSFER | %TX_PENDING |    100 | NEW.ZZB   | NEW.ZZA | labor   | 1      |
  | NEW.AAAF | %today | %TX_REBATE   | %TX_PENDING |      5 | community | NEW.ZZB | rebate  | 0      |
  | NEW.AAAG | %today | %TX_BONUS    | %TX_PENDING |     10 | community | NEW.ZZA | bonus   | 0      |
  And balances:
  | id        | balance |
  | community |    -750 |
  | NEW.ZZA   |     250 |
  | NEW.ZZB   |     250 |
  | NEW.ZZC   |     250 |

  