<?php
/**
 * @file
 * Specialized settings
 * To be inserted in place of database settings in settings.php like this:
 * require_once __DIR__ . '/../all/modules/rcredits/boot.php';
 * Also used by /do.php, for no-sign-in database changes
 */
 
define('R_VERSION', '2.48'); // software version
define('STAGE_SUBDOMAIN', 'ws');
define('STAGE', STAGE_SUBDOMAIN . '.rcredits.org'); // staging site (for secrets, etc.)
define('PRODUCTION', 'new.rcredits.org'); // production site (used for setting $base_url)
define('GAME_SITE', 'game.rcredits.org');

$host = strtolower($_SERVER['HTTP_HOST']);
define('isPRODUCTION', $host == PRODUCTION);
define('isSTAGE', substr($host, 0, 6) == 'stage-' or $host == STAGE);
define('isGAME', FALSE or (@$_SERVER['HTTP_HOST'] == GAME_SITE));
define('isDEV', !isPRODUCTION and !isSTAGE and !isGAME); // developing

define('DEV_ROOT', '/rMembers'); // path to drupal root on DEV machine
define('DEV_PW2', 'devPassword#two!'); // second password on DEV machine (len must be 16, 24, or 32)
define('DEV_ADMIN_PASS', '123'); // admin password when isDEV

// time periods are defined here because this script uses them
define('MIN_SECS', 60);
define('HOUR_SECS', 60 * MIN_SECS);
define('DAY_SECS', 24 * HOUR_SECS);
define('WEEK_SECS', 7 * DAY_SECS);
define('WEEK7_SECS', 52 * DAY_SECS);
define('MONTH_DAYS', 30.4368);
define('YEAR_SECS', round(12 * MONTH_DAYS * DAY_SECS));

global $R_POST; $R_POST = $_POST;

$secrets = (array) json_decode(utf8_encode(file_get_contents(dirname($_SERVER['DOCUMENT_ROOT']) . '/.databases')));
global $db_name;
$db_name = key($secrets); // rCredits members db must be the first db name in the file
extract((array) $secrets[$db_name], EXTR_PREFIX_ALL, 'db');

define('R_WORD', hex2bin($db_word));
define('R_INVITE_KEY', $db_inviteKey); // this value is the same for all rCredits servers (except DEV)
//define('DW_API_KEY', isPRODUCTION ? $db_dwollaKey :$db_dwSandKey);
//define('DW_API_SECRET', isPRODUCTION ? $db_dwollaSecret :$db_dwSandSecret);
define('R_SSN_USER', @$db_ssnUser);
define('R_SSN_PASS', @$db_ssnPass);
define('R_SALTY_PASSWORD', $db_pass); // (Drupal's salt is too long for our encryption algorithm)
define('POSTMARK_TOKEN', $secrets['postmarkToken']);
// $db_salt is used further below
// define('R_STAGE_WORD', @$db_stageWord); // password for staging (UNUSED)

$databases = array (
  'default' => 
  array (
    'default' => 
    array (
      'database' => $db_name,
      'username' => $db_user,
      'password' => $db_pass,
      'collation' => 'utf8_general_ci', // or utf8mb4_general_ci
      'host' => $db_host,
      'port' => $db_port,
      'driver' => $db_driver,
      'prefix' => '',
      'pdo' => [PDO::MYSQL_ATTR_INIT_COMMAND => 'SET NAMES utf8'], // or utf8mb4
    ),
  ),
);

ini_set('error_reporting', isDEV ? E_ALL : (E_ALL & ~E_NOTICE & ~E_DEPRECATED & ~E_STRICT));
ini_set('max_execution_time', isDEV ? 0 : 240); // don't ever timeout when developing
ini_set('session.gc_probability', 1); // garbage collection this fraction of the time on startup (numerator)
ini_set('session.gc_divisor', 100); // (denominator)
define('SESSION_LIFE', @$_COOKIE['pw2'] ? 12 * HOUR_SECS : 24 * MIN_SECS); // standard 24 mins (more for admin)
ini_set('session.gc_maxlifetime', SESSION_LIFE); // session timeout (and garbage collection happens)
ini_set('session.cookie_lifetime', 0); // SESSION_LIFE); // (drupal doesn't refresh the cookie)

$conf['cron_safe_threshold'] = 0; // disable poorman's cron
if (isDEV) {
  error_reporting(E_ALL);
  ini_set('display_errors', TRUE);
  ini_set('display_startup_errors', TRUE);
}

$uri = 'I' . $_SERVER['REQUEST_URI']; // handle scanned rCard URIs
if (isDEV) $uri = str_replace(DEV_ROOT . '/', '/', $uri);
if (preg_match('~^I/[A-Z]+[\.-]~', $uri)) $_GET['q'] = $_SERVER['REQUEST_URI'] = $uri;

$drupal_hash_salt = $db_salt;
$protocol = (isDEV or isGAME) ? 'http://' : 'https://';
$base_url = $protocol . $_SERVER['HTTP_HOST'] . (isDEV ? DEV_ROOT : ''); // NO trailing slash!
define('BASE_URL', $base_url);
define('PROMO_URL', isDEV ? 'http://localhost/rPromo/_site' : 'http://rcredits.org');
define('R_PATH', '/sites/all/modules/rcredits');
global $rUrl; $rUrl = BASE_URL . R_PATH;