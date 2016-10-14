Feature: Changes
AS a community administrator
I WANT to review the significant changes to an rCredits account
SO I can provide informed customer service

AS an overall administrator
I WANT to review the significant changes to an rCredits account
SO I can request changes to software, that will enhance the experience of rCredits members

Setup:
  Given members:
  | id   | fullName | address | city | state | flags        | minimum | achMin | saveWeekly | share |*
  | .ZZA | Abe One  | 1 A St. | Aton | MA    | ok,bona,ided |     100 |     10 |          0 |    20 |
  | .ZZB | Bea Two  | 2 B St. | Bton | MA    | ok,bona,debt |     200 |     20 |          0 |    50 |
  | .ZZC | Cor Pub  | 3 C St. | Cton | CA    | ok,co,bona   |     300 |     30 |          0 |    50 |
  | .ZZD | Dee Four | 4 D St. | Dton | DE    | ok,admin     |     400 |     40 |          0 |    50 |

Scenario: A member changes some settings
  Given member ".ZZA" completes form "settings/preferences" with values:
#  | minimum | achMin | savingsAdd | smsNotices | notices | statements | debtOk | secretBal | share |*
#  |     100 |     11 |          0 |          0 |       1 |          0 |      1 |         0 |    25 |
  | notices | statements | secretBal | share |*
  |  weekly |      paper |         0 |    25 |
  Given member ".ZZA" completes form "settings/bank" with values:
  | connect | routingNumber | bankAccount | bankAccount2 | refills | target | achMin | saveWeekly |*
  |       1 |     211870281 |         123 |          123 |       1 |    100 |     11 |          0 |
  When member ".ZZD" visits page "sadmin/changes/NEW.ZZA"
  Then we show "Account Changes for Abe One" with:
  | Date | Field       | Old Value | New Value |
  | %dmy | flags       | member ok bona ided | member ok bona ided refill weekly paper |
  | %dmy | share       |        20 | 25 |
  | %dmy | hasBank     |           | 1  |
  | %dmy | bankAccount |           | USkk211870281123 |    
  | %dmy | achMin      |        10 | 11 |
#  | %dmy | flags   | member ok bona | member ok bona weekly debt |