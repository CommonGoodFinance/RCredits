<?php
namespace rCredits\Txs; // typically abbreviated as "x"
use rCredits\Backend as be;
use rCredits\Util as u;
use rCredits\Testing as t;
use rCredits as r;
use rCredits\DB as db;
use rCredits\Txs as x;
use rCredits\Web as w;

/**
 * @file
 * Auxiliary functions for listing transactions
 */

define('DT_MAX', 100000000000); // bigger than any date
define('TX_TID_MAX', 100000000); // zero-filler
 
/**
 * Get a formatted list of transaction records.
 * @return [$header, $classes, $rows, $totals]
 */
function getTxRows($starting, $ending, $url0 = '', $maxRows = PHP_INT_MAX, $options = []) {
  $mya = r\acct();
  extract(u\just('download agents channels boxes', $options));
  if ($mya->jid) $agents = TRUE; // show which partner did it, on joint account
  list ($classes, $header, $downloadHeader) = txFields(@$download, @$agents, @$channels, @$boxes);

  if (@$download) {
    $csv = setupDownload($starting, $ending);
    u\csv($downloadHeader);
  } else $csv = FALSE;
  $totDone = $totPending = array_combine($classes, array_fill(0, count($classes), FALSE));
  
  // build the transaction list
//  $dones = ($states != STATES_PENDING);
//  $pendings = ($states != STATES_DONE);
//  $txOpts = compact(u\ray('dones pendings exchanges pexchanges'));
  $txOpts = compact(u\ray('dones pendings', TRUE, FALSE));
  $rows = [];
  
  foreach (u\ray('bankTransfers myTransactions') as $func) { // transfers first (usually not many)
    $func = "rCredits\\Backend\\$func";
//    $q = $func($starting, $ending, $report ? 'ctty' : $mya->id, $txOpts);
    $q = $func($starting, $ending, $mya->id, $mya->jid ?: 0);
    while (count($rows) < $maxRows) {
      if (!$tx = $q->fetchAssoc()) break;
      //$tot = (strpos($func, 'myTrans') or $tx['completed']) ? 'totDone' : 'totPending';
      $rows[] = doRow($tx, $totDone, $classes, $csv, @$agents, @$channels, @$boxes, $url0);
    }
  }
  
  if (@$tx and $q->fetchAssoc()) r\Web\say('transaction list truncated'); // there was more than we could show
  if (@$download) {
    u\splice($totDone, 'state', 2); // don't download status or buttons
    u\splice($totDone, 'fromyou', 0, u\ray('frombank', 0));
    $totDone['date'] = '';
    $totDone['name'] = 'TOTALS';
    foreach(u\just(TX_DOWNLOAD_MONEY_FIELDS, $totDone) as $key => $value) $totDone[$key] = round($value, 2);
    //if ($value === FALSE) $totDone[$key] = '-';
    u\csv(array_values($totDone));
    if (!u\test()) exit();
  }
  
  return array($header, $classes, @$download ? $rows : sortRows($rows), $totDone);
}

/**
 * Prepare one transaction row for output (or download).
 * @param assoc $tx: the transaction record
 * @param file $csv: the (open) download file or FALSE if not downloading
 *
 * @return: the row
 */
function doRow($tx, &$tot, $classes, $csv = '', $agents = FALSE, $channels = FALSE, $boxes = FALSE, $url0 = '') {
  global $rUrl;
  
  $mya = r\acct();
  $myx = r\x($tx, $mya);
  foreach (u\ray('xid amount byMe toMe fromMe reallyToMe agent data banking tid') as $k) $$k = $myx->$k;

  if ($banking and ($amount < 0 xor $xid < 0)) $amount = -$amount; // show as positive unless reversing
  $isExtra = ($myx->type != TX_TRANSFER and !$banking);
  $state = txColoredState($data);
  
  $url = "$url0&xid=" . ($banking ? 'b' : '') . $xid;
  $reward = $banking ? 0 : ($toMe ? (@$data['bonus'] + @$data['xfee']) : (@$data['rebate'] - @$data['xfee']));
  $net = number_format(($toMe ? $amount : -$amount) + $reward, 2);
  
  if (isset($data['xfee'])) {$amount = -$amount; $toMe = !$toMe;}
  $amount = number_format($amount, 2);
  txNeatAmounts($amount, $reward, $isExtra);
  if ($toMe and $fromMe) {
    $toYou = $fromYou = "<span class=\"virtual\">$amount</span>"; // make old draws clear
  } else list ($toYou, $fromYou) = u\order($toMe, $amount, R_NONE);
  
  $button1 = (!$byMe and @$data['disputed']) ? w\button('OK', "$url&do=ok", $myx->msg('theyDid')) : ''; // w\txAct($toMe, $byMe, $reallyToMe)
  $button2 = ((($mya->can(@$reallyToMe ? B_BUY : B_SELL) and !$isExtra and !$banking) or $mya->admin) and !@$data['undoneBy']) ? w\button(' X ', "$url&do=no", $myx->msg('reverse')) : ''; // (admin not cAdmin)
  //was txAct($toMe, $byMe, $reallyToMe)
  $button3 = ($mya->can(@$reallyToMe ? B_SELL : B_BUY) and !$isExtra and !$banking) ? "<a href='$url&do=edit' title='Edit Transaction'><img src='$rUrl/images/icons/edit.png' width=20 height=20 border=0 /></a>" : '';
//  if (!$mya->can(B_MANAGE)) $button1 = $button2 = $button3 = '';
  $buttons = "<div class='txs-buttons'><div>$button1</div><div>$button2</div><div>$button3</div></div>";
//    $row = array($tid, strftime('%d-%b', $tx_created), @$myx->name, $fromYou, $toYou, $rpct, $amount, $usd, $buttons, $myx->for, $reward, $net);
  $row = array($tid, strftime($csv ? '%Y-%m-%d' : '%d-%b', $myx->created), @$myx->name2, $fromYou, $toYou, $state, $buttons, $myx->for, $reward, $net);

  if (@$agents) $row[] = (!@$agent or ($mya->isMe($agent) and $mya->co) or $agent < 0) ? R_NONE 
  : ($csv ? r\qid($agent) : substr(r\qid($agent), 3));
  if (@$channels and $txChannels = u\ray(TX_CHANNELS)) $row[] = $txChannels[$myx->channel];
  if (@$boxes) $row[] = db\lookup('boxnum', 'r_boxes', 'id=:tx_box', compact('tx_box'));
  addRow($tot, array_combine($classes, $row), $tx, $csvRow);
  if ($csv) return u\csv($csvRow); // not STDOUT
//  finishCsvRow($csv, array_combine($classes, $row), $tx, $tot);
  
  $sort = (DT_MAX + $myx->created) . (TX_TID_MAX + $tid) . ($banking ? 5 : 0) . u\randomFloat(.0001, .9999); // make it a string so as not to lose precision
  array_unshift($row, $sort); // sort by date (fails around the year 2260), then by tid
  
  return $row;
}

/**
 * Sort the rows descending by the first column and discard it.
 * @param array $rows: the unsorted rows
 * @return array: those rows without the sort column, sorted
 */
function sortRows($rows) {
  if (!@$rows) return []; // no records
  $count = count($rows);
  foreach ($rows as $i => $row) {
    $keys[] = array_shift($rows[$i]); // assume we never will list more than a million at once
  }
  $rows = array_combine($keys, $rows);
  krsort($rows, SORT_STRING);
  u\EXPECT(count($rows) == $count, "ksort lost rows: was $count, now " . count($rows));
  return array_values($rows);
}

function setupDownload($starting, $ending) {
  $mya = r\acct();
  $dt1 = strftime('%Y%m%d', $starting);
  $dt2 = strftime('%Y%m%d', $ending);
  return u\csvStart(($mya->admin ? $mya->mainQid : 'rcredits') . "$dt1-$dt2.csv");
}

/**
 * Return classes and headers for the appropriate set of fields.
 */
function txFields($download = FALSE, $agents = FALSE, $channels = FALSE, $boxes = FALSE) {
  global $agtViaBoxHelp;
  $headHelp = t('Transaction number|Date transaction was completed|Payer name|Payee name|Transaction amount|Current state of transaction -- check mark means completed.|What is the purpose of the transaction (usually the item or service purchased)|How much incentive rewards were awarded for this transaction|Who did you pay or receive from|Amounts you spent|Amounts you received|Click a symbol to reverse a transaction or to edit the purpose|The net change in account balance') . @$agtViaBoxHelp;
  $classes = 'tid date from to amount state purpose reward';
  $helpFields = $classes . ' name fromyou toyou buttons net' . (@$agtViaBoxHelp ? ' agent channel box' : '');
  $headHelp = array_combine(u\ray($helpFields), u\ray($headHelp));
/*  if ($report) { // community transaction history
    $headers = t('Tx#,Date,From,To,Total,Status,Purpose,Rewards');
  } else { // individual account transaction history
  */
  $classes = 'tid date name fromyou toyou state buttons purpose reward net';
  $headers = t('Tx#,Date,Name,From you,To you,Status,Action,Purpose,Reward/Fee,Net');
//  }

  $header = array_combine(u\ray($classes), u\ray($headers)); // make splices and additions easy and clear
  
  if (@$agents) $header['agent'] = t('Agent');
  if (@$channels) $header['channel'] = t('Via');
  if (@$boxes) $header['box'] = t('Box');

  if (@$download) { // downloading -- modify standard individual or community field list
    $downloadHeader = $header;
    u\splice($downloadHeader, 'state', 2); // don't download status or buttons
    u\splice($downloadHeader, 'fromyou', 0, u\ray('frombank', t('From bank')));
  }

  foreach ($header as $k => $v) $header[$k] = w\popHelp($v, $headHelp[$k]);
  
  return array(array_keys($header), array_values($header), @array_values($downloadHeader));
}

/**
 * Update the totals with values from this row.
 * @param assoc $tot: (UPDATED) the running totals
 * @param assoc $row: keyed values for the current record
 * @param assoc $tx: the transaction record
 * @param array $csvRow: (RETURNED) the values, formatted for CSV export
 */
function addRow(&$tot, $row, $tx, &$csvRow = '') {
  extract(u\just('xid type toMe amount', $tx));

  if ($type == TX_BANK) {
    $frombank = $toMe ? abs($amount) : -abs($amount);
    $row['fromyou'] = $row['toyou'] = '';
  } else $frombank = '';
  
  u\splice($row, 'state', 2); // don't download status or buttons
  u\splice($row, 'fromyou', 0, compact('frombank'));

  foreach ($row as $k => $v) $row[$k] = $v == R_NONE ? '' : strip_tags($v);
//  if ($row['fromyou'] == $row['toyou']) $row['fromyou'] = $row['toyou'] = ''; // don't report gross on payex
  foreach (u\ray(TX_DOWNLOAD_MONEY_FIELDS) as $k) {
    $row[$k] = u\cleanAmount(@$row[$k]) + 0;
    $tot[$k] = (@$tot[$k] ?: 0) + $row[$k]; // formatting value is useless
  }
  $csvRow = array_values($row);
}

function txColoredState($data) {
  $states = array('&#10004;', 'disputed');
  $colors = array('green', 'red');
  $state = @$data['disputed'] ? 1 : 0;
  $color = $colors[$state];
  $state = $states[$state];
  return "<span style='color:$color;'>$state</span>";
}

function txNeatAmounts(&$amount, &$reward, $isExtra) {
  $reward = $reward ? number_format($reward, 2) : R_NONE;
  if ($isExtra) list ($amount, $reward) = array(R_NONE, $amount); // show refunds, inflation, grants, fines, and signup bonus as rewards
}

/**
 * Sum the amounts in each transaction category, based on description.
 * @param array $cats: (MODIFIED) counts and amount totals, indexed by category description
 * @param string $purpose: description of transaction
 * @param numeric $amount: transaction amount
 * @param bool $dups: (RETURNED) set TRUE if this category has at least two transactions in it
 */
function addCat(&$cats, $purpose, $amount, &$dups) {    
  if (strpos($purpose, '(') === 0 and ($i = strpos($purpose, ')')) and $i < strlen($purpose) - 1) {
    $purpose = trim(substr($purpose, $i + 1)); // remove reversal info, if any
  }
  if ($m = strpbrk($purpose, ':-;(/[{~#*') and $i = strpos($purpose, $m) and $i < strlen($purpose) - 1) {
    $cat = trim(substr($purpose, 0, $i));
  } else $cat = $purpose;
  
  if ($one = @$cats[$cat]) $dups = TRUE; else $one = array(0, 0);
  list ($oldCount, $oldAmount) = $one;
  $cats[$cat] = array($oldCount + 1, $oldAmount + $amount);
}

/**
 * Adjust the categories list before displaying.
 */
function finishCats(&$cats) {
  $otherInCount = $otherIn = $otherOutCount = $otherOut = 0;
  foreach ($cats as $cat => $one) if ($one[0] == 1 and $cat != INFLATION_DESC) { // show inflation in summary
    if ($one[1] < 0) {
      $otherOutCount++; $otherOut += $one[1];
    } else {$otherInCount++; $otherIn += $one[1];}
    
    unset($cats[$cat]);
  }
  
  ksort($cats);
  if ($otherInCount) $cats[t('other funds IN')] = array($otherInCount, $otherIn);
  if ($otherOutCount) $cats[t('other funds OUT')] = array($otherOutCount, $otherOut);
}