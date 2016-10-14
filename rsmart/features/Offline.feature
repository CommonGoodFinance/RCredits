Feature: Offline
AS a company agent
I WANT to accept transactions offline
SO my company can sell stuff, give refunds, and trade USD for rCredits even when no internet is available

and I WANT those transactions to be reconciled when an internet connection becomes available again
SO my company's online account records are not incorrect for long.

Setup:
  Given members:
  | id   | fullName   | email | city  | state | cc  | cc2  | rebate | flags                | *
  | .ZZA | Abe One    | a@    | Atown | AK    | ccA | ccA2 |     10 | ok,confirmed,bona    |
  | .ZZB | Bea Two    | b@    | Btown | UT    | ccB | ccB2 |     10 | ok,confirmed,bona    |
  | .ZZC | Corner Pub | c@    | Ctown | CA    | ccC |      |      5 | ok,confirmed,co,bona |
  | .ZZD | Dee Four   | d@    | Dtown | DE    | ccD | ccD2 |     10 | ok,confirmed,bona    |
  | .ZZE | Eve Five   | e@    | Etown | IL    | ccE | ccE2 |     10 | ok,confirmed,bona,secret |
  | .ZZF | Far Co     | f@    | Ftown | FL    | ccF |      |      5 | ok,confirmed,co,bona |
  And devices:
  | id   | code |*
  | .ZZC | devC |
  And selling:
  | id   | selling         |*
  | .ZZC | this,that,other |
  And company flags:
  | id   | flags        |*
  | .ZZC | refund,r4usd |
  And relations:
  | main | agent | num | permission |*
  | .ZZC | .ZZA  |   1 | buy        |
  | .ZZC | .ZZB  |   2 | scan       |
  | .ZZC | .ZZD  |   3 | read       |
  | .ZZF | .ZZE  |   1 | sell       |
  And transactions: 
  | xid | created   | type   | amount | from | to   | purpose |*
  | 1   | %today-6m | signup |    250 | ctty | .ZZA | signup  |
  | 2   | %today-6m | signup |    250 | ctty | .ZZB | signup  |
  | 3   | %today-6m | signup |    250 | ctty | .ZZC | signup  |
  | 4   | %today-6m | grant  |    250 | ctty | .ZZF | stuff   |
  Then balances:
  | id   |       r |*
  | ctty |   -1000 |
  | .ZZA |     250 |
  | .ZZB |     250 |
  | .ZZC |     250 |
  | .ZZF |     250 |

Scenario: A cashier charged someone offline
  When reconciling "C:A" on "devC" charging ".ZZB,ccB" $100 for "goods": "food" at "%now-1h" force 1
  Then we respond ok txid 5 created "%now-1h" balance -100 rewards 260 saying:
  | did     | otherName | amount | why   | reward |*
  | charged | Bea Two   | $100   | goods | $10    |
# NOPE  And with proof of agent "C:A" amount 100.00 created "%now-1h" member ".ZZB" code "ccB"
  And we notice "new charge|reward other" to member ".ZZB" with subs:
  | created | fullName | otherName  | amount | payerPurpose | otherRewardType | otherRewardAmount |*
  | %today  | Bea Two  | Corner Pub | $100   | food         | reward          | $10               |
  And balances:
  | id   |       r |*
  | ctty |   -1015 |
  | .ZZA |     250 |
  | .ZZB |     160 |
  | .ZZC |     355 |

Scenario: A cashier charged someone offline and they have insufficient balance
  Given transactions: 
  | xid | created | type     | amount | from | to   | purpose |*
  | 5   | %today  | transfer |    200 | .ZZB | .ZZA | cash    |
  When reconciling "C:A" on "devC" charging ".ZZB,ccB" $100 for "goods": "food" at "%now-1h" force 1
  Then we respond ok txid 6 created "%now-1h" balance -300 rewards 260
  And we notice "new charge|reward other" to member ".ZZB" with subs:
  | created | fullName | otherName  | amount | payerPurpose | otherRewardType | otherRewardAmount |*
  | %today  | Bea Two  | Corner Pub | $100   | food         | reward          | $10               |
  And balances:
  | id   |       r |*
  | ctty |   -1015 |
  | .ZZA |     450 |
  | .ZZB |     -40 |
  | .ZZC |     355 |

Scenario: A cashier charged someone offline but it actually went through
  Given agent "C:A" asks device "devC" to charge ".ZZB,ccB" $100 for "goods": "food" at "%now-1h"
  When reconciling "C:A" on "devC" charging ".ZZB,ccB" $100 for "goods": "food" at "%now-1h" force 1
  Then we respond ok txid 5 created "%now-1h" balance -100 rewards 260
  #And we notice nothing
  And balances:
  | id   |       r |*
  | ctty |   -1015 |
  | .ZZA |     250 |
  | .ZZB |     160 |
  | .ZZC |     355 |

Scenario: A cashier declined to charge someone offline and it didn't go through
  When reconciling "C:A" on "devC" charging ".ZZB,ccB" $100 for "goods": "food" at "%now-1h" force -1
  Then we respond ok txid 0 created "" balance 0 rewards 250
  #And we notice nothing
  And balances:
  | id   |       r |*
  | ctty |   -1000 |
  | .ZZA |     250 |
  | .ZZB |     250 |
  | .ZZC |     250 |

Scenario: A cashier canceled offline a supposedly offline charge that actually went through
  Given agent "C:A" asks device "devC" to charge ".ZZB,ccB" $100 for "goods": "food" at "%now-1h"
  When reconciling "C:A" on "devC" charging ".ZZB,ccB" $100 for "goods": "food" at "%now-1h" force -1
  Then we respond ok txid 8 created %now balance 0 rewards 250
  And with undo "5"
  And we notice "new charge|reward other" to member ".ZZB" with subs:
  | created | fullName | otherName  | amount | payerPurpose | otherRewardType | otherRewardAmount |*
  | %today  | Bea Two  | Corner Pub | $100   | food         | reward          | $10               |
  And we notice "new refund|reward other" to member ".ZZB" with subs:
  | created | fullName | otherName  | amount | payerPurpose | otherRewardType | otherRewardAmount |*
  | %today  | Bea Two  | Corner Pub | $100   | reverses #2  | reward          | $-10              |
  And balances:
  | id   |       r |*
  | ctty |   -1000 |
  | .ZZA |     250 |
  | .ZZB |     250 |
  | .ZZC |     250 |

Scenario: A cashier canceled offline a supposedly offline charge that actually went through, but customer is broke
  Given transactions: 
  | xid | created | type     | amount | from | to   | purpose |*
  | 5   | %today  | grant    |    500 | ctty | .ZZC | growth  |
  And agent "C:A" asks device "devC" to charge ".ZZB,ccB" $-100 for "goods": "refund" at "%now-1h"
  And transactions: 
  | xid | created | type     | amount | from | to   | purpose |*
  | 9   | %today  | transfer |    300 | .ZZB | .ZZA | cash    |
  When reconciling "C:A" on "devC" charging ".ZZB,ccB" $-100 for "goods": "refund" at "%now-1h" force -1
  Then we respond ok txid 10 created %now balance -300 rewards 250
  And with undo "6"
  And we notice "new refund|reward other" to member ".ZZB" with subs:
  | created | fullName | otherName  | amount | payerPurpose | otherRewardType | otherRewardAmount |*
  | %today  | Bea Two  | Corner Pub | $100   | refund       | reward          | $-10              |
  And we notice "new charge|reward other" to member ".ZZB" with subs:
  | created | fullName | otherName  | amount | payerPurpose | otherRewardType | otherRewardAmount |*
  | %today  | Bea Two  | Corner Pub | $100   | reverses #2  | reward          | $10               |
  And balances:
  | id   |       r |*
  | ctty |   -1500 |
  | .ZZA |     550 |
  | .ZZB |     -50 |
  | .ZZC |     750 |

Scenario: Device sends correct old proof for legit tx after member loses card, with app offline
  Given members have:
  | id   | cardCode |*
  | .ZZB | ccB2     |
  // member just changed cardCode
  When reconciling "C:A" on "devC" charging ".ZZB,ccB" $100 for "goods": "food" at "%now-1h" force 1
  Then we respond ok txid 5 created "%now-1h" balance -100 rewards 260 saying:
  | did     | otherName | amount | why   | reward |*
  | charged | Bea Two   | $100   | goods | $10    |

Scenario: Device sends correct old proof for legit tx after member loses card, with app online
  Given members have:
  | id   | cardCode |*
  | .ZZB | ccB2     |
  // member reported lost card, we just changed cardCode, now the member (or someone) tries to use the card with app online:
  When reconciling "C:A" on "devC" charging ".ZZB,ccB" $100 for "goods": "food" at "%now-1h" force 0
  Then we return error "bad proof"


Scenario: Device sends correct old proof for legit tx after member loses card, with tx date after the change
  Given members have:
  | id   | cardCode |*
  | .ZZB | ccB2     |
  // member reported lost card, we just changed cardCode, now the member (or someone) tries to use the card with app online:
  When reconciling "C:A" on "devC" charging ".ZZB,ccB" $100 for "goods": "food" at "%now+1h" force 1
  Then we return error "bad proof"