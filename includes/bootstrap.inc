<?php

/**
 * @file
 * Functions that need to be loaded on every Drupal request.
 */

/**
 * The current system version.
 */
define('VERSION', '6.38-dev');

/**
 * Core API compatibility.
 */
define('DRUPAL_CORE_COMPATIBILITY', '6.x');

/**
 * Minimum supported version of PHP.
 */
define('DRUPAL_MINIMUM_PHP',    '5.2.4');

/**
 * Minimum recommended value of PHP memory_limit.
 */
define('DRUPAL_MINIMUM_PHP_MEMORY_LIMIT',    '16M');

/**
 * Minimum supported version of MySQL, if it is used.
 */
define('DRUPAL_MINIMUM_MYSQL',  '4.1.1');

/**
 * Minimum supported version of PostgreSQL, if it is used.
 */
define('DRUPAL_MINIMUM_PGSQL',  '7.4');

/**
 * Maximum age of temporary files in seconds.
 */
define('DRUPAL_MAXIMUM_TEMP_FILE_AGE', 21600);

/**
 * Indicates that the item should never be removed unless explicitly told to
 * using cache_clear_all() with a cache ID.
 */
define('CACHE_PERMANENT', 0);

/**
 * Indicates that the item should be removed at the next general cache wipe.
 */
define('CACHE_TEMPORARY', -1);

/**
 * Indicates that page caching is disabled.
 */
define('CACHE_DISABLED', 0);

/**
 * Indicates that page caching is enabled, using "normal" mode.
 */
define('CACHE_NORMAL', 1);

/**
 * Indicates that page caching is using "aggressive" mode. This bypasses
 * loading any modules for additional speed, which may break functionality in
 * modules that expect to be run on each page load.
 */
define('CACHE_AGGRESSIVE', 2);

/**
 * Log message severity -- Emergency: system is unusable.
 *
 * The WATCHDOG_* constant definitions correspond to the logging severity levels
 * defined in RFC 3164, section 4.1.1: http://www.faqs.org/rfcs/rfc3164.html
 *
 * @see watchdog()
 * @see watchdog_severity_levels()
 */
define('WATCHDOG_EMERG', 0);

/**
 * Log message severity -- Alert: action must be taken immediately.
 *
 * The WATCHDOG_* constant definitions correspond to the logging severity levels
 * defined in RFC 3164, section 4.1.1: http://www.faqs.org/rfcs/rfc3164.html
 *
 * @see watchdog()
 * @see watchdog_severity_levels()
 */
define('WATCHDOG_ALERT', 1);

/**
 * Log message severity -- Critical: critical conditions.
 *
 * The WATCHDOG_* constant definitions correspond to the logging severity levels
 * defined in RFC 3164, section 4.1.1: http://www.faqs.org/rfcs/rfc3164.html
 *
 * @see watchdog()
 * @see watchdog_severity_levels()
 */
define('WATCHDOG_CRITICAL', 2);

/**
 * Log message severity -- Error: error conditions.
 *
 * The WATCHDOG_* constant definitions correspond to the logging severity levels
 * defined in RFC 3164, section 4.1.1: http://www.faqs.org/rfcs/rfc3164.html
 *
 * @see watchdog()
 * @see watchdog_severity_levels()
 */
define('WATCHDOG_ERROR', 3);

/**
 * Log message severity -- Warning: warning conditions.
 *
 * The WATCHDOG_* constant definitions correspond to the logging severity levels
 * defined in RFC 3164, section 4.1.1: http://www.faqs.org/rfcs/rfc3164.html
 *
 * @see watchdog()
 * @see watchdog_severity_levels()
 */
define('WATCHDOG_WARNING', 4);

/**
 * Log message severity -- Notice: normal but significant condition.
 *
 * The WATCHDOG_* constant definitions correspond to the logging severity levels
 * defined in RFC 3164, section 4.1.1: http://www.faqs.org/rfcs/rfc3164.html
 *
 * @see watchdog()
 * @see watchdog_severity_levels()
 */
define('WATCHDOG_NOTICE', 5);

/**
 * Log message severity -- Informational: informational messages.
 *
 * The WATCHDOG_* constant definitions correspond to the logging severity levels
 * defined in RFC 3164, section 4.1.1: http://www.faqs.org/rfcs/rfc3164.html
 *
 * @see watchdog()
 * @see watchdog_severity_levels()
 */
define('WATCHDOG_INFO', 6);

/**
 * Log message severity -- Debug: debug-level messages.
 *
 * The WATCHDOG_* constant definitions correspond to the logging severity levels
 * defined in RFC 3164, section 4.1.1: http://www.faqs.org/rfcs/rfc3164.html
 *
 * @see watchdog()
 * @see watchdog_severity_levels()
 */
define('WATCHDOG_DEBUG', 7);

/**
 * First bootstrap phase: initialize configuration.
 */
define('DRUPAL_BOOTSTRAP_CONFIGURATION', 0);

/**
 * Second bootstrap phase: try to call a non-database cache
 * fetch routine.
 */
define('DRUPAL_BOOTSTRAP_EARLY_PAGE_CACHE', 1);

/**
 * Third bootstrap phase: initialize database layer.
 */
define('DRUPAL_BOOTSTRAP_DATABASE', 2);

/**
 * Fourth bootstrap phase: identify and reject banned hosts.
 */
define('DRUPAL_BOOTSTRAP_ACCESS', 3);

/**
 * Fifth bootstrap phase: initialize session handling.
 */
define('DRUPAL_BOOTSTRAP_SESSION', 4);

/**
 * Sixth bootstrap phase: load bootstrap.inc and module.inc, start
 * the variable system and try to serve a page from the cache.
 */
define('DRUPAL_BOOTSTRAP_LATE_PAGE_CACHE', 5);

/**
 * Seventh bootstrap phase: find out language of the page.
 */
define('DRUPAL_BOOTSTRAP_LANGUAGE', 6);

/**
 * Eighth bootstrap phase: set $_GET['q'] to Drupal path of request.
 */
define('DRUPAL_BOOTSTRAP_PATH', 7);

/**
 * Final bootstrap phase: Drupal is fully loaded; validate and fix
 * input data.
 */
define('DRUPAL_BOOTSTRAP_FULL', 8);

/**
 * Role ID for anonymous users; should match what's in the "role" table.
 */
define('DRUPAL_ANONYMOUS_RID', 1);

/**
 * Role ID for authenticated users; should match what's in the "role" table.
 */
define('DRUPAL_AUTHENTICATED_RID', 2);

/**
 * The number of bytes in a kilobyte.
 *
 * For more information, visit http://en.wikipedia.org/wiki/Kilobyte.
 */
define('DRUPAL_KILOBYTE', 1024);

/**
 * The language code used when no language is explicitly assigned.
 *
 * Defined by ISO639-2 for "Undetermined".
 */
define('LANGUAGE_NONE', 'und');

/**
 * The type of language used to define the content language.
 */
define('LANGUAGE_TYPE_CONTENT', 'language_content');

/**
 * The type of language used to select the user interface.
 */
define('LANGUAGE_TYPE_INTERFACE', 'language');

/**
 * The type of language used for URLs.
 */
define('LANGUAGE_TYPE_URL', 'language_url');

/**
 * Language written left to right. Possible value of $language->direction.
 */
define('LANGUAGE_LTR', 0);

/**
 * Language written right to left. Possible value of $language->direction.
 */
define('LANGUAGE_RTL', 1);

/**
 * No language negotiation. The default language is used.
 */
define('LANGUAGE_NEGOTIATION_NONE', 0);

/**
 * Path based negotiation with fallback to default language
 * if no defined path prefix identified.
 */
define('LANGUAGE_NEGOTIATION_PATH_DEFAULT', 1);

/**
 * Path based negotiation with fallback to user preferences
 * and browser language detection if no defined path prefix
 * identified.
 */
define('LANGUAGE_NEGOTIATION_PATH', 2);

/**
 * Domain based negotiation with fallback to default language
 * if no language identified by domain.
 */
define('LANGUAGE_NEGOTIATION_DOMAIN', 3);

// Hide E_DEPRECATED messages.
if (defined('E_DEPRECATED')) {
  error_reporting(error_reporting() & ~E_DEPRECATED);
}

/**
 * Time of the current request in seconds elapsed since the Unix Epoch. Backport from D7
 *
 * This differs from $_SERVER['REQUEST_TIME'], which is stored as a float
 * since PHP 5.4.0. Float timestamps confuse most PHP functions
 * (including date_create()).
 *
 * @see http://php.net/manual/reserved.variables.server.php
 * @see http://php.net/manual/function.time.php
 */
define('REQUEST_TIME', (int) $_SERVER['REQUEST_TIME']);

/**
 * Regular expression to match PHP function names.
 *
 * @see http://php.net/manual/language.functions.php
 */
define('DRUPAL_PHP_FUNCTION_PATTERN', '[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*');

/**
 * A RFC7231 Compliant date. Backport from D7
 *
 * http://tools.ietf.org/html/rfc7231#section-7.1.1.1
 *
 * Example: Sun, 06 Nov 1994 08:49:37 GMT
 */
define('DATE_RFC7231', 'D, d M Y H:i:s \G\M\T');

/**
 * Start the timer with the specified name. If you start and stop
 * the same timer multiple times, the measured intervals will be
 * accumulated.
 *
 * @param name
 *   The name of the timer.
 */
function timer_start($name) {
  global $timers;

  $timers[$name]['start'] = microtime(TRUE);
  $timers[$name]['count'] = isset($timers[$name]['count']) ? ++$timers[$name]['count'] : 1;
}

/**
 * Read the current timer value without stopping the timer.
 *
 * @param name
 *   The name of the timer.
 * @return
 *   The current timer value in ms.
 */
function timer_read($name) {
  global $timers;

  if (isset($timers[$name]['start'])) {
    $stop = microtime(TRUE);
    $diff = round(($stop - $timers[$name]['start']) * 1000, 2);

    if (isset($timers[$name]['time'])) {
      $diff += $timers[$name]['time'];
    }
    return $diff;
  }
  return $timers[$name]['time'];
}

/**
 * Stop the timer with the specified name.
 *
 * @param name
 *   The name of the timer.
 * @return
 *   A timer array. The array contains the number of times the
 *   timer has been started and stopped (count) and the accumulated
 *   timer value in ms (time).
 */
function timer_stop($name) {
  global $timers;

  $timers[$name]['time'] = timer_read($name);
  unset($timers[$name]['start']);

  return $timers[$name];
}

/**
 * Find the appropriate configuration directory.
 *
 * Try finding a matching configuration directory by stripping the website's
 * hostname from left to right and pathname from right to left. The first
 * configuration file found will be used; the remaining will ignored. If no
 * configuration file is found, return a default value '$confdir/default'.
 *
 * Example for a fictitious site installed at
 * http://www.drupal.org:8080/mysite/test/ the 'settings.php' is searched in
 * the following directories:
 *
 *  1. $confdir/8080.www.drupal.org.mysite.test
 *  2. $confdir/www.drupal.org.mysite.test
 *  3. $confdir/drupal.org.mysite.test
 *  4. $confdir/org.mysite.test
 *
 *  5. $confdir/8080.www.drupal.org.mysite
 *  6. $confdir/www.drupal.org.mysite
 *  7. $confdir/drupal.org.mysite
 *  8. $confdir/org.mysite
 *
 *  9. $confdir/8080.www.drupal.org
 * 10. $confdir/www.drupal.org
 * 11. $confdir/drupal.org
 * 12. $confdir/org
 *
 * 13. $confdir/default
 *
 * @param $require_settings
 *   Only configuration directories with an existing settings.php file
 *   will be recognized. Defaults to TRUE. During initial installation,
 *   this is set to FALSE so that Drupal can detect a matching directory,
 *   then create a new settings.php file in it.
 * @param reset
 *   Force a full search for matching directories even if one had been
 *   found previously.
 * @return
 *   The path of the matching directory.
 */
function conf_path($require_settings = TRUE, $reset = FALSE) {
  static $conf = '';

  if ($conf && !$reset) {
    return $conf;
  }

  $confdir = 'sites';
  $uri = explode('/', $_SERVER['SCRIPT_NAME'] ? $_SERVER['SCRIPT_NAME'] : $_SERVER['SCRIPT_FILENAME']);
  $server = explode('.', implode('.', array_reverse(explode(':', rtrim($_SERVER['HTTP_HOST'], '.')))));
  for ($i = count($uri) - 1; $i > 0; $i--) {
    for ($j = count($server); $j > 0; $j--) {
      $dir = implode('.', array_slice($server, -$j)) . implode('.', array_slice($uri, 0, $i));
      if (file_exists("$confdir/$dir/settings.php") || (!$require_settings && file_exists("$confdir/$dir"))) {
        $conf = "$confdir/$dir";
        return $conf;
      }
    }
  }
  $conf = "$confdir/default";
  return $conf;
}

/**
 * Sets appropriate server variables needed for command line scripts to work. 
 * Backport from D7
 *
 * This function can be called by command line scripts before bootstrapping
 * Drupal, to ensure that the page loads with the desired server parameters.
 * This is because many parts of Drupal assume that they are running in a web
 * browser and therefore use information from the global PHP $_SERVER variable
 * that does not get set when Drupal is run from the command line.
 *
 * In many cases, the default way in which this function populates the $_SERVER
 * variable is sufficient, and it can therefore be called without passing in
 * any input. However, command line scripts running on a multisite installation
 * (or on any installation that has settings.php stored somewhere other than
 * the sites/default folder) need to pass in the URL of the site to allow
 * Drupal to detect the correct location of the settings.php file. Passing in
 * the 'url' parameter is also required for functions like request_uri() to
 * return the expected values.
 *
 * Most other parameters do not need to be passed in, but may be necessary in
 * some cases; for example, if Drupal's ip_address() function needs to return
 * anything but the standard localhost value ('127.0.0.1'), the command line
 * script should pass in the desired value via the 'REMOTE_ADDR' key.
 *
 * @param $variables
 *   (optional) An associative array of variables within $_SERVER that should
 *   be replaced. If the special element 'url' is provided in this array, it
 *   will be used to populate some of the server defaults; it should be set to
 *   the URL of the current page request, excluding any $_GET request but
 *   including the script name (e.g., http://www.example.com/mysite/index.php).
 *
 * @see conf_path()
 * @see request_uri()
 * @see ip_address()
 */
function drupal_override_server_variables($variables = array()) {
  // Allow the provided URL to override any existing values in $_SERVER.
  if (isset($variables['url'])) {
    $url = parse_url($variables['url']);
    if (isset($url['host'])) {
      $_SERVER['HTTP_HOST'] = $url['host'];
    }
    if (isset($url['path'])) {
      $_SERVER['SCRIPT_NAME'] = $url['path'];
    }
    unset($variables['url']);
  }
  // Define default values for $_SERVER keys. These will be used if $_SERVER
  // does not already define them and no other values are passed in to this
  // function.
  $defaults = array(
    'HTTP_HOST' => 'localhost',
    'SCRIPT_NAME' => NULL,
    'REMOTE_ADDR' => '127.0.0.1',
    'REQUEST_METHOD' => 'GET',
    'SERVER_NAME' => NULL,
    'SERVER_SOFTWARE' => NULL,
    'HTTP_USER_AGENT' => NULL,
  );
  // Replace elements of the $_SERVER array, as appropriate.
  $_SERVER = $variables + $_SERVER + $defaults;
}

/**
 * Unsets all disallowed global variables. See $allowed for what's allowed.
 */
function drupal_unset_globals() {
  if (ini_get('register_globals')) {
    $allowed = array('_ENV' => 1, '_GET' => 1, '_POST' => 1, '_COOKIE' => 1, '_FILES' => 1, '_SERVER' => 1, '_REQUEST' => 1, 'GLOBALS' => 1);
    foreach ($GLOBALS as $key => $value) {
      if (!isset($allowed[$key])) {
        unset($GLOBALS[$key]);
      }
    }
  }
}

/**
 * Validate that a hostname (for example $_SERVER['HTTP_HOST']) is safe.
 *
 * As $_SERVER['HTTP_HOST'] is user input, ensure it only contains characters
 * allowed in hostnames.  See RFC 952 (and RFC 2181). $_SERVER['HTTP_HOST'] is
 * lowercased.
 *
 * @return
 *  TRUE if only containing valid characters, or FALSE otherwise.
 */
function drupal_valid_http_host($host) {
  // Limit the length of the host name to 1000 bytes to prevent DoS attacks with
  // long host names.
  return strlen($host) <= 1000
    // Limit the number of subdomains and port separators to prevent DoS attacks
    // in conf_path().
    && substr_count($host, '.') <= 100
    && substr_count($host, ':') <= 100
    && preg_match('/^\[?(?:[a-zA-Z0-9-:\]_]+\.?)+$/', $host);
}

/**
 * Loads the configuration and sets the base URL, cookie domain, and
 * session name correctly.
 */
function conf_init() {
  global $base_url, $base_path, $base_root;

  // Export the following settings.php variables to the global namespace
  global $db_url, $db_prefix, $db_collation, $cookie_domain, $conf, $installed_profile, $update_free_access;
  $conf = array();

  if (!isset($_SERVER['SERVER_PROTOCOL']) || ($_SERVER['SERVER_PROTOCOL'] != 'HTTP/1.0' && $_SERVER['SERVER_PROTOCOL'] != 'HTTP/1.1')) {
    $_SERVER['SERVER_PROTOCOL'] = 'HTTP/1.0';
  }

  if (isset($_SERVER['HTTP_HOST'])) {
    // As HTTP_HOST is user input, ensure it only contains characters allowed
    // in hostnames. See RFC 952 (and RFC 2181).
    // $_SERVER['HTTP_HOST'] is lowercased here per specifications.
    $_SERVER['HTTP_HOST'] = strtolower($_SERVER['HTTP_HOST']);
    if (!drupal_valid_http_host($_SERVER['HTTP_HOST'])) {
      // HTTP_HOST is invalid, e.g. if containing slashes it may be an attack.
      header($_SERVER['SERVER_PROTOCOL'] .' 400 Bad Request');
      exit;
    }
  }
  else {
    // Some pre-HTTP/1.1 clients will not send a Host header. Ensure the key is
    // defined for E_ALL compliance.
    $_SERVER['HTTP_HOST'] = '';
  }

  if (file_exists('./'. conf_path() .'/settings.php')) {
    include_once './'. conf_path() .'/settings.php';
  }

  // Ignore the placeholder URL from default.settings.php.
  if (isset($db_url) && $db_url == 'mysql://username:password@localhost/databasename') {
    $db_url = '';
  }

  if (isset($base_url)) {
    // Parse fixed base URL from settings.php.
    $parts = parse_url($base_url);
    if (!isset($parts['path'])) {
      $parts['path'] = '';
    }
    $base_path = $parts['path'] .'/';
    // Build $base_root (everything until first slash after "scheme://").
    $base_root = substr($base_url, 0, strlen($base_url) - strlen($parts['path']));
  }
  else {
    // Create base URL
    $base_root = (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] == 'on') ? 'https' : 'http';

    $base_url = $base_root .= '://'. $_SERVER['HTTP_HOST'];

    // $_SERVER['SCRIPT_NAME'] can, in contrast to $_SERVER['PHP_SELF'], not
    // be modified by a visitor.
    if ($dir = trim(dirname($_SERVER['SCRIPT_NAME']), '\,/')) {
      $base_path = "/$dir";
      $base_url .= $base_path;
      $base_path .= '/';
    }
    else {
      $base_path = '/';
    }
  }

  if ($cookie_domain) {
    // If the user specifies the cookie domain, also use it for session name.
    $session_name = $cookie_domain;
  }
  else {
    // Otherwise use $base_url as session name, without the protocol
    // to use the same session identifiers across HTTP and HTTPS.
    list( , $session_name) = explode('://', $base_url, 2);
    // We escape the hostname because it can be modified by a visitor.
    if (!empty($_SERVER['HTTP_HOST'])) {
      $cookie_domain = check_plain($_SERVER['HTTP_HOST']);
      // Strip leading periods, www., and port numbers from cookie domain.
      $cookie_domain = ltrim($cookie_domain, '.');
      if (strpos($cookie_domain, 'www.') === 0) {
        $cookie_domain = substr($cookie_domain, 4);
      }
      $cookie_domain = explode(':', $cookie_domain);
      $cookie_domain = '.'. $cookie_domain[0];
    }
  }
  // To prevent session cookies from being hijacked, a user can configure the
  // SSL version of their website to only transfer session cookies via SSL by
  // using PHP's session.cookie_secure setting. The browser will then use two
  // separate session cookies for the HTTPS and HTTP versions of the site. So we
  // must use different session identifiers for HTTPS and HTTP to prevent a
  // cookie collision.
  if (ini_get('session.cookie_secure')) {
    $session_name .= 'SSL';
  }
  // Per RFC 2109, cookie domains must contain at least one dot other than the
  // first. For hosts such as 'localhost' or IP Addresses we don't set a cookie domain.
  if (count(explode('.', $cookie_domain)) > 2 && !is_numeric(str_replace('.', '', $cookie_domain))) {
    ini_set('session.cookie_domain', $cookie_domain);
  }
  session_name('SESS'. md5($session_name));
}

/**
 * Returns and optionally sets the filename for a system item (module,
 * theme, etc.). The filename, whether provided, cached, or retrieved
 * from the database, is only returned if the file exists.
 *
 * This function plays a key role in allowing Drupal's resources (modules
 * and themes) to be located in different places depending on a site's
 * configuration. For example, a module 'foo' may legally be be located
 * in any of these three places:
 *
 * modules/foo/foo.module
 * sites/all/modules/foo/foo.module
 * sites/example.com/modules/foo/foo.module
 *
 * Calling drupal_get_filename('module', 'foo') will give you one of
 * the above, depending on where the module is located.
 *
 * @param $type
 *   The type of the item (i.e. theme, theme_engine, module, profile).
 * @param $name
 *   The name of the item for which the filename is requested.
 * @param $filename
 *   The filename of the item if it is to be set explicitly rather
 *   than by consulting the database.
 *
 * @return
 *   The filename of the requested item.
 */
function drupal_get_filename($type, $name, $filename = NULL) {
  static $files = array();

  if (!isset($files[$type])) {
    $files[$type] = array();
  }

  if (!empty($filename) && file_exists($filename)) {
    $files[$type][$name] = $filename;
  }
  elseif (isset($files[$type][$name])) {
    // nothing
  }
  // Verify that we have an active database connection, before querying
  // the database.  This is required because this function is called both
  // before we have a database connection (i.e. during installation) and
  // when a database connection fails.
  elseif (db_is_active() && (($file = db_result(db_query("SELECT filename FROM {system} WHERE name = '%s' AND type = '%s'", $name, $type))) && file_exists($file))) {
    $files[$type][$name] = $file;
  }
  else {
    // Fallback to searching the filesystem if the database connection is
    // not established or the requested file is not found.
    $config = conf_path();
    if($type == 'theme_engine') {
      $dir = 'themes/engines';
      $file = "$name.engine";
    } else if($type == 'theme') {
      $dir = "${type}s";
      $file = "$name.info";
    } else {
      $dir = "${type}s";
      $file = "$name.$type";
    }

    foreach (array("$config/$dir/$file", "$config/$dir/$name/$file", "$dir/$file", "$dir/$name/$file") as $file) {
      if (file_exists($file)) {
        $files[$type][$name] = $file;
        break;
      }
    }
  }

  if (isset($files[$type][$name])) {
    return $files[$type][$name];
  }
}

/**
 * Load the persistent variable table.
 *
 * The variable table is composed of values that have been saved in the table
 * with variable_set() as well as those explicitly specified in the configuration
 * file.
 */
function variable_init($conf = array()) {
  // NOTE: caching the variables improves performance by 20% when serving cached pages.
  if ($cached = cache_get('variables', 'cache')) {
    $variables = $cached->data;
  }
  else {
    $result = db_query('SELECT * FROM {variable}');
    while ($variable = db_fetch_object($result)) {
      $variables[$variable->name] = unserialize($variable->value);
    }
    cache_set('variables', $variables);
  }

  foreach ($conf as $name => $value) {
    $variables[$name] = $value;
  }

  return $variables;
}

/**
 * Returns a persistent variable.
 *
 * Case-sensitivity of the variable_* functions depends on the database
 * collation used. To avoid problems, always use lower case for persistent
 * variable names.
 *
 * @param $name
 *   The name of the variable to return.
 * @param $default
 *   The default value to use if this variable has never been set.
 * @return
 *   The value of the variable.
 *
 * @see variable_del(), variable_set()
 */
function variable_get($name, $default = NULL) {
  global $conf;

  return isset($conf[$name]) ? $conf[$name] : $default;
}

/**
 * Sets a persistent variable.
 *
 * Case-sensitivity of the variable_* functions depends on the database
 * collation used. To avoid problems, always use lower case for persistent
 * variable names.
 *
 * @param $name
 *   The name of the variable to set.
 * @param $value
 *   The value to set. This can be any PHP data type; these functions take care
 *   of serialization as necessary.
 *
 * @see variable_del(), variable_get()
 */
function variable_set($name, $value) {
  global $conf;

  $serialized_value = serialize($value);
  db_query("UPDATE {variable} SET value = '%s' WHERE name = '%s'", $serialized_value, $name);
  if (!db_affected_rows()) {
    @db_query("INSERT INTO {variable} (name, value) VALUES ('%s', '%s')", $name, $serialized_value);
  }

  cache_clear_all('variables', 'cache');

  $conf[$name] = $value;
}

/**
 * Unsets a persistent variable.
 *
 * Case-sensitivity of the variable_* functions depends on the database
 * collation used. To avoid problems, always use lower case for persistent
 * variable names.
 *
 * @param $name
 *   The name of the variable to undefine.
 *
 * @see variable_get(), variable_set()
 */
function variable_del($name) {
  global $conf;

  db_query("DELETE FROM {variable} WHERE name = '%s'", $name);
  cache_clear_all('variables', 'cache');

  unset($conf[$name]);
}


/**
 * Retrieves the current page from the cache. Backport from D7
 *
 * Note: we do not serve cached pages to authenticated users, or to anonymous
 * users when $_SESSION is non-empty. $_SESSION may contain status messages
 * from a form submission, the contents of a shopping cart, or other user-
 * specific content that should not be cached and displayed to other users.
 *
 * @param $check_only
 *   (optional) Set to TRUE to only return whether a previous call found a
 *   cache entry.
 *
 * @return
 *   The cache object, if the page was found in the cache, NULL otherwise.
 */
function drupal_page_get_cache($check_only = FALSE) {
  global $base_root;
  static $cache_hit = FALSE;

  if ($check_only) {
    return $cache_hit;
  }

  if (drupal_page_is_cacheable()) {
    $cache = cache_get($base_root . request_uri(), 'cache_page');
    if ($cache !== FALSE) {
      $cache_hit = TRUE;
    }
    return $cache;
  }
}

/**
 * Determines the cacheability of the current page. Backport from D7
 *
 * @param $allow_caching
 *   Set to FALSE if you want to prevent this page to get cached.
 *
 * @return
 *   TRUE if the current page can be cached, FALSE otherwise.
 */
function drupal_page_is_cacheable($allow_caching = NULL) {
  $allow_caching_static = &drupal_static(__FUNCTION__, TRUE);
  if (isset($allow_caching)) {
    $allow_caching_static = $allow_caching;
  }

  return $allow_caching_static && ($_SERVER['REQUEST_METHOD'] == 'GET' || $_SERVER['REQUEST_METHOD'] == 'HEAD')
    && !drupal_is_cli();
}

/**
 * Call all init or exit hooks without including all modules.
 *
 * @param $hook
 *   The name of the bootstrap hook we wish to invoke.
 */
function bootstrap_invoke_all($hook) {
  foreach (module_list(TRUE, TRUE) as $module) {
    drupal_load('module', $module);
    module_invoke($module, $hook);
  }
}

/**
 * Includes a file with the provided type and name. This prevents
 * including a theme, engine, module, etc., more than once.
 *
 * @param $type
 *   The type of item to load (i.e. theme, theme_engine, module, profile).
 * @param $name
 *   The name of the item to load.
 *
 * @return
 *   TRUE if the item is loaded or has already been loaded.
 */
function drupal_load($type, $name) {
  static $files = array();

  if (isset($files[$type][$name])) {
    return TRUE;
  }

  $filename = drupal_get_filename($type, $name);

  if ($filename) {
    include_once "./$filename";
    $files[$type][$name] = TRUE;

    return TRUE;
  }

  return FALSE;
}

/**
 * Sets an HTTP response header for the current page. Backport from D7
 *
 * Note: When sending a Content-Type header, always include a 'charset' type,
 * too. This is necessary to avoid security bugs (e.g. UTF-7 XSS).
 *
 * @param $name
 *   The HTTP header name, or the special 'Status' header name.
 * @param $value
 *   The HTTP header value; if equal to FALSE, the specified header is unset.
 *   If $name is 'Status', this is expected to be a status code followed by a
 *   reason phrase, e.g. "404 Not Found".
 * @param $append
 *   Whether to append the value to an existing header or to replace it.
 */
function drupal_add_http_header($name, $value, $append = FALSE) {
  // The headers as name/value pairs.
  $headers = &drupal_static('drupal_http_headers', array());

  $name_lower = strtolower($name);
  _drupal_set_preferred_header_name($name);

  if ($value === FALSE) {
    $headers[$name_lower] = FALSE;
  }
  elseif (isset($headers[$name_lower]) && $append) {
    // Multiple headers with identical names may be combined using comma (RFC
    // 2616, section 4.2).
    $headers[$name_lower] .= ',' . $value;
  }
  else {
    $headers[$name_lower] = $value;
  }
  drupal_send_headers(array($name => $headers[$name_lower]), TRUE);
}

/**
 * Gets the HTTP response headers for the current page. Backport from D7
 *
 * @param $name
 *   An HTTP header name. If omitted, all headers are returned as name/value
 *   pairs. If an array value is FALSE, the header has been unset.
 *
 * @return
 *   A string containing the header value, or FALSE if the header has been set,
 *   or NULL if the header has not been set.
 */
function drupal_get_http_header($name = NULL) {
  $headers = &drupal_static('drupal_http_headers', array());
  if (isset($name)) {
    $name = strtolower($name);
    return isset($headers[$name]) ? $headers[$name] : NULL;
  }
  else {
    return $headers;
  }
}

/**
 * Sets the preferred name for the HTTP header. Backport from D7
 *
 * Header names are case-insensitive, but for maximum compatibility they should
 * follow "common form" (see RFC 2617, section 4.2).
 */
function _drupal_set_preferred_header_name($name = NULL) {
  static $header_names = array();

  if (!isset($name)) {
    return $header_names;
  }
  $header_names[strtolower($name)] = $name;
}

/**
 * Sends the HTTP response headers that were previously set, adding defaults. Backport from D7
 *
 * Headers are set in drupal_add_http_header(). Default headers are not set
 * if they have been replaced or unset using drupal_add_http_header().
 *
 * @param array $default_headers
 *   (optional) An array of headers as name/value pairs.
 * @param bool $only_default
 *   (optional) If TRUE and headers have already been sent, send only the
 *   specified headers.
 */
function drupal_send_headers($default_headers = array(), $only_default = FALSE) {
  $headers_sent = &drupal_static(__FUNCTION__, FALSE);
  $headers = drupal_get_http_header();
  if ($only_default && $headers_sent) {
    $headers = array();
  }
  $headers_sent = TRUE;

  $header_names = _drupal_set_preferred_header_name();
  foreach ($default_headers as $name => $value) {
    $name_lower = strtolower($name);
    if (!isset($headers[$name_lower])) {
      $headers[$name_lower] = $value;
      $header_names[$name_lower] = $name;
    }
  }
  foreach ($headers as $name_lower => $value) {
    if ($name_lower == 'status') {
      header($_SERVER['SERVER_PROTOCOL'] . ' ' . $value);
    }
    // Skip headers that have been unset.
    elseif ($value !== FALSE) {
      header($header_names[$name_lower] . ': ' . $value);
    }
  }
}

/**
 * Sets HTTP headers in preparation for a page response. Backport from D7
 *
 * Authenticated users are always given a 'no-cache' header, and will fetch a
 * fresh page on every request. This prevents authenticated users from seeing
 * locally cached pages.
 *
 * ETag and Last-Modified headers are not set per default for authenticated
 * users so that browsers do not send If-Modified-Since headers from
 * authenticated user pages. drupal_serve_page_from_cache() will set appropriate
 * ETag and Last-Modified headers for cached pages.
 *
 * @see drupal_page_set_cache()
 */
function drupal_page_header() {
  $headers_sent = &drupal_static(__FUNCTION__, FALSE);
  if ($headers_sent) {
    return TRUE;
  }
  $headers_sent = TRUE;

  $default_headers = array(
    'Expires' => 'Sun, 19 Nov 1978 05:00:00 GMT',
    'Cache-Control' => 'no-cache, must-revalidate, post-check=0, pre-check=0',
  );
  drupal_send_headers($default_headers);
}

/**
 * Sets HTTP headers in preparation for a cached page response.
 *
 * The headers allow as much as possible in proxies and browsers without any
 * particular knowledge about the pages. Modules can override these headers
 * using drupal_add_http_header().
 *
 * If the request is conditional (using If-Modified-Since and If-None-Match),
 * and the conditions match those currently in the cache, a 304 Not Modified
 * response is sent.
 */
function drupal_serve_page_from_cache(stdClass $cache) {
  // Negotiate whether to use compression.
  $page_compression = !empty($cache->data['page_compressed']);
  $return_compressed = $page_compression && isset($_SERVER['HTTP_ACCEPT_ENCODING']) && strpos($_SERVER['HTTP_ACCEPT_ENCODING'], 'gzip') !== FALSE;

  // Get headers set in hook_boot(). Keys are lower-case.
  $hook_boot_headers = drupal_get_http_header();

  // Headers generated in this function, that may be replaced or unset using
  // drupal_add_http_headers(). Keys are mixed-case.
  $default_headers = array();

  foreach ($cache->data['headers'] as $name => $value) {
    // In the case of a 304 response, certain headers must be sent, and the
    // remaining may not (see RFC 2616, section 10.3.5). Do not override
    // headers set in hook_boot().
    $name_lower = strtolower($name);
    if (in_array($name_lower, array('content-location', 'expires', 'cache-control', 'vary')) && !isset($hook_boot_headers[$name_lower])) {
      drupal_add_http_header($name, $value);
      unset($cache->data['headers'][$name]);
    }
  }

  // If the client sent a session cookie, a cached copy will only be served
  // to that one particular client due to Vary: Cookie. Thus, do not set
  // max-age > 0, allowing the page to be cached by external proxies, when a
  // session cookie is present unless the Vary header has been replaced or
  // unset in hook_boot().
  $max_age = !isset($_COOKIE[session_name()]) || isset($hook_boot_headers['vary']) ? variable_get('page_cache_maximum_age', 0) : 0;
  $default_headers['Cache-Control'] = 'public, max-age=' . $max_age;

  // Entity tag should change if the output changes.
  $etag = '"' . $cache->created . '-' . intval($return_compressed) . '"';
  header('Etag: ' . $etag);

  // See if the client has provided the required HTTP headers.
  $if_modified_since = isset($_SERVER['HTTP_IF_MODIFIED_SINCE']) ? strtotime($_SERVER['HTTP_IF_MODIFIED_SINCE']) : FALSE;
  $if_none_match = isset($_SERVER['HTTP_IF_NONE_MATCH']) ? stripslashes($_SERVER['HTTP_IF_NONE_MATCH']) : FALSE;

  if ($if_modified_since && $if_none_match
      && $if_none_match == $etag // etag must match
      && $if_modified_since == $cache->created) {  // if-modified-since must match
    header($_SERVER['SERVER_PROTOCOL'] . ' 304 Not Modified');
    drupal_send_headers($default_headers);
    return;
  }

  // Send the remaining headers.
  foreach ($cache->data['headers'] as $name => $value) {
    drupal_add_http_header($name, $value);
  }

  $default_headers['Last-Modified'] = gmdate(DATE_RFC7231, $cache->created);

  // HTTP/1.0 proxies does not support the Vary header, so prevent any caching
  // by sending an Expires date in the past. HTTP/1.1 clients ignores the
  // Expires header if a Cache-Control: max-age= directive is specified (see RFC
  // 2616, section 14.9.3).
  $default_headers['Expires'] = 'Sun, 19 Nov 1978 05:00:00 GMT';

  drupal_send_headers($default_headers);

  // Allow HTTP proxies to cache pages for anonymous users without a session
  // cookie. The Vary header is used to indicates the set of request-header
  // fields that fully determines whether a cache is permitted to use the
  // response to reply to a subsequent request for a given URL without
  // revalidation. If a Vary header has been set in hook_boot(), it is assumed
  // that the module knows how to cache the page.
  if (!isset($hook_boot_headers['vary']) && !variable_get('omit_vary_cookie')) {
    header('Vary: Cookie');
  }

  if ($page_compression) {
    header('Vary: Accept-Encoding', FALSE);
    // If page_compression is enabled, the cache contains gzipped data.
    if ($return_compressed) {
      // $cache->data['body'] is already gzip'ed, so make sure
      // zlib.output_compression does not compress it once more.
      ini_set('zlib.output_compression', '0');
      header('Content-Encoding: gzip');
    }
    else {
      // The client does not support compression, so unzip the data in the
      // cache. Strip the gzip header and run uncompress.
      $cache->data['body'] = gzinflate(substr(substr($cache->data['body'], 10), 0, -8));
    }
  }

  // Print the page.
  print $cache->data['body'];
}

/**
 * Define the critical hooks that force modules to always be loaded.
 */
function bootstrap_hooks() {
  return array('boot', 'exit');
}

/**
 * Unserializes and appends elements from a serialized string.
 *
 * @param $obj
 *   The object to which the elements are appended.
 * @param $field
 *   The attribute of $obj whose value should be unserialized.
 */
function drupal_unpack($obj, $field = 'data') {
  if ($obj->$field && $data = unserialize($obj->$field)) {
    foreach ($data as $key => $value) {
      if (!empty($key) && !isset($obj->$key)) {
        $obj->$key = $value;
      }
    }
  }
  return $obj;
}

/**
 * Formats a string for HTML display by replacing variable placeholders. Backport from D7
 *
 * This function replaces variable placeholders in a string with the requested
 * values and escapes the values so they can be safely displayed as HTML. It
 * should be used on any unknown text that is intended to be printed to an HTML
 * page (especially text that may have come from untrusted users, since in that
 * case it prevents cross-site scripting and other security problems).
 *
 * In most cases, you should use t() rather than calling this function
 * directly, since it will translate the text (on non-English-only sites) in
 * addition to formatting it.
 *
 * @param $string
 *   A string containing placeholders.
 * @param $args
 *   An associative array of replacements to make. Occurrences in $string of
 *   any key in $args are replaced with the corresponding value, after optional
 *   sanitization and formatting. The type of sanitization and formatting
 *   depends on the first character of the key:
 *   - @variable: Escaped to HTML using check_plain(). Use this as the default
 *     choice for anything displayed on a page on the site.
 *   - %variable: Escaped to HTML and formatted using drupal_placeholder(),
 *     which makes it display as <em>emphasized</em> text.
 *   - !variable: Inserted as is, with no sanitization or formatting. Only use
 *     this for text that has already been prepared for HTML display (for
 *     example, user-supplied text that has already been run through
 *     check_plain() previously, or is expected to contain some limited HTML
 *     tags and has already been run through filter_xss() previously).
 *
 * @see t()
 * @ingroup sanitization
 */
function format_string($string, array $args = array()) {
  // Transform arguments before inserting them.
  foreach ($args as $key => $value) {
    switch ($key[0]) {
      case '@':
        // Escaped only.
        $args[$key] = check_plain($value);
        break;

      case '%':
      default:
        // Escaped and placeholder.
        $args[$key] = drupal_placeholder($value);
        break;

      case '!':
        // Pass-through.
    }
  }
  return strtr($string, $args);
}

/**
 * Return the URI of the referring page.
 */
function referer_uri() {
  if (isset($_SERVER['HTTP_REFERER'])) {
    return $_SERVER['HTTP_REFERER'];
  }
}

/**
 * Encode special characters in a plain-text string for display as HTML.
 *
 * Also validates strings as UTF-8 to prevent cross site scripting attacks on
 * Internet Explorer 6.
 *
 * @param $text
 *   The text to be checked or processed.
 * @return
 *   An HTML safe version of $text, or an empty string if $text is not
 *   valid UTF-8.
 *
 * @see drupal_validate_utf8().
 */
function check_plain($text) {
  static $php525;

  if (!isset($php525)) {
    $php525 = version_compare(PHP_VERSION, '5.2.5', '>=');
  }
  // We duplicate the preg_match() to validate strings as UTF-8 from
  // drupal_validate_utf8() here. This avoids the overhead of an additional
  // function call, since check_plain() may be called hundreds of times during
  // a request. For PHP 5.2.5+, this check for valid UTF-8 should be handled
  // internally by PHP in htmlspecialchars().
  // @see http://www.php.net/releases/5_2_5.php
  // @todo remove this when support for either IE6 or PHP < 5.2.5 is dropped.

  if ($php525) {
    return htmlspecialchars($text, ENT_QUOTES, 'UTF-8');
  }
  return (preg_match('/^./us', $text) == 1) ? htmlspecialchars($text, ENT_QUOTES, 'UTF-8') : '';
}

/**
 * Checks whether a string is valid UTF-8.
 *
 * All functions designed to filter input should use drupal_validate_utf8
 * to ensure they operate on valid UTF-8 strings to prevent bypass of the
 * filter.
 *
 * When text containing an invalid UTF-8 lead byte (0xC0 - 0xFF) is presented
 * as UTF-8 to Internet Explorer 6, the program may misinterpret subsequent
 * bytes. When these subsequent bytes are HTML control characters such as
 * quotes or angle brackets, parts of the text that were deemed safe by filters
 * end up in locations that are potentially unsafe; An onerror attribute that
 * is outside of a tag, and thus deemed safe by a filter, can be interpreted
 * by the browser as if it were inside the tag.
 *
 * This function exploits preg_match behaviour (since PHP 4.3.5) when used
 * with the u modifier, as a fast way to find invalid UTF-8. When the matched
 * string contains an invalid byte sequence, it will fail silently.
 *
 * preg_match may not fail on 4 and 5 octet sequences, even though they
 * are not supported by the specification.
 *
 * The specific preg_match behaviour is present since PHP 4.3.5.
 *
 * @param $text
 *   The text to check.
 * @return
 *   TRUE if the text is valid UTF-8, FALSE if not.
 */
function drupal_validate_utf8($text) {
  if (strlen($text) == 0) {
    return TRUE;
  }
  // For performance reasons this logic is duplicated in check_plain().
  return (preg_match('/^./us', $text) == 1);
}

/**
 * Since $_SERVER['REQUEST_URI'] is only available on Apache, we
 * generate an equivalent using other environment variables.
 */
function request_uri() {

  if (isset($_SERVER['REQUEST_URI'])) {
    $uri = $_SERVER['REQUEST_URI'];
  }
  else {
    if (isset($_SERVER['argv'])) {
      $uri = $_SERVER['SCRIPT_NAME'] .'?'. $_SERVER['argv'][0];
    }
    elseif (isset($_SERVER['QUERY_STRING'])) {
      $uri = $_SERVER['SCRIPT_NAME'] .'?'. $_SERVER['QUERY_STRING'];
    }
    else {
      $uri = $_SERVER['SCRIPT_NAME'];
    }
  }
  // Prevent multiple slashes to avoid cross site requests via the FAPI.
  $uri = '/'. ltrim($uri, '/');

  return $uri;
}

/**
 * Log a system message.
 *
 * @param $type
 *   The category to which this message belongs. Can be any string, but the
 *   general practice is to use the name of the module calling watchdog().
 *   The $type parameter is limited to 16 characters; anything longer is
 *   truncated.
 * @param $message
 *   The message to store in the log. See t() for documentation
 *   on how $message and $variables interact. Keep $message
 *   translatable by not concatenating dynamic values into it!
 * @param $variables
 *   Array of variables to replace in the message on display or
 *   NULL if message is already translated or not possible to
 *   translate.
 * @param $severity
 *   The severity of the message; one of the following values as defined in
 *   @link http://www.faqs.org/rfcs/rfc3164.html RFC 3164: @endlink
 *   - WATCHDOG_EMERGENCY: Emergency, system is unusable.
 *   - WATCHDOG_ALERT: Alert, action must be taken immediately.
 *   - WATCHDOG_CRITICAL: Critical conditions.
 *   - WATCHDOG_ERROR: Error conditions.
 *   - WATCHDOG_WARNING: Warning conditions.
 *   - WATCHDOG_NOTICE: (default) Normal but significant conditions.
 *   - WATCHDOG_INFO: Informational messages.
 *   - WATCHDOG_DEBUG: Debug-level messages.
 * @param $link
 *   A link to associate with the message.
 *
 * @see watchdog_severity_levels()
 */
function watchdog($type, $message, $variables = array(), $severity = WATCHDOG_NOTICE, $link = NULL) {
  global $user, $base_root;

  // Prepare the fields to be logged
  $log_message = array(
    'type'        => $type,
    'message'     => $message,
    'variables'   => $variables,
    'severity'    => $severity,
    'link'        => $link,
    'user'        => $user,
    'request_uri' => $base_root . request_uri(),
    'referer'     => referer_uri(),
    'ip'          => ip_address(),
    'timestamp'   => time(),
    );

  // Call the logging hooks to log/process the message
  foreach (module_implements('watchdog') as $module) {
    module_invoke($module, 'watchdog', $log_message);
  }
}

/**
 * Set a message which reflects the status of the performed operation.
 *
 * If the function is called with no arguments, this function returns all set
 * messages without clearing them.
 *
 * @param $message
 *   The message should begin with a capital letter and always ends with a
 *   period '.'.
 * @param $type
 *   The type of the message. One of the following values are possible:
 *   - 'status'
 *   - 'warning'
 *   - 'error'
 * @param $repeat
 *   If this is FALSE and the message is already set, then the message won't
 *   be repeated.
 */
function drupal_set_message($message = NULL, $type = 'status', $repeat = TRUE) {
  if ($message) {
    if (!isset($_SESSION['messages'])) {
      $_SESSION['messages'] = array();
    }

    if (!isset($_SESSION['messages'][$type])) {
      $_SESSION['messages'][$type] = array();
    }

    if ($repeat || !in_array($message, $_SESSION['messages'][$type])) {
      $_SESSION['messages'][$type][] = $message;
    }
  }

  // messages not set when DB connection fails
  return isset($_SESSION['messages']) ? $_SESSION['messages'] : NULL;
}

/**
 * Return all messages that have been set.
 *
 * @param $type
 *   (optional) Only return messages of this type.
 * @param $clear_queue
 *   (optional) Set to FALSE if you do not want to clear the messages queue
 * @return
 *   An associative array, the key is the message type, the value an array
 *   of messages. If the $type parameter is passed, you get only that type,
 *   or an empty array if there are no such messages. If $type is not passed,
 *   all message types are returned, or an empty array if none exist.
 */
function drupal_get_messages($type = NULL, $clear_queue = TRUE) {
  if ($messages = drupal_set_message()) {
    if ($type) {
      if ($clear_queue) {
        unset($_SESSION['messages'][$type]);
      }
      if (isset($messages[$type])) {
        return array($type => $messages[$type]);
      }
    }
    else {
      if ($clear_queue) {
        unset($_SESSION['messages']);
      }
      return $messages;
    }
  }
  return array();
}

/**
 * Perform an access check for a given mask and rule type. Rules are usually
 * created via admin/user/rules page.
 *
 * If any allow rule matches, access is allowed. Otherwise, if any deny rule
 * matches, access is denied.  If no rule matches, access is allowed.
 *
 * @param $type string
 *   Type of access to check: Allowed values are:
 *     - 'host': host name or IP address
 *     - 'mail': e-mail address
 *     - 'user': username
 * @param $mask string
 *   String or mask to test: '_' matches any character, '%' matches any
 *   number of characters.
 * @return bool
 *   TRUE if access is denied, FALSE if access is allowed.
 */
function drupal_is_denied($type, $mask) {
  // Because this function is called for every page request, both cached
  // and non-cached pages, we tried to optimize it as much as possible.
  // We deny access if the only matching records in the {access} table have
  // status 0 (deny). If any have status 1 (allow), or if there are no
  // matching records, we allow access.
  $sql = "SELECT 1 FROM {access} WHERE type = '%s' AND LOWER('%s') LIKE LOWER(mask) AND status = %d";
  return db_result(db_query_range($sql, $type, $mask, 0, 0, 1)) && !db_result(db_query_range($sql, $type, $mask, 1, 0, 1));
}

/**
 * Handles denied users. Backport from D7
 *
 * @param $ip
 *   IP address to check. Prints a message and exits if access is denied.
 */
function drupal_block_denied($ip) {
  // Deny access to blocked IP addresses - t() is not yet available.
  if (drupal_is_denied('host', $ip)) {
    header($_SERVER['SERVER_PROTOCOL'] . ' 403 Forbidden');
    print 'Sorry, ' . check_plain(ip_address()) . ' has been banned.';
    exit();
  }
}

/**
 * Generates a default anonymous $user object.
 *
 * @return Object - the user object.
 */
function drupal_anonymous_user($session = '') {
  $user = new stdClass();
  $user->uid = 0;
  $user->hostname = ip_address();
  $user->roles = array();
  $user->roles[DRUPAL_ANONYMOUS_RID] = 'anonymous user';
  $user->session = $session;
  $user->cache = 0;
  return $user;
}

/**
 * A string describing a phase of Drupal to load. Each phase adds to the
 * previous one, so invoking a later phase automatically runs the earlier
 * phases too. The most important usage is that if you want to access the
 * Drupal database from a script without loading anything else, you can
 * include bootstrap.inc, and call drupal_bootstrap(DRUPAL_BOOTSTRAP_DATABASE).
 *
 * @param $phase
 *   A constant. Allowed values are:
 *     DRUPAL_BOOTSTRAP_CONFIGURATION: initialize configuration.
 *     DRUPAL_BOOTSTRAP_EARLY_PAGE_CACHE: try to call a non-database cache fetch routine.
 *     DRUPAL_BOOTSTRAP_DATABASE: initialize database layer.
 *     DRUPAL_BOOTSTRAP_ACCESS: identify and reject banned hosts.
 *     DRUPAL_BOOTSTRAP_SESSION: initialize session handling.
 *     DRUPAL_BOOTSTRAP_LATE_PAGE_CACHE: load bootstrap.inc and module.inc, start
 *       the variable system and try to serve a page from the cache.
 *     DRUPAL_BOOTSTRAP_LANGUAGE: identify the language used on the page.
 *     DRUPAL_BOOTSTRAP_PATH: set $_GET['q'] to Drupal path of request.
 *     DRUPAL_BOOTSTRAP_FULL: Drupal is fully loaded, validate and fix input data.
 */
function drupal_bootstrap($phase = NULL) {
  static $phases = array(DRUPAL_BOOTSTRAP_CONFIGURATION, DRUPAL_BOOTSTRAP_EARLY_PAGE_CACHE, DRUPAL_BOOTSTRAP_DATABASE, DRUPAL_BOOTSTRAP_ACCESS, DRUPAL_BOOTSTRAP_SESSION, DRUPAL_BOOTSTRAP_LATE_PAGE_CACHE, DRUPAL_BOOTSTRAP_LANGUAGE, DRUPAL_BOOTSTRAP_PATH, DRUPAL_BOOTSTRAP_FULL), $phase_index = 0;

  while ($phase >= $phase_index && isset($phases[$phase_index])) {
    $current_phase = $phases[$phase_index];
    unset($phases[$phase_index++]);
    _drupal_bootstrap($current_phase);
  }

  return $phase_index;
}

function _drupal_bootstrap($phase) {
  global $conf, $db_prefix;

  switch ($phase) {

    case DRUPAL_BOOTSTRAP_CONFIGURATION:
      drupal_unset_globals();
      // Start a page timer:
      timer_start('page');
      // Initialize the configuration
      conf_init();
      break;

    case DRUPAL_BOOTSTRAP_EARLY_PAGE_CACHE:
      // Allow specifying special cache handlers in settings.php, like
      // using memcached or files for storing cache information.
      require_once variable_get('cache_inc', './includes/cache.inc');
      // If the page_cache_fastpath is set to TRUE in settings.php and
      // page_cache_fastpath (implemented in the special implementation of
      // cache.inc) printed the page and indicated this with a returned TRUE
      // then we are done.
      if (variable_get('page_cache_fastpath', FALSE) && page_cache_fastpath()) {
        exit;
      }
      break;

    case DRUPAL_BOOTSTRAP_DATABASE:
      // The user agent header is used to pass a database prefix in the request when
      // running tests. However, for security reasons, it is imperative that we
      // validate we ourselves made the request.
      $GLOBALS['simpletest_installed'] = TRUE;
      if (isset($_SERVER['HTTP_USER_AGENT']) && preg_match("/^(simpletest\d+);/", $_SERVER['HTTP_USER_AGENT'], $matches)) {
        if (!drupal_valid_test_ua($_SERVER['HTTP_USER_AGENT'])) {
          header($_SERVER['SERVER_PROTOCOL'] . ' 403 Forbidden');
          exit;
        }
        $db_prefix_string = is_array($db_prefix) ? $db_prefix['default'] : $db_prefix;
        $db_prefix = $db_prefix_string . $matches[1];
      }

      // Initialize the default database.
      require_once './includes/database.inc';
      db_set_active();
      // Allow specifying alternate lock implementations in settings.php, like
      // those using APC or memcached.
      require_once variable_get('lock_inc', './includes/lock.inc');
      lock_init();
      break;

    case DRUPAL_BOOTSTRAP_ACCESS:
      // Deny access to hosts which were banned - t() is not yet available.
      if (drupal_is_denied('host', ip_address())) {
        header($_SERVER['SERVER_PROTOCOL'] .' 403 Forbidden');
        print 'Sorry, '. check_plain(ip_address()) .' has been banned.';
        exit();
      }
      break;

    case DRUPAL_BOOTSTRAP_SESSION:
      require_once variable_get('session_inc', './includes/session.inc');

      // Backport from D7
      drupal_session_initialize();

      break;

    case DRUPAL_BOOTSTRAP_LATE_PAGE_CACHE:
      // Initialize configuration variables, using values from settings.php if available.
      $conf = variable_init(isset($conf) ? $conf : array());

      // Sanitize the destination parameter (which is often used for redirects)
      // to prevent open redirect attacks leading to other domains. Sanitize
      // both $_GET['destination'] and $_REQUEST['destination'] to protect code
      // that relies on either, but do not sanitize $_POST to avoid interfering
      // with unrelated form submissions. $_REQUEST['edit']['destination'] is
      // also sanitized since drupal_goto() will sometimes rely on it, and
      // other code might therefore use it too. The sanitization happens here
      // because menu_path_is_external() requires the variable system to be
      // available.
      if (isset($_GET['destination']) || isset($_REQUEST['destination']) || isset($_REQUEST['edit']['destination'])) {
        require_once './includes/menu.inc';
        drupal_load('module', 'filter');
        // If the destination is an external URL, remove it.
        if (isset($_GET['destination']) && menu_path_is_external($_GET['destination'])) {
          unset($_GET['destination']);
          unset($_REQUEST['destination']);
        }
        // If there's still something in $_REQUEST['destination'] that didn't
        // come from $_GET, check it too.
        if (isset($_REQUEST['destination']) && (!isset($_GET['destination']) || $_REQUEST['destination'] != $_GET['destination']) && menu_path_is_external($_REQUEST['destination'])) {
          unset($_REQUEST['destination']);
        }
        // Check $_REQUEST['edit']['destination'] separately.
        if (isset($_REQUEST['edit']['destination']) && menu_path_is_external($_REQUEST['edit']['destination'])) {
          unset($_REQUEST['edit']['destination']);
        }
      }

      $cache_mode = variable_get('cache', CACHE_DISABLED);
      
      $cache_enabled = $cache_mode && !isset($_COOKIE[session_name()]);
      // Get the page from the cache.
      $cache = $cache_mode == CACHE_DISABLED && $cache_enabled ? '' : drupal_page_get_cache();
      // If the skipping of the bootstrap hooks is not enforced, call hook_boot.
      if (!$cache || $cache_mode != CACHE_AGGRESSIVE) {
        // Load module handling.
        require_once './includes/module.inc';
        bootstrap_invoke_all('boot');
      }

      if ($cache_enabled) {
        // If there is a cached page, display it.
        if ($cache) {
          // backport compatibility
          header('X-Drupal-Cache: HIT');

          drupal_serve_page_from_cache($cache);
          // If the skipping of the bootstrap hooks is not enforced, call hook_exit.
          if ($cache_mode != CACHE_AGGRESSIVE) {
            bootstrap_invoke_all('exit');
          }
          // We are done.
          exit;
        } else {
          // backport compatibility
          header('X-Drupal-Cache: MISS');
        }
      }

      // Prepare for non-cached page workflow.
      // Backport from D7, DRUPAL_BOOTSTRAP_PAGE_HEADER
      if (!drupal_is_cli()) {
        ob_start();
        drupal_page_header();
      }
      break;

    case DRUPAL_BOOTSTRAP_LANGUAGE:
      drupal_init_language();
      break;

    case DRUPAL_BOOTSTRAP_PATH:
      require_once './includes/path.inc';
      // Initialize $_GET['q'] prior to loading modules and invoking hook_init().
      drupal_init_path();
      break;

    case DRUPAL_BOOTSTRAP_FULL:
      require_once './includes/common.inc';
      _drupal_bootstrap_full();
      break;
  }
}

/**
 * Returns the time zone of the current user. Backport from D7
 */
function drupal_get_user_timezone() {
  global $user;
  if (variable_get('configurable_timezones', 1) && $user->uid && $user->timezone) {
    return $user->timezone;
  }
  else {
    // Ignore PHP strict notice if time zone has not yet been set in the php.ini
    // configuration.
    return variable_get('date_default_timezone', @date_default_timezone_get());
  }
}

/**
 * Gets a salt useful for hardening against SQL injection. Backport from D7
 *
 * @return
 *   A salt based on information in settings.php, not in the database.
 */
function drupal_get_hash_salt() {
  global $drupal_hash_salt, $databases;
  // If the $drupal_hash_salt variable is empty, a hash of the serialized
  // database credentials is used as a fallback salt.
  return empty($drupal_hash_salt) ? hash('sha256', serialize($databases)) : $drupal_hash_salt;
}

/**
 * Enables use of the theme system without requiring database access.
 *
 * Loads and initializes the theme system for site installs, updates and when
 * the site is in off-line mode. This also applies when the database fails.
 *
 * @see _drupal_maintenance_theme()
 */
function drupal_maintenance_theme() {
  require_once './includes/theme.maintenance.inc';
  _drupal_maintenance_theme();
}

/**
 * Return the name of the localisation function. Use in code that needs to
 * run both during installation and normal operation.
 */
function get_t() {
  static $t;
  if (is_null($t)) {
    $t = function_exists('install_main') ? 'st' : 't';
  }
  return $t;
}

/**
 *  Choose a language for the current page, based on site and user preferences.
 */
function drupal_init_language() {
  global $language, $user;

  // Ensure the language is correctly returned, even without multilanguage support.
  // Useful for eg. XML/HTML 'lang' attributes.
  if (variable_get('language_count', 1) == 1) {
    $language = language_default();
  }
  else {
    include_once './includes/language.inc';
    $language = language_initialize();
  }
}

/**
 * Get a list of languages set up indexed by the specified key
 *
 * @param $field The field to index the list with.
 * @param $reset Boolean to request a reset of the list.
 */
function language_list($field = 'language', $reset = FALSE) {
  static $languages = NULL;

  // Reset language list
  if ($reset) {
    $languages = NULL;
  }

  // Init language list
  if (!isset($languages)) {
    if (variable_get('language_count', 1) > 1 || module_exists('locale')) {
      $result = db_query('SELECT * FROM {languages} ORDER BY weight ASC, name ASC');
      while ($row = db_fetch_object($result)) {
        $languages['language'][$row->language] = $row;
      }
    }
    else {
      // No locale module, so use the default language only.
      $default = language_default();
      $languages['language'][$default->language] = $default;
    }
  }

  // Return the array indexed by the right field
  if (!isset($languages[$field])) {
    $languages[$field] = array();
    foreach ($languages['language'] as $lang) {
      // Some values should be collected into an array
      if (in_array($field, array('enabled', 'weight'))) {
        $languages[$field][$lang->$field][$lang->language] = $lang;
      }
      else {
        $languages[$field][$lang->$field] = $lang;
      }
    }
  }
  return $languages[$field];
}

/**
 * Default language used on the site
 *
 * @param $property
 *   Optional property of the language object to return
 */
function language_default($property = NULL) {
  $language = variable_get('language_default', (object) array('language' => 'en', 'name' => 'English', 'native' => 'English', 'direction' => 0, 'enabled' => 1, 'plurals' => 0, 'formula' => '', 'domain' => '', 'prefix' => '', 'weight' => 0, 'javascript' => ''));
  return $property ? $language->$property : $language;
}

/**
 * Returns the IP address of the client machine. Version from D7
 *
 * If Drupal is behind a reverse proxy, we use the X-Forwarded-For header
 * instead of $_SERVER['REMOTE_ADDR'], which would be the IP address of
 * the proxy server, and not the client's. The actual header name can be
 * configured by the reverse_proxy_header variable.
 *
 * @return
 *   IP address of client machine, adjusted for reverse proxy and/or cluster
 *   environments.
 */
function ip_address() {
  $ip_address = &drupal_static(__FUNCTION__);

  if (!isset($ip_address)) {
    $ip_address = $_SERVER['REMOTE_ADDR'];

    if (variable_get('reverse_proxy', 0)) {
      $reverse_proxy_header = variable_get('reverse_proxy_header', 'HTTP_X_FORWARDED_FOR');
      if (!empty($_SERVER[$reverse_proxy_header])) {
        // If an array of known reverse proxy IPs is provided, then trust
        // the XFF header if request really comes from one of them.
        $reverse_proxy_addresses = variable_get('reverse_proxy_addresses', array());

        // Turn XFF header into an array.
        $forwarded = explode(',', $_SERVER[$reverse_proxy_header]);

        // Trim the forwarded IPs; they may have been delimited by commas and spaces.
        $forwarded = array_map('trim', $forwarded);

        // Tack direct client IP onto end of forwarded array.
        $forwarded[] = $ip_address;

        // Eliminate all trusted IPs.
        $untrusted = array_diff($forwarded, $reverse_proxy_addresses);

        // The right-most IP is the most specific we can trust.
        $ip_address = array_pop($untrusted);
      }
    }
  }

  return $ip_address;
}

/**
 * Returns a URL-safe, base64 encoded string of highly randomized bytes (over the full 8-bit range).
 *
 * @param $byte_count
 *   The number of random bytes to fetch and base64 encode.
 *
 * @return string
 *   The base64 encoded result will have a length of up to 4 * $byte_count.
 */
function drupal_random_key($byte_count = 32) {
  return drupal_base64_encode(drupal_random_bytes($byte_count));
}

/**
 * Returns a URL-safe, base64 encoded version of the supplied string.
 *
 * @param $string
 *   The string to convert to base64.
 *
 * @return string
 */
function drupal_base64_encode($string) {
  $data = base64_encode($string);
  // Modify the output so it's safe to use in URLs.
  return strtr($data, array('+' => '-', '/' => '_', '=' => ''));
}

/**
 * Returns a string of highly randomized bytes (over the full 8-bit range).
 *
 * This function is better than simply calling mt_rand() or any other built-in
 * PHP function because it can return a long string of bytes (compared to < 4
 * bytes normally from mt_rand()) and uses the best available pseudo-random
 * source.
 *
 * @param $count
 *   The number of characters (bytes) to return in the string.
 */
function drupal_random_bytes($count) {
  // $random_state does not use drupal_static as it stores random bytes.
  static $random_state, $bytes, $has_openssl, $has_hash;

  $missing_bytes = $count - strlen($bytes);

  if ($missing_bytes > 0) {
    // PHP versions prior 5.3.4 experienced openssl_random_pseudo_bytes()
    // locking on Windows and rendered it unusable.
    if (!isset($has_openssl)) {
      $has_openssl = version_compare(PHP_VERSION, '5.3.4', '>=') && function_exists('openssl_random_pseudo_bytes');
    }

    // openssl_random_pseudo_bytes() will find entropy in a system-dependent
    // way.
    if ($has_openssl) {
      $bytes .= openssl_random_pseudo_bytes($missing_bytes);
    }

    // Else, read directly from /dev/urandom, which is available on many *nix
    // systems and is considered cryptographically secure.
    elseif ($fh = @fopen('/dev/urandom', 'rb')) {
      // PHP only performs buffered reads, so in reality it will always read
      // at least 4096 bytes. Thus, it costs nothing extra to read and store
      // that much so as to speed any additional invocations.
      $bytes .= fread($fh, max(4096, $missing_bytes));
      fclose($fh);
    }

    // If we couldn't get enough entropy, this simple hash-based PRNG will
    // generate a good set of pseudo-random bytes on any system.
    // Note that it may be important that our $random_state is passed
    // through hash() prior to being rolled into $output, that the two hash()
    // invocations are different, and that the extra input into the first one -
    // the microtime() - is prepended rather than appended. This is to avoid
    // directly leaking $random_state via the $output stream, which could
    // allow for trivial prediction of further "random" numbers.
    if (strlen($bytes) < $count) {
      // Initialize on the first call. The contents of $_SERVER includes a mix of
      // user-specific and system information that varies a little with each page.
      if (!isset($random_state)) {
        $random_state = print_r($_SERVER, TRUE);
        if (function_exists('getmypid')) {
          // Further initialize with the somewhat random PHP process ID.
          $random_state .= getmypid();
        }
        // hash() is only available in PHP 5.1.2+ or via PECL.
        $has_hash = function_exists('hash') && in_array('sha256', hash_algos());
        $bytes = '';
      }

      if ($has_hash) {
        do {
          $random_state = hash('sha256', microtime() . mt_rand() . $random_state);
          $bytes .= hash('sha256', mt_rand() . $random_state, TRUE);
        } while (strlen($bytes) < $count);
      }
      else {
        do {
          $random_state = md5(microtime() . mt_rand() . $random_state);
          $bytes .= pack("H*", md5(mt_rand() . $random_state));
        } while (strlen($bytes) < $count);
      }
    }
  }
  $output = substr($bytes, 0, $count);
  $bytes = substr($bytes, $count);
  return $output;
}

/**
 * Calculates a hexadecimal encoded sha-1 hmac.
 *
 * @param string $data
 *   String to be validated with the hmac.
 * @param string $key
 *   A secret string key.
 *
 * See RFC2104 (http://www.ietf.org/rfc/rfc2104.txt). Note, the result of this
 * must be identical to using hash_hmac('sha1', $data, $key);  We don't use
 * that function since PHP can be missing it if it was compiled with the
 * --disable-hash switch.
 *
 * @return string
 *   A hexadecimal encoded sha-1 hmac.
 */
function drupal_hash_hmac_sha1($data, $key) {
  // Keys longer than the 64 byte block size must be hashed first.
  if (strlen($key) > 64) {
    $key = pack("H*", sha1($key));
  }
  return sha1((str_pad($key, 64, chr(0x00)) ^ (str_repeat(chr(0x5c), 64))) . pack("H*", sha1((str_pad($key, 64, chr(0x00)) ^ (str_repeat(chr(0x36), 64))) . $data)));
}

/**
 * Calculates a base-64 encoded, URL-safe sha-1 hmac.
 *
 * @param string $data
 *   String to be validated with the hmac.
 * @param string $key
 *   A secret string key.
 *
 * @return string
 *   A base-64 encoded sha-1 hmac, with + replaced with -, / with _ and
 *   any = padding characters removed.
 */
function drupal_hmac_base64($data, $key) {
  // Casting $data and $key to strings here is necessary to avoid empty string
  // results of the hash function if they are not scalar values. As this
  // function is used in security-critical contexts like token validation it is
  // important that it never returns an empty string.
  $hmac = base64_encode(pack("H*", drupal_hash_hmac_sha1((string) $data, (string) $key)));
  // Modify the hmac so it's safe to use in URLs.
  return strtr($hmac, array('+' => '-', '/' => '_', '=' => ''));
}

/**
 * Calculates a base-64 encoded, URL-safe sha-256 hash. Backport from D7.
 *
 * @param $data
 *   String to be hashed.
 *
 * @return
 *   A base-64 encoded sha-256 hash, with + replaced with -, / with _ and
 *   any = padding characters removed.
 */
function drupal_hash_base64($data) {
  $hash = base64_encode(hash('sha256', $data, TRUE));
  // Modify the hash so it's safe to use in URLs.
  return strtr($hash, array('+' => '-', '/' => '_', '=' => ''));
}

/**
 * Validate the HMAC and timestamp of a user agent header from simpletest.
 */
function drupal_valid_test_ua() {
  // No reason to reset this.
  static $test_prefix;

  if (isset($test_prefix)) {
    return $test_prefix;
  }

  if (isset($_SERVER['HTTP_USER_AGENT']) && preg_match("/^(simpletest\d+);(.+);(.+);(.+)$/", $_SERVER['HTTP_USER_AGENT'], $matches)) {
    list(, $prefix, $time, $salt, $hmac) = $matches;
    $check_string =  $prefix . ';' . $time . ';' . $salt;
    // We use the salt from settings.php to make the HMAC key, since
    // the database is not yet initialized and we can't access any Drupal variables.
    // The file properties add more entropy not easily accessible to others.
    $key = drupal_get_hash_salt() . filectime(__FILE__) . fileinode(__FILE__);
    $time_diff = REQUEST_TIME - $time;
    // Since we are making a local request a 5 second time window is allowed,
    // and the HMAC must match.
    if ($time_diff >= 0 && $time_diff <= 5 && $hmac == drupal_hmac_base64($check_string, $key)) {
      $test_prefix = $prefix;
      return $test_prefix;
    }
  }

  $test_prefix = FALSE;
  return $test_prefix;
}

/**
 * Generates a user agent string with a HMAC and timestamp for simpletest. Backport from D7
 */
function drupal_generate_test_ua($prefix) {
  static $key;

  if (!isset($key)) {
    // We use the salt from settings.php to make the HMAC key, since
    // the database is not yet initialized and we can't access any Drupal variables.
    // The file properties add more entropy not easily accessible to others.
    $key = drupal_get_hash_salt() . filectime(__FILE__) . fileinode(__FILE__);
  }
  // Generate a moderately secure HMAC based on the database credentials.
  $salt = uniqid('', TRUE);
  $check_string = $prefix . ';' . time() . ';' . $salt;
  return $check_string . ';' . drupal_hmac_base64($check_string, $key);
}

/**
 * Merges multiple arrays, recursively, and returns the merged array. Backport from D7
 *
 * This function is similar to PHP's array_merge_recursive() function, but it
 * handles non-array values differently. When merging values that are not both
 * arrays, the latter value replaces the former rather than merging with it.
 *
 * Example:
 * @code
 * $link_options_1 = array('fragment' => 'x', 'attributes' => array('title' => t('X'), 'class' => array('a', 'b')));
 * $link_options_2 = array('fragment' => 'y', 'attributes' => array('title' => t('Y'), 'class' => array('c', 'd')));
 *
 * // This results in array('fragment' => array('x', 'y'), 'attributes' => array('title' => array(t('X'), t('Y')), 'class' => array('a', 'b', 'c', 'd'))).
 * $incorrect = array_merge_recursive($link_options_1, $link_options_2);
 *
 * // This results in array('fragment' => 'y', 'attributes' => array('title' => t('Y'), 'class' => array('a', 'b', 'c', 'd'))).
 * $correct = drupal_array_merge_deep($link_options_1, $link_options_2);
 * @endcode
 *
 * @param ...
 *   Arrays to merge.
 *
 * @return
 *   The merged array.
 *
 * @see drupal_array_merge_deep_array()
 */
function drupal_array_merge_deep() {
  $args = func_get_args();
  return drupal_array_merge_deep_array($args);
}

/**
 * Merges multiple arrays, recursively, and returns the merged array. Backport from D7
 *
 * This function is equivalent to drupal_array_merge_deep(), except the
 * input arrays are passed as a single array parameter rather than a variable
 * parameter list.
 *
 * The following are equivalent:
 * - drupal_array_merge_deep($a, $b);
 * - drupal_array_merge_deep_array(array($a, $b));
 *
 * The following are also equivalent:
 * - call_user_func_array('drupal_array_merge_deep', $arrays_to_merge);
 * - drupal_array_merge_deep_array($arrays_to_merge);
 *
 * @see drupal_array_merge_deep()
 */
function drupal_array_merge_deep_array($arrays) {
  $result = array();

  foreach ($arrays as $array) {
    foreach ($array as $key => $value) {
      // Renumber integer keys as array_merge_recursive() does. Note that PHP
      // automatically converts array keys that are integer strings (e.g., '1')
      // to integers.
      if (is_integer($key)) {
        $result[] = $value;
      }
      // Recurse when both values are arrays.
      elseif (isset($result[$key]) && is_array($result[$key]) && is_array($value)) {
        $result[$key] = drupal_array_merge_deep_array(array($result[$key], $value));
      }
      // Otherwise, use the latter value, overriding any previous value.
      else {
        $result[$key] = $value;
      }
    }
  }

  return $result;
}

/**
 * Registers a function for execution on shutdown. Backport from D7
 *
 * Wrapper for register_shutdown_function() that catches thrown exceptions to
 * avoid "Exception thrown without a stack frame in Unknown".
 *
 * @param $callback
 *   The shutdown function to register.
 * @param ...
 *   Additional arguments to pass to the shutdown function.
 *
 * @return
 *   Array of shutdown functions to be executed.
 *
 * @see register_shutdown_function()
 * @ingroup php_wrappers
 */
function &drupal_register_shutdown_function($callback = NULL) {
  // We cannot use drupal_static() here because the static cache is reset during
  // batch processing, which breaks batch handling.
  static $callbacks = array();

  if (isset($callback)) {
    // Only register the internal shutdown function once.
    if (empty($callbacks)) {
      register_shutdown_function('_drupal_shutdown_function');
    }
    $args = func_get_args();
    array_shift($args);
    // Save callback and arguments
    $callbacks[] = array('callback' => $callback, 'arguments' => $args);
  }
  return $callbacks;
}

/**
 * Executes registered shutdown functions. Backport from D7
 */
function _drupal_shutdown_function() {
  $callbacks = &drupal_register_shutdown_function();

  // Set the CWD to DRUPAL_ROOT as it is not guaranteed to be the same as it
  // was in the normal context of execution.
  chdir(DRUPAL_ROOT);

  try {
    while (list($key, $callback) = each($callbacks)) {
      call_user_func_array($callback['callback'], $callback['arguments']);
    }
  }
  catch (Exception $exception) {
    // If we are displaying errors, then do so with no possibility of a further uncaught exception being thrown.
   require_once DRUPAL_ROOT . '/includes/errors.inc';
   if (error_displayable()) {
      print '<h1>Uncaught exception thrown in shutdown function.</h1>';
      print '<p>' . _drupal_render_exception_safe($exception) . '</p><hr />';
    }
  }
}

/**
 * Compares the memory required for an operation to the available memory. Backport from D7
 *
 * @param $required
 *   The memory required for the operation, expressed as a number of bytes with
 *   optional SI or IEC binary unit prefix (e.g. 2, 3K, 5MB, 10G, 6GiB, 8bytes,
 *   9mbytes).
 * @param $memory_limit
 *   (optional) The memory limit for the operation, expressed as a number of
 *   bytes with optional SI or IEC binary unit prefix (e.g. 2, 3K, 5MB, 10G,
 *   6GiB, 8bytes, 9mbytes). If no value is passed, the current PHP
 *   memory_limit will be used. Defaults to NULL.
 *
 * @return
 *   TRUE if there is sufficient memory to allow the operation, or FALSE
 *   otherwise.
 */
function drupal_check_memory_limit($required, $memory_limit = NULL) {
  if (!isset($memory_limit)) {
    $memory_limit = ini_get('memory_limit');
  }

  // There is sufficient memory if:
  // - No memory limit is set.
  // - The memory limit is set to unlimited (-1).
  // - The memory limit is greater than the memory required for the operation.
  return ((!$memory_limit) || ($memory_limit == -1) || (parse_size($memory_limit) >= parse_size($required)));
}

/**
 * Provides central static variable storage. Backport from D7
 *
 * All functions requiring a static variable to persist or cache data within
 * a single page request are encouraged to use this function unless it is
 * absolutely certain that the static variable will not need to be reset during
 * the page request. By centralizing static variable storage through this
 * function, other functions can rely on a consistent API for resetting any
 * other function's static variables.
 *
 * Example:
 * @code
 * function language_list($field = 'language') {
 *   $languages = &drupal_static(__FUNCTION__);
 *   if (!isset($languages)) {
 *     // If this function is being called for the first time after a reset,
 *     // query the database and execute any other code needed to retrieve
 *     // information about the supported languages.
 *     ...
 *   }
 *   if (!isset($languages[$field])) {
 *     // If this function is being called for the first time for a particular
 *     // index field, then execute code needed to index the information already
 *     // available in $languages by the desired field.
 *     ...
 *   }
 *   // Subsequent invocations of this function for a particular index field
 *   // skip the above two code blocks and quickly return the already indexed
 *   // information.
 *   return $languages[$field];
 * }
 * function locale_translate_overview_screen() {
 *   // When building the content for the translations overview page, make
 *   // sure to get completely fresh information about the supported languages.
 *   drupal_static_reset('language_list');
 *   ...
 * }
 * @endcode
 *
 * In a few cases, a function can have certainty that there is no legitimate
 * use-case for resetting that function's static variable. This is rare,
 * because when writing a function, it's hard to forecast all the situations in
 * which it will be used. A guideline is that if a function's static variable
 * does not depend on any information outside of the function that might change
 * during a single page request, then it's ok to use the "static" keyword
 * instead of the drupal_static() function.
 *
 * Example:
 * @code
 * function actions_do(...) {
 *   // $stack tracks the number of recursive calls.
 *   static $stack;
 *   $stack++;
 *   if ($stack > variable_get('actions_max_stack', 35)) {
 *     ...
 *     return;
 *   }
 *   ...
 *   $stack--;
 * }
 * @endcode
 *
 * In a few cases, a function needs a resettable static variable, but the
 * function is called many times (100+) during a single page request, so
 * every microsecond of execution time that can be removed from the function
 * counts. These functions can use a more cumbersome, but faster variant of
 * calling drupal_static(). It works by storing the reference returned by
 * drupal_static() in the calling function's own static variable, thereby
 * removing the need to call drupal_static() for each iteration of the function.
 * Conceptually, it replaces:
 * @code
 * $foo = &drupal_static(__FUNCTION__);
 * @endcode
 * with:
 * @code
 * // Unfortunately, this does not work.
 * static $foo = &drupal_static(__FUNCTION__);
 * @endcode
 * However, the above line of code does not work, because PHP only allows static
 * variables to be initializied by literal values, and does not allow static
 * variables to be assigned to references.
 * - http://php.net/manual/language.variables.scope.php#language.variables.scope.static
 * - http://php.net/manual/language.variables.scope.php#language.variables.scope.references
 * The example below shows the syntax needed to work around both limitations.
 * For benchmarks and more information, see http://drupal.org/node/619666.
 *
 * Example:
 * @code
 * function user_access($string, $account = NULL) {
 *   // Use the advanced drupal_static() pattern, since this is called very often.
 *   static $drupal_static_fast;
 *   if (!isset($drupal_static_fast)) {
 *     $drupal_static_fast['perm'] = &drupal_static(__FUNCTION__);
 *   }
 *   $perm = &$drupal_static_fast['perm'];
 *   ...
 * }
 * @endcode
 *
 * @param $name
 *   Globally unique name for the variable. For a function with only one static,
 *   variable, the function name (e.g. via the PHP magic __FUNCTION__ constant)
 *   is recommended. For a function with multiple static variables add a
 *   distinguishing suffix to the function name for each one.
 * @param $default_value
 *   Optional default value.
 * @param $reset
 *   TRUE to reset one or all variables(s). This parameter is only used
 *   internally and should not be passed in; use drupal_static_reset() instead.
 *   (This function's return value should not be used when TRUE is passed in.)
 *
 * @return
 *   Returns a variable by reference.
 *
 * @see drupal_static_reset()
 */
function &drupal_static($name, $default_value = NULL, $reset = FALSE) {
  static $data = array(), $default = array();
  // First check if dealing with a previously defined static variable.
  if (isset($data[$name]) || array_key_exists($name, $data)) {
    // Non-NULL $name and both $data[$name] and $default[$name] statics exist.
    if ($reset) {
      // Reset pre-existing static variable to its default value.
      $data[$name] = $default[$name];
    }
    return $data[$name];
  }
  // Neither $data[$name] nor $default[$name] static variables exist.
  if (isset($name)) {
    if ($reset) {
      // Reset was called before a default is set and yet a variable must be
      // returned.
      return $data;
    }
    // First call with new non-NULL $name. Initialize a new static variable.
    $default[$name] = $data[$name] = $default_value;
    return $data[$name];
  }
  // Reset all: ($name == NULL). This needs to be done one at a time so that
  // references returned by earlier invocations of drupal_static() also get
  // reset.
  foreach ($default as $name => $value) {
    $data[$name] = $value;
  }
  // As the function returns a reference, the return should always be a
  // variable.
  return $data;
}

/**
 * Resets one or all centrally stored static variable(s). Backport from D7.
 *
 * @param $name
 *   Name of the static variable to reset. Omit to reset all variables.
 *   Resetting all variables should only be used, for example, for running unit
 *   tests with a clean environment.
 */
function drupal_static_reset($name = NULL) {
  drupal_static($name, NULL, TRUE);
}

/**
 * Detects whether the current script is running in a command-line environment. Backport from D7
 */
function drupal_is_cli() {
  return (!isset($_SERVER['SERVER_SOFTWARE']) && (php_sapi_name() == 'cli' || (is_numeric($_SERVER['argc']) && $_SERVER['argc'] > 0)));
}

/**
 * Formats text for emphasized display in a placeholder inside a sentence. Backport from D7
 *
 * Used automatically by format_string().
 *
 * @param $text
 *   The text to format (plain-text).
 *
 * @return
 *   The formatted text (html).
 */
function drupal_placeholder($text) {
  return '<em class="placeholder">' . check_plain($text) . '</em>';
}