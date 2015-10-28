<?php

/**
 * @file
 * Common functions that many Drupal modules will need to reference.
 *
 * The functions that are critical and need to be available even when serving
 * a cached page are instead located in bootstrap.inc.
 */

/**
 * Return status for saving which involved creating a new item.
 */
define('SAVED_NEW', 1);

/**
 * Return status for saving which involved an update to an existing item.
 */
define('SAVED_UPDATED', 2);

/**
 * Return status for saving which deleted an existing item.
 */
define('SAVED_DELETED', 3);

/**
 * The default group for system CSS files added to the page.
 */
define('CSS_SYSTEM', -100);

/**
 * The default group for module CSS files added to the page.
 */
define('CSS_DEFAULT', 0);

/**
 * The default group for theme CSS files added to the page.
 */
define('CSS_THEME', 100);

/**
 * The default group for JavaScript and jQuery libraries added to the page.
 */
define('JS_LIBRARY', -100);

/**
 * The default group for module JavaScript code added to the page.
 */
define('JS_DEFAULT', 0);

/**
 * The default group for theme JavaScript code added to the page.
 */
define('JS_THEME', 100);

/**
 * Create E_DEPRECATED constant for older PHP versions (<5.3).
 */
if (!defined('E_DEPRECATED')) {
  define('E_DEPRECATED', 8192);
}

/**
 * Error code indicating that the request made by drupal_http_request() exceeded
 * the specified timeout.
 */
define('HTTP_REQUEST_TIMEOUT', -1);

/**
 * Set content for a specified region.
 *
 * @param $region
 *   Page region the content is assigned to.
 * @param $data
 *   Content to be set.
 */
function drupal_set_content($region = NULL, $data = NULL) {
  static $content = array();

  if (!is_null($region) && !is_null($data)) {
    $content[$region][] = $data;
  }
  return $content;
}

/**
 * Get assigned content.
 *
 * @param $region
 *   A specified region to fetch content for. If NULL, all regions will be
 *   returned.
 * @param $delimiter
 *   Content to be inserted between imploded array elements.
 */
function drupal_get_content($region = NULL, $delimiter = ' ') {
  $content = drupal_set_content();
  if (isset($region)) {
    if (isset($content[$region]) && is_array($content[$region])) {
      return implode($delimiter, $content[$region]);
    }
  }
  else {
    foreach (array_keys($content) as $region) {
      if (is_array($content[$region])) {
        $content[$region] = implode($delimiter, $content[$region]);
      }
    }
    return $content;
  }
}

/**
 * Gets the name of the currently active installation profile. Backport from D7.
 *
 * When this function is called during Drupal's initial installation process,
 * the name of the profile that's about to be installed is stored in the global
 * installation state. At all other times, the standard Drupal systems variable
 * table contains the name of the current profile, and we can call
 * variable_get() to determine what one is active.
 *
 * @return $profile
 *   The name of the installation profile.
 */
function drupal_get_profile() {
  return variable_get('install_profile', 'standard');
}

/**
 * Set the breadcrumb trail for the current page.
 *
 * @param $breadcrumb
 *   Array of links, starting with "home" and proceeding up to but not including
 *   the current page.
 */
function drupal_set_breadcrumb($breadcrumb = NULL) {
  static $stored_breadcrumb;

  if (!is_null($breadcrumb)) {
    $stored_breadcrumb = $breadcrumb;
  }
  return $stored_breadcrumb;
}

/**
 * Get the breadcrumb trail for the current page.
 */
function drupal_get_breadcrumb() {
  $breadcrumb = drupal_set_breadcrumb();

  if (is_null($breadcrumb)) {
    $breadcrumb = menu_get_active_breadcrumb();
  }

  return $breadcrumb;
}

/**
 * Add output to the head tag of the HTML page.
 *
 * This function can be called as long the headers aren't sent.
 */
function drupal_set_html_head($data = NULL) {
  static $stored_head = '';

  if (!is_null($data)) {
    $stored_head .= $data ."\n";
  }
  return $stored_head;
}

/**
 * Retrieve output to be displayed in the head tag of the HTML page.
 */
function drupal_get_html_head() {
  $output = "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n";
  return $output . drupal_set_html_head();
}

/**
 * Reset the static variable which holds the aliases mapped for this request.
 */
function drupal_clear_path_cache() {
  drupal_lookup_path('wipe');
}

/**
 * Set an HTTP response header for the current page.
 *
 * Note: When sending a Content-Type header, always include a 'charset' type,
 * too. This is necessary to avoid security bugs (e.g. UTF-7 XSS).
 */
function drupal_set_header($header = NULL) {
  // We use an array to guarantee there are no leading or trailing delimiters.
  // Otherwise, header('') could get called when serving the page later, which
  // ends HTTP headers prematurely on some PHP versions.
  static $stored_headers = array();

  if (strlen($header)) {
    header($header);
    $stored_headers[] = $header;
  }
  return implode("\n", $stored_headers);
}

/**
 * Get the HTTP response headers for the current page.
 */
function drupal_get_headers() {
  return drupal_set_header();
}

/**
 * Make any final alterations to the rendered xhtml.
 */
function drupal_final_markup($content) {
  // Make sure that the charset is always specified as the first element of the
  // head region to prevent encoding-based attacks.
  return preg_replace('/<head[^>]*>/i', "\$0\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />", $content, 1);
}

/**
 * Add a feed URL for the current page.
 *
 * @param $url
 *   A URL for the feed.
 * @param $title
 *   The title of the feed.
 */
function drupal_add_feed($url = NULL, $title = '') {
  static $stored_feed_links = array();

  if (!is_null($url) && !isset($stored_feed_links[$url])) {
    $stored_feed_links[$url] = theme('feed_icon', $url, $title);

    drupal_add_link(array('rel' => 'alternate',
                          'type' => 'application/rss+xml',
                          'title' => $title,
                          'href' => url($url, array('absolute' => TRUE))));
  }
  return $stored_feed_links;
}

/**
 * Get the feed URLs for the current page.
 *
 * @param $delimiter
 *   A delimiter to split feeds by.
 */
function drupal_get_feeds($delimiter = "\n") {
  $feeds = drupal_add_feed();
  return implode($feeds, $delimiter);
}

/**
 * @defgroup http_handling HTTP handling
 * @{
 * Functions to properly handle HTTP responses.
 */

/**
 * Processes a URL query parameter array to remove unwanted elements. 
 * Backport from D7.
 *
 * @param $query
 *   (optional) An array to be processed. Defaults to $_GET.
 * @param $exclude
 *   (optional) A list of $query array keys to remove. Use "parent[child]" to
 *   exclude nested items. Defaults to array('q').
 * @param $parent
 *   Internal use only. Used to build the $query array key for nested items.
 *
 * @return
 *   An array containing query parameters, which can be used for url().
 */
function drupal_get_query_parameters(array $query = NULL, array $exclude = array('q'), $parent = '') {
  // Set defaults, if none given.
  if (!isset($query)) {
    $query = $_GET;
  }
  // If $exclude is empty, there is nothing to filter.
  if (empty($exclude)) {
    return $query;
  }
  elseif (!$parent) {
    $exclude = array_flip($exclude);
  }

  $params = array();
  foreach ($query as $key => $value) {
    $string_key = ($parent ? $parent . '[' . $key . ']' : $key);
    if (isset($exclude[$string_key])) {
      continue;
    }

    if (is_array($value)) {
      $params[$key] = drupal_get_query_parameters($value, $exclude, $string_key);
    }
    else {
      $params[$key] = $value;
    }
  }

  return $params;
}

/**
 * Parse an array into a valid urlencoded query string.
 *
 * @param $query
 *   The array to be processed e.g. $_GET.
 * @param $exclude
 *   The array filled with keys to be excluded. Use parent[child] to exclude
 *   nested items.
 * @param $parent
 *   Should not be passed, only used in recursive calls.
 * @return
 *   An urlencoded string which can be appended to/as the URL query string.
 */
function drupal_query_string_encode($query, $exclude = array(), $parent = '') {
  $params = array();

  foreach ($query as $key => $value) {
    $key = rawurlencode($key);
    if ($parent) {
      $key = $parent .'['. $key .']';
    }

    if (in_array($key, $exclude)) {
      continue;
    }

    if (is_array($value)) {
      $params[] = drupal_query_string_encode($value, $exclude, $key);
    }
    else {
      $params[] = $key .'='. rawurlencode($value);
    }
  }

  return implode('&', $params);
}

/**
 * Parses an array into a valid, rawurlencoded query string. Backport from D7.
 *
 * This differs from http_build_query() as we need to rawurlencode() (instead of
 * urlencode()) all query parameters.
 *
 * @param $query
 *   The query parameter array to be processed, e.g. $_GET.
 * @param $parent
 *   Internal use only. Used to build the $query array key for nested items.
 *
 * @return
 *   A rawurlencoded string which can be used as or appended to the URL query
 *   string.
 *
 * @see drupal_get_query_parameters()
 * @ingroup php_wrappers
 */
function drupal_http_build_query(array $query, $parent = '') {
  $params = array();

  foreach ($query as $key => $value) {
    $key = ($parent ? $parent . '[' . rawurlencode($key) . ']' : rawurlencode($key));

    // Recurse into children.
    if (is_array($value)) {
      $params[] = drupal_http_build_query($value, $key);
    }
    // If a query parameter value is NULL, only append its key.
    elseif (!isset($value)) {
      $params[] = $key;
    }
    else {
      // For better readability of paths in query strings, we decode slashes.
      $params[] = $key . '=' . str_replace('%2F', '/', rawurlencode($value));
    }
  }

  return implode('&', $params);
}

/**
 * Prepares a 'destination' URL query parameter for use with drupal_goto().
 * Backport from D7.
 *
 * Used to direct the user back to the referring page after completing a form.
 * By default the current URL is returned. If a destination exists in the
 * previous request, that destination is returned. As such, a destination can
 * persist across multiple pages.
 *
 * @return
 *   An associative array containing the key:
 *   - destination: The path provided via the destination query string or, if
 *     not available, the current path.
 *
 * @see current_path()
 * @see drupal_goto()
 */
function drupal_get_destination() {
  $destination = &drupal_static(__FUNCTION__);

  if (isset($destination)) {
    return $destination;
  }

  if (isset($_GET['destination'])) {
    $destination = array('destination' => $_GET['destination']);
  }
  else {
    $path = $_GET['q'];
    $query = drupal_http_build_query(drupal_get_query_parameters());
    if ($query != '') {
      $path .= '?' . $query;
    }
    $destination = array('destination' => $path);
  }
  return $destination;
}

/**
 * Parses a URL string into its path, query, and fragment components.
 * Backport from D7.
 *
 * This function splits both internal paths like @code node?b=c#d @endcode and
 * external URLs like @code https://example.com/a?b=c#d @endcode into their
 * component parts. See
 * @link http://tools.ietf.org/html/rfc3986#section-3 RFC 3986 @endlink for an
 * explanation of what the component parts are.
 *
 * Note that, unlike the RFC, when passed an external URL, this function
 * groups the scheme, authority, and path together into the path component.
 *
 * @param string $url
 *   The internal path or external URL string to parse.
 *
 * @return array
 *   An associative array containing:
 *   - path: The path component of $url. If $url is an external URL, this
 *     includes the scheme, authority, and path.
 *   - query: An array of query parameters from $url, if they exist.
 *   - fragment: The fragment component from $url, if it exists.
 *
 * @see drupal_goto()
 * @see l()
 * @see url()
 * @see http://tools.ietf.org/html/rfc3986
 *
 * @ingroup php_wrappers
 */
function drupal_parse_url($url) {
  $options = array(
    'path' => NULL,
    'query' => array(),
    'fragment' => '',
  );

  // External URLs: not using parse_url() here, so we do not have to rebuild
  // the scheme, host, and path without having any use for it.
  if (strpos($url, '://') !== FALSE) {
    // Split off everything before the query string into 'path'.
    $parts = explode('?', $url);
    $options['path'] = $parts[0];
    // If there is a query string, transform it into keyed query parameters.
    if (isset($parts[1])) {
      $query_parts = explode('#', $parts[1]);
      parse_str($query_parts[0], $options['query']);
      // Take over the fragment, if there is any.
      if (isset($query_parts[1])) {
        $options['fragment'] = $query_parts[1];
      }
    }
  }
  // Internal URLs.
  else {
    // parse_url() does not support relative URLs, so make it absolute. E.g. the
    // relative URL "foo/bar:1" isn't properly parsed.
    $parts = parse_url('http://example.com/' . $url);
    // Strip the leading slash that was just added.
    $options['path'] = substr($parts['path'], 1);
    if (isset($parts['query'])) {
      parse_str($parts['query'], $options['query']);
    }
    if (isset($parts['fragment'])) {
      $options['fragment'] = $parts['fragment'];
    }
  }
  // The 'q' parameter contains the path of the current page if clean URLs are
  // disabled. It overrides the 'path' of the URL when present, even if clean
  // URLs are enabled, due to how Apache rewriting rules work.
  if (isset($options['query']['q'])) {
    $options['path'] = $options['query']['q'];
    unset($options['query']['q']);
  }

  return $options;
}

/**
 * Encodes a Drupal path for use in a URL. Backport from D7.
 *
 * For aesthetic reasons slashes are not escaped.
 *
 * Note that url() takes care of calling this function, so a path passed to that
 * function should not be encoded in advance.
 *
 * @param $path
 *   The Drupal path to encode.
 */
function drupal_encode_path($path) {
  return str_replace('%2F', '/', rawurlencode($path));
}

/**
 * Sends the user to a different page.
 *
 * This issues an on-site HTTP redirect. The function makes sure the redirected
 * URL is formatted correctly.
 *
 * Usually the redirected URL is constructed from this function's input
 * parameters. However you may override that behavior by setting a
 * destination in either the $_REQUEST-array (i.e. by using
 * the query string of an URI) This is used to direct the user back to
 * the proper page after completing a form. For example, after editing
 * a post on the 'admin/content'-page or after having logged on using the
 * 'user login'-block in a sidebar. The function drupal_get_destination()
 * can be used to help set the destination URL.
 *
 * Drupal will ensure that messages set by drupal_set_message() and other
 * session data are written to the database before the user is redirected.
 *
 * This function ends the request; use it instead of a return in your menu
 * callback.
 *
 * @param $path
 *   (optional) A Drupal path or a full URL, which will be passed to url() to
 *   compute the redirect for the URL.
 * @param $options
 *   (optional) An associative array of additional URL options to pass to url().
 * @param $http_response_code
 *   (optional) The HTTP status code to use for the redirection, defaults to
 *   302. The valid values for 3xx redirection status codes are defined in
 *   @link http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html#sec10.3 RFC 2616 @endlink
 *   and the
 *   @link http://tools.ietf.org/html/draft-reschke-http-status-308-07 draft for the new HTTP status codes: @endlink
 *   - 301: Moved Permanently (the recommended value for most redirects).
 *   - 302: Found (default in Drupal and PHP, sometimes used for spamming search
 *     engines).
 *   - 303: See Other.
 *   - 304: Not Modified.
 *   - 305: Use Proxy.
 *   - 307: Temporary Redirect.
 *
 * @see drupal_get_destination()
 * @see url()
 */
function drupal_goto($path = '', $options = array(), $http_response_code = 302) {
      // D6 Compat:  $path = '', $query = NULL, $fragment = NULL, $http_response_code = 302

  if (is_string($options) || is_string($http_response_code)) {
    $options = array('query' => $options);
    $args = func_get_args();
    if (!empty($args[2])) {
      $options['fragment'] = $args[2];
    }
    if (!empty($args[3])) {
      $http_response_code = $args[3];
    } else {
      $http_response_code = 302;
    }
  }
  // A destination in $_GET always overrides the function arguments.
  // We do not allow absolute URLs to be passed via $_GET, as this can be an attack vector.
  if (isset($_GET['destination']) && !url_is_external($_GET['destination'])) {
    $destination = drupal_parse_url($_GET['destination']);
    $path = $destination['path'];
    $options['query'] = $destination['query'];
    $options['fragment'] = $destination['fragment'];
  }

  drupal_alter('drupal_goto', $path, $options, $http_response_code);

  // The 'Location' HTTP header must be absolute.
  $options['absolute'] = TRUE;

  $url = url($path, $options);

  // Allow modules to react to the end of the page request before redirecting.
  // We do not want this while running update.php.
  if (!defined('MAINTENANCE_MODE') || MAINTENANCE_MODE != 'update') {
    module_invoke_all('exit', $url);
  }

  // We need all session data written to the database before redirecting.
  drupal_session_commit();

  header('Location: '. $url, TRUE, $http_response_code);

  // The "Location" header sends a redirect status code to the HTTP daemon. In
  // some cases this can be wrong, so we make sure none of the code below the
  // drupal_goto() call gets executed upon redirection.
  exit();
}

/**
 * Generates a site off-line message.
 */
function drupal_site_offline() {
  drupal_maintenance_theme();
  drupal_set_header($_SERVER['SERVER_PROTOCOL'] .' 503 Service unavailable');
  drupal_set_title(t('Site off-line'));
  print theme('maintenance_page', filter_xss_admin(variable_get('site_offline_message',
    t('@site is currently under maintenance. We should be back shortly. Thank you for your patience.', array('@site' => variable_get('site_name', 'Drupal'))))));
}

/**
 * Generates a 404 error if the request can not be handled.
 */
function drupal_not_found() {
  drupal_set_header($_SERVER['SERVER_PROTOCOL'] .' 404 Not Found');

  watchdog('page not found', check_plain($_GET['q']), NULL, WATCHDOG_WARNING);

  // Keep old path for reference, and to allow forms to redirect to it.
  if (!isset($_REQUEST['destination'])) {
    // Make sure that the current path is not interpreted as external URL.
    if (!menu_path_is_external($_GET['q'])) {
      $_REQUEST['destination'] = $_GET['q'];
    }
  }

  $path = drupal_get_normal_path(variable_get('site_404', ''));
  if ($path && $path != $_GET['q']) {
    // Set the active item in case there are tabs to display, or other
    // dependencies on the path.
    menu_set_active_item($path);
    $return = menu_execute_active_handler($path);
  }

  if (empty($return) || $return == MENU_NOT_FOUND || $return == MENU_ACCESS_DENIED) {
    drupal_set_title(t('Page not found'));
    $return = t('The requested page could not be found.');
  }

  // To conserve CPU and bandwidth, omit the blocks.
  print theme('page', $return, FALSE);
}

/**
 * Generates a 403 error if the request is not allowed.
 */
function drupal_access_denied() {
  drupal_set_header($_SERVER['SERVER_PROTOCOL'] .' 403 Forbidden');

  watchdog('access denied', check_plain($_GET['q']), NULL, WATCHDOG_WARNING);

  // Keep old path for reference, and to allow forms to redirect to it.
  if (!isset($_GET['destination'])) {
    // Make sure that the current path is not interpreted as external URL.
    if (!menu_path_is_external($_GET['q'])) {
      $_GET['destination'] = $_GET['q'];
    }
  }

  $path = drupal_get_normal_path(variable_get('site_403', ''));
  if ($path && $path != $_GET['q']) {
    // Set the active item in case there are tabs to display or other
    // dependencies on the path.
    menu_set_active_item($path);
    $return = menu_execute_active_handler($path);
  }

  if (empty($return) || $return == MENU_NOT_FOUND || $return == MENU_ACCESS_DENIED) {
    drupal_set_title(t('Access denied'));
    $return = t('You are not authorized to access this page.');
  }
  print theme('page', $return);
}

/**
 * Perform an HTTP request.
 *
 * This is a flexible and powerful HTTP client implementation. Correctly handles
 * GET, POST, PUT or any other HTTP requests. Handles redirects.
 *
 * @param $url
 *   A string containing a fully qualified URI.
 * @param $headers
 *   An array containing an HTTP header => value pair.
 * @param $method
 *   A string defining the HTTP request to use.
 * @param $data
 *   A string containing data to include in the request.
 * @param $retry
 *   An integer representing how many times to retry the request in case of a
 *   redirect.
 * @param $timeout
 *   A float representing the maximum number of seconds the function call may
 *   take. The default is 30 seconds. If a timeout occurs, the error code is set
 *   to the HTTP_REQUEST_TIMEOUT constant.
 * @return
 *   An object containing the HTTP request headers, response code, protocol,
 *   status message, headers, data and redirect status.
 */
function drupal_http_request($url, $headers = array(), $method = 'GET', $data = NULL, $retry = 3, $timeout = 30.0) {
  global $db_prefix;

  $result = new stdClass();

  // Parse the URL and make sure we can handle the schema.
  $uri = parse_url($url);

  if ($uri == FALSE) {
    $result->error = 'unable to parse URL';
    $result->code = -1001;
    return $result;
  }

  if (!isset($uri['scheme'])) {
    $result->error = 'missing schema';
    $result->code = -1002;
    return $result;
  }

  timer_start(__FUNCTION__);

  switch ($uri['scheme']) {
    case 'http':
    case 'feed':
      $port = isset($uri['port']) ? $uri['port'] : 80;
      $host = $uri['host'] . ($port != 80 ? ':'. $port : '');
      $fp = @fsockopen($uri['host'], $port, $errno, $errstr, $timeout);
      break;
    case 'https':
      // Note: Only works for PHP 4.3 compiled with OpenSSL.
      $port = isset($uri['port']) ? $uri['port'] : 443;
      $host = $uri['host'] . ($port != 443 ? ':'. $port : '');
      $fp = @fsockopen('ssl://'. $uri['host'], $port, $errno, $errstr, $timeout);
      break;
    default:
      $result->error = 'invalid schema '. $uri['scheme'];
      $result->code = -1003;
      return $result;
  }

  // Make sure the socket opened properly.
  if (!$fp) {
    // When a network error occurs, we use a negative number so it does not
    // clash with the HTTP status codes.
    $result->code = -$errno;
    $result->error = trim($errstr);

    // Mark that this request failed. This will trigger a check of the web
    // server's ability to make outgoing HTTP requests the next time that
    // requirements checking is performed.
    // @see system_requirements()
    variable_set('drupal_http_request_fails', TRUE);

    return $result;
  }

  // Construct the path to act on.
  $path = isset($uri['path']) ? $uri['path'] : '/';
  if (isset($uri['query'])) {
    $path .= '?'. $uri['query'];
  }

  // Create HTTP request.
  $defaults = array(
    // RFC 2616: "non-standard ports MUST, default ports MAY be included".
    // We don't add the port to prevent from breaking rewrite rules checking the
    // host that do not take into account the port number.
    'Host' => "Host: $host",
    'User-Agent' => 'User-Agent: Drupal (+http://drupal.org/)',
  );

  // Only add Content-Length if we actually have any content or if it is a POST
  // or PUT request. Some non-standard servers get confused by Content-Length in
  // at least HEAD/GET requests, and Squid always requires Content-Length in
  // POST/PUT requests.
  $content_length = strlen($data);
  if ($content_length > 0 || $method == 'POST' || $method == 'PUT') {
    $defaults['Content-Length'] = 'Content-Length: '. $content_length;
  }

  // If the server URL has a user then attempt to use basic authentication
  if (isset($uri['user'])) {
    $defaults['Authorization'] = 'Authorization: Basic '. base64_encode($uri['user'] . (!empty($uri['pass']) ? ":". $uri['pass'] : ''));
  }

  // If the database prefix is being used by SimpleTest to run the tests in a copied
  // database then set the user-agent header to the database prefix so that any
  // calls to other Drupal pages will run the SimpleTest prefixed database. The
  // user-agent is used to ensure that multiple testing sessions running at the
  // same time won't interfere with each other as they would if the database
  // prefix were stored statically in a file or database variable.
  if (is_string($db_prefix) && preg_match("/^simpletest\d+$/", $db_prefix, $matches)) {
    $defaults['User-Agent'] = 'User-Agent: ' . drupal_generate_test_ua($matches[0]);
  }

  foreach ($headers as $header => $value) {
    $defaults[$header] = $header .': '. $value;
  }

  $request = $method .' '. $path ." HTTP/1.0\r\n";
  $request .= implode("\r\n", $defaults);
  $request .= "\r\n\r\n";
  $request .= $data;

  $result->request = $request;

  // Calculate how much time is left of the original timeout value.
  $time_left = $timeout - timer_read(__FUNCTION__) / 1000;
  if ($time_left > 0) {
    stream_set_timeout($fp, floor($time_left), floor(1000000 * fmod($time_left, 1)));
    fwrite($fp, $request);
  }

  // Fetch response.
  $response = '';
  while (!feof($fp)) {
    // Calculate how much time is left of the original timeout value.
    $time_left = $timeout - timer_read(__FUNCTION__) / 1000;
    if ($time_left <= 0) {
      $result->code = HTTP_REQUEST_TIMEOUT;
      $result->error = 'request timed out';
      return $result;
    }
    stream_set_timeout($fp, floor($time_left), floor(1000000 * fmod($time_left, 1)));
    $chunk = fread($fp, 1024);
    $response .= $chunk;
  }
  fclose($fp);

  // Parse response headers from the response body.
  // Be tolerant of malformed HTTP responses that separate header and body with
  // \n\n or \r\r instead of \r\n\r\n.  See http://drupal.org/node/183435
  list($split, $result->data) = preg_split("/\r\n\r\n|\n\n|\r\r/", $response, 2);
  $split = preg_split("/\r\n|\n|\r/", $split);

  list($protocol, $code, $status_message) = explode(' ', trim(array_shift($split)), 3);
  $result->protocol = $protocol;
  $result->status_message = $status_message;

  $result->headers = array();

  // Parse headers.
  while ($line = trim(array_shift($split))) {
    list($header, $value) = explode(':', $line, 2);
    if (isset($result->headers[$header]) && $header == 'Set-Cookie') {
      // RFC 2109: the Set-Cookie response header comprises the token Set-
      // Cookie:, followed by a comma-separated list of one or more cookies.
      $result->headers[$header] .= ','. trim($value);
    }
    else {
      $result->headers[$header] = trim($value);
    }
  }

  $responses = array(
    100 => 'Continue', 101 => 'Switching Protocols',
    200 => 'OK', 201 => 'Created', 202 => 'Accepted', 203 => 'Non-Authoritative Information', 204 => 'No Content', 205 => 'Reset Content', 206 => 'Partial Content',
    300 => 'Multiple Choices', 301 => 'Moved Permanently', 302 => 'Found', 303 => 'See Other', 304 => 'Not Modified', 305 => 'Use Proxy', 307 => 'Temporary Redirect',
    400 => 'Bad Request', 401 => 'Unauthorized', 402 => 'Payment Required', 403 => 'Forbidden', 404 => 'Not Found', 405 => 'Method Not Allowed', 406 => 'Not Acceptable', 407 => 'Proxy Authentication Required', 408 => 'Request Time-out', 409 => 'Conflict', 410 => 'Gone', 411 => 'Length Required', 412 => 'Precondition Failed', 413 => 'Request Entity Too Large', 414 => 'Request-URI Too Large', 415 => 'Unsupported Media Type', 416 => 'Requested range not satisfiable', 417 => 'Expectation Failed',
    500 => 'Internal Server Error', 501 => 'Not Implemented', 502 => 'Bad Gateway', 503 => 'Service Unavailable', 504 => 'Gateway Time-out', 505 => 'HTTP Version not supported'
  );
  // RFC 2616 states that all unknown HTTP codes must be treated the same as the
  // base code in their class.
  if (!isset($responses[$code])) {
    $code = floor($code / 100) * 100;
  }

  switch ($code) {
    case 200: // OK
    case 304: // Not modified
      break;
    case 301: // Moved permanently
    case 302: // Moved temporarily
    case 307: // Moved temporarily
      $location = $result->headers['Location'];
      $timeout -= timer_read(__FUNCTION__) / 1000;
      if ($timeout <= 0) {
        $result->code = HTTP_REQUEST_TIMEOUT;
        $result->error = 'request timed out';
      }
      elseif ($retry) {
        $result = drupal_http_request($result->headers['Location'], $headers, $method, $data, --$retry, $timeout);
        $result->redirect_code = $result->code;
      }
      $result->redirect_url = $location;

      break;
    default:
      $result->error = $status_message;
  }

  $result->code = $code;
  return $result;
}
/**
 * @} End of "HTTP handling".
 */

/**
 * Log errors as defined by administrator.
 *
 * Error levels:
 * - 0 = Log errors to database.
 * - 1 = Log errors to database and to screen.
 */
function drupal_error_handler($errno, $message, $filename, $line, $context) {
  // If the @ error suppression operator was used, error_reporting will have
  // been temporarily set to 0.
  if (error_reporting() == 0) {
    return;
  }

  if ($errno & (E_ALL ^ E_DEPRECATED)) {
    $types = array(1 => 'error', 2 => 'warning', 4 => 'parse error', 8 => 'notice', 16 => 'core error', 32 => 'core warning', 64 => 'compile error', 128 => 'compile warning', 256 => 'user error', 512 => 'user warning', 1024 => 'user notice', 2048 => 'strict warning', 4096 => 'recoverable fatal error');

    // For database errors, we want the line number/file name of the place that
    // the query was originally called, not _db_query().
    if (isset($context[DB_ERROR])) {
      $backtrace = array_reverse(debug_backtrace());

      // List of functions where SQL queries can originate.
      $query_functions = array('db_query', 'pager_query', 'db_query_range', 'db_query_temporary', 'update_sql');

      // Determine where query function was called, and adjust line/file
      // accordingly.
      foreach ($backtrace as $index => $function) {
        if (in_array($function['function'], $query_functions)) {
          $line = $backtrace[$index]['line'];
          $filename = $backtrace[$index]['file'];
          break;
        }
      }
    }

    // Try to use filter_xss(). If it's too early in the bootstrap process for
    // filter_xss() to be loaded, use check_plain() instead.
    $entry = check_plain($types[$errno]) .': '. (function_exists('filter_xss') ? filter_xss($message) : check_plain($message)) .' in '. check_plain($filename) .' on line '. check_plain($line) .'.';

    // Force display of error messages in update.php.
    if (variable_get('error_level', 1) == 1 || strstr($_SERVER['SCRIPT_NAME'], 'update.php')) {
      drupal_set_message($entry, 'error');
    }

    watchdog('php', '%message in %file on line %line.', array('%error' => $types[$errno], '%message' => $message, '%file' => $filename, '%line' => $line), WATCHDOG_ERROR);
  }
}

function _fix_gpc_magic(&$item) {
  if (is_array($item)) {
    array_walk($item, '_fix_gpc_magic');
  }
  else {
    $item = stripslashes($item);
  }
}

/**
 * Helper function to strip slashes from $_FILES skipping over the tmp_name keys
 * since PHP generates single backslashes for file paths on Windows systems.
 *
 * tmp_name does not have backslashes added see
 * http://php.net/manual/en/features.file-upload.php#42280
 */
function _fix_gpc_magic_files(&$item, $key) {
  if ($key != 'tmp_name') {
    if (is_array($item)) {
      array_walk($item, '_fix_gpc_magic_files');
    }
    else {
      $item = stripslashes($item);
    }
  }
}

/**
 * Fix double-escaping problems caused by "magic quotes" in some PHP installations.
 */
function fix_gpc_magic() {
  static $fixed = FALSE;
  if (!$fixed && ini_get('magic_quotes_gpc')) {
    array_walk($_GET, '_fix_gpc_magic');
    array_walk($_POST, '_fix_gpc_magic');
    array_walk($_COOKIE, '_fix_gpc_magic');
    array_walk($_REQUEST, '_fix_gpc_magic');
    array_walk($_FILES, '_fix_gpc_magic_files');
    $fixed = TRUE;
  }
}

/**
 * Translate strings to the page language or a given language.
 *
 * Human-readable text that will be displayed somewhere within a page should
 * be run through the t() function.
 *
 * Examples:
 * @code
 *   if (!$info || !$info['extension']) {
 *     form_set_error('picture_upload', t('The uploaded file was not an image.'));
 *   }
 *
 *   $form['submit'] = array(
 *     '#type' => 'submit',
 *     '#value' => t('Log in'),
 *   );
 * @endcode
 *
 * Any text within t() can be extracted by translators and changed into
 * the equivalent text in their native language.
 *
 * Special variables called "placeholders" are used to signal dynamic
 * information in a string which should not be translated. Placeholders
 * can also be used for text that may change from time to time (such as
 * link paths) to be changed without requiring updates to translations.
 *
 * For example:
 * @code
 *   $output = t('There are currently %members and %visitors online.', array(
 *     '%members' => format_plural($total_users, '1 user', '@count users'),
 *     '%visitors' => format_plural($guests->count, '1 guest', '@count guests')));
 * @endcode
 *
 * There are three styles of placeholders:
 * - !variable, which indicates that the text should be inserted as-is. This is
 *   useful for inserting variables into things like e-mail.
 *   @code
 *     $message[] = t("If you don't want to receive such e-mails, you can change your settings at !url.", array('!url' => url("user/$account->uid", array('absolute' => TRUE))));
 *   @endcode
 *
 * - @variable, which indicates that the text should be run through
 *   check_plain, to escape HTML characters. Use this for any output that's
 *   displayed within a Drupal page.
 *   @code
 *     drupal_set_title($title = t("@name's blog", array('@name' => $account->name)));
 *   @endcode
 *
 * - %variable, which indicates that the string should be HTML escaped and
 *   highlighted with theme_placeholder() which shows up by default as
 *   <em>emphasized</em>.
 *   @code
 *     $message = t('%name-from sent %name-to an e-mail.', array('%name-from' => $user->name, '%name-to' => $account->name));
 *   @endcode
 *
 * When using t(), try to put entire sentences and strings in one t() call.
 * This makes it easier for translators, as it provides context as to what
 * each word refers to. HTML markup within translation strings is allowed, but
 * should be avoided if possible. The exception are embedded links; link
 * titles add a context for translators, so should be kept in the main string.
 *
 * Here is an example of incorrect usage of t():
 * @code
 *   $output .= t('<p>Go to the @contact-page.</p>', array('@contact-page' => l(t('contact page'), 'contact')));
 * @endcode
 *
 * Here is an example of t() used correctly:
 * @code
 *   $output .= '<p>'. t('Go to the <a href="@contact-page">contact page</a>.', array('@contact-page' => url('contact'))) .'</p>';
 * @endcode
 *
 * Avoid escaping quotation marks wherever possible.
 *
 * Incorrect:
 * @code
 *   $output .= t('Don\'t click me.');
 * @endcode
 *
 * Correct:
 * @code
 *   $output .= t("Don't click me.");
 * @endcode
 *
 * Because t() is designed for handling code-based strings, in almost all
 * cases, the actual string and not a variable must be passed through t().
 *
 * Extraction of translations is done based on the strings contained in t()
 * calls. If a variable is passed through t(), the content of the variable
 * cannot be extracted from the file for translation.
 *
 * Incorrect:
 * @code
 *   $message = 'An error occurred.';
 *   drupal_set_message(t($message), 'error');
 *   $output .= t($message);
 * @endcode
 *
 * Correct:
 * @code
 *   $message = t('An error occurred.');
 *   drupal_set_message($message, 'error');
 *   $output .= $message;
 * @endcode
 *
 * The only case in which variables can be passed safely through t() is when
 * code-based versions of the same strings will be passed through t() (or
 * otherwise extracted) elsewhere.
 *
 * In some cases, modules may include strings in code that can't use t()
 * calls. For example, a module may use an external PHP application that
 * produces strings that are loaded into variables in Drupal for output.
 * In these cases, module authors may include a dummy file that passes the
 * relevant strings through t(). This approach will allow the strings to be
 * extracted.
 *
 * Sample external (non-Drupal) code:
 * @code
 *   class Time {
 *     public $yesterday = 'Yesterday';
 *     public $today = 'Today';
 *     public $tomorrow = 'Tomorrow';
 *   }
 * @endcode
 *
 * Sample dummy file.
 * @code
 *   // Dummy function included in example.potx.inc.
 *   function example_potx() {
 *     $strings = array(
 *       t('Yesterday'),
 *       t('Today'),
 *       t('Tomorrow'),
 *     );
 *     // No return value needed, since this is a dummy function.
 *   }
 * @endcode
 *
 * Having passed strings through t() in a dummy function, it is then
 * okay to pass variables through t().
 *
 * Correct (if a dummy file was used):
 * @code
 *   $time = new Time();
 *   $output .= t($time->today);
 * @endcode
 *
 * However tempting it is, custom data from user input or other non-code
 * sources should not be passed through t(). Doing so leads to the following
 * problems and errors:
 *  - The t() system doesn't support updates to existing strings. When user
 *    data is updated, the next time it's passed through t() a new record is
 *    created instead of an update. The database bloats over time and any
 *    existing translations are orphaned with each update.
 *  - The t() system assumes any data it receives is in English. User data may
 *    be in another language, producing translation errors.
 *  - The "Built-in interface" text group in the locale system is used to
 *    produce translations for storage in .po files. When non-code strings are
 *    passed through t(), they are added to this text group, which is rendered
 *    inaccurate since it is a mix of actual interface strings and various user
 *    input strings of uncertain origin.
 *
 * Incorrect:
 * @code
 *   $item = item_load();
 *   $output .= check_plain(t($item['title']));
 * @endcode
 *
 * Instead, translation of these data can be done through the locale system,
 * either directly or through helper functions provided by contributed
 * modules.
 * @see hook_locale()
 *
 * During installation, st() is used in place of t(). Code that may be called
 * during installation or during normal operation should use the get_t()
 * helper function.
 * @see st()
 * @see get_t()
 *
 * @param $string
 *   A string containing the English string to translate.
 * @param $args
 *   An associative array of replacements to make after translation. Incidences
 *   of any key in this array are replaced with the corresponding value. Based
 *   on the first character of the key, the value is escaped and/or themed:
 *    - !variable: inserted as is
 *    - @variable: escape plain text to HTML (check_plain)
 *    - %variable: escape text and theme as a placeholder for user-submitted
 *      content (check_plain + theme_placeholder)
 * @param $langcode
 *   Optional language code to translate to a language other than what is used
 *   to display the page.
 * @param $context
 *   Optional context which will be used to translate string. Used by 
 *   format_date() function to translate long month format names
 * @return
 *   The translated string.
 */
function t($string, $args = array(), $langcode = NULL, $context = '') {
  global $language;
  $custom_strings = &drupal_static(__FUNCTION__, array());

  $langcode = isset($langcode) ? $langcode : $language->language;

  // First, check for an array of customized strings. If present, use the array
  // *instead of* database lookups. This is a high performance way to provide a
  // handful of string replacements. See settings.php for examples.
  // Cache the $custom_strings variable to improve performance.
  if (!isset($custom_strings[$langcode])) {
    $custom_strings[$langcode] = variable_get('locale_custom_strings_'. $langcode, array());
    if (!isset($custom_strings[$langcode][''])) {
      // D6 compatibility, setting empty context if it's not present
      $custom_strings[$langcode][''] = $custom_strings[$langcode];
    }
  }
  // Custom strings work for English too, even if locale module is disabled.
  if (isset($custom_strings[$langcode][$context][$string])) {
    $string = $custom_strings[$langcode][$context][$string];
  }
  // Translate with locale module if enabled.
  elseif (function_exists('locale') && $langcode != 'en') {
    $string = locale($string, $langcode);
  }
  if (empty($args)) {
    return $string;
  }
  else {
    return format_string($string, $args);
  }
}

/**
 * @defgroup validation Input validation
 * @{
 * Functions to validate user input.
 */

/**
 * Verifies the syntax of the given e-mail address.
 *
 * See @link http://tools.ietf.org/html/rfc5322 RFC 5322 @endlink for details.
 *
 * @param $mail
 *   A string containing an e-mail address.
 * @return
 *   1 if the email address is valid, 0 if it is invalid or empty, and FALSE if
 *   there is an input error (such as passing in an array instead of a string).
 */
function valid_email_address($mail) {
  if (function_exists('filter_var')) {
    return (bool)filter_var($mail, FILTER_VALIDATE_EMAIL);
  }

  $user = '[a-zA-Z0-9_\-\.\+\^!#\$%&*+\/\=\?\`\|\{\}~\']+';
  $domain = '(?:(?:[a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]*[a-zA-Z0-9])\.?)+';
  $ipv4 = '[0-9]{1,3}(\.[0-9]{1,3}){3}';
  $ipv6 = '[0-9a-fA-F]{1,4}(\:[0-9a-fA-F]{1,4}){7}';

  return preg_match("/^$user@($domain|(\[($ipv4|$ipv6)\]))$/", $mail);
}

/**
 * Verify the syntax of the given URL.
 *
 * This function should only be used on actual URLs. It should not be used for
 * Drupal menu paths, which can contain arbitrary characters.
 * Valid values per RFC 3986.
 *
 * @param $url
 *   The URL to verify.
 * @param $absolute
 *   Whether the URL is absolute (beginning with a scheme such as "http:").
 * @return
 *   TRUE if the URL is in a valid format.
 */
function valid_url($url, $absolute = FALSE) {
  if ($absolute) {
    return (bool)preg_match("
      /^                                                      # Start at the beginning of the text
      (?:ftp|https?|feed):\/\/                                # Look for ftp, http, https or feed schemes
      (?:                                                     # Userinfo (optional) which is typically
        (?:(?:[\w\.\-\+!$&'\(\)*\+,;=]|%[0-9a-f]{2})+:)*      # a username or a username and password
        (?:[\w\.\-\+%!$&'\(\)*\+,;=]|%[0-9a-f]{2})+@          # combination
      )?
      (?:
        (?:[a-z0-9\-\.]|%[0-9a-f]{2})+                        # A domain name or a IPv4 address
        |(?:\[(?:[0-9a-f]{0,4}:)*(?:[0-9a-f]{0,4})\])         # or a well formed IPv6 address
      )
      (?::[0-9]+)?                                            # Server port number (optional)
      (?:[\/|\?]
        (?:[\w#!:\.\?\+=&@$'~*,;\/\(\)\[\]\-]|%[0-9a-f]{2})   # The path and query (optional)
      *)?
    $/xi", $url);
  }
  else {
    return (bool)preg_match("/^(?:[\w#!:\.\?\+=&@$'~*,;\/\(\)\[\]\-]|%[0-9a-f]{2})+$/i", $url);
  }
}


/**
 * @} End of "defgroup validation".
 */

/**
 * Helper function for flood event hostname processing.
 *
 * @param $identifier
 *   Optional identifier (defaults to the current user's IP address).
 * @return
 *   User's IP address if no identifier provided. Otherwise returns a long-strings-safe one.
 */
function flood_get_identifier($identifier = NULL) {
  if (empty($identifier)) {
    $identifier = ip_address();
  }
  else if(strlen($identifier) > 128) {
    // Make a hash from identifier if it's longer than 128 chars.
    $identifier = md5($identifier);
  }
  return $identifier;
}

/**
 * Register an event for the current visitor (hostname/IP) to the flood control mechanism.
 *
 * @param $name
 *   The name of an event.
 * @param $identifier
 *   Optional identifier (defaults to the current user's IP address).
 */
function flood_register_event($name, $identifier = NULL) {
  db_query("INSERT INTO {flood} (event, hostname, timestamp) VALUES ('%s', '%s', %d)", $name, flood_get_identifier($identifier), time());
}

/**
 * Check if the current visitor (hostname/IP) is allowed to proceed with the specified event.
 *
 * The user is allowed to proceed if he did not trigger the specified event more
 * than $threshold times per hour.
 *
 * @param $name
 *   The name of the event.
 * @param $threshold
 *   The maximum number of the specified event per hour (per visitor).
 * @param $identifier
 *   Optional identifier (defaults to the current user's IP address).
 * @return
 *   True if the user did not exceed the hourly threshold. False otherwise.
 */
function flood_is_allowed($name, $threshold, $identifier = NULL) {
  $number = db_result(db_query("SELECT COUNT(*) FROM {flood} WHERE event = '%s' AND hostname = '%s' AND timestamp > %d", $name, flood_get_identifier($identifier), time() - 3600));
  return ($number < $threshold ? TRUE : FALSE);
}

/**
 * Makes the flood control mechanism forget an event for the current visitor.
 *
 * @param $name
 *   The name of an event.
 * @param $identifier
 *   Optional identifier (defaults to the current user's IP address).
 */
function flood_clear_event($name, $identifier = NULL) {
  db_query("DELETE FROM {flood} WHERE event = '%s' AND hostname = '%s'", $name, flood_get_identifier($identifier));
}

function check_file($filename) {
  return is_uploaded_file($filename);
}

/**
 * Strips dangerous protocols (e.g. 'javascript:') from a URI. Backport from D7.
 *
 * This function must be called for all URIs within user-entered input prior
 * to being output to an HTML attribute value. It is often called as part of
 * check_url() or filter_xss(), but those functions return an HTML-encoded
 * string, so this function can be called independently when the output needs to
 * be a plain-text string for passing to t(), l(), drupal_attributes(), or
 * another function that will call check_plain() separately.
 *
 * @param $uri
 *   A plain-text URI that might contain dangerous protocols.
 *
 * @return
 *   A plain-text URI stripped of dangerous protocols. As with all plain-text
 *   strings, this return value must not be output to an HTML page without
 *   check_plain() being called on it. However, it can be passed to functions
 *   expecting plain-text strings.
 *
 * @see check_url()
 */
function drupal_strip_dangerous_protocols($uri) {
  static $allowed_protocols;

  if (!isset($allowed_protocols)) {
    $allowed_protocols = array_flip(variable_get('filter_allowed_protocols', array('ftp', 'http', 'https', 'irc', 'mailto', 'news', 'nntp', 'rtsp', 'sftp', 'ssh', 'tel', 'telnet', 'webcal')));
  }

  // Iteratively remove any invalid protocol found.
  do {
    $before = $uri;
    $colonpos = strpos($uri, ':');
    if ($colonpos > 0) {
      // We found a colon, possibly a protocol. Verify.
      $protocol = substr($uri, 0, $colonpos);
      // If a colon is preceded by a slash, question mark or hash, it cannot
      // possibly be part of the URL scheme. This must be a relative URL, which
      // inherits the (safe) protocol of the base document.
      if (preg_match('![/?#]!', $protocol)) {
        break;
      }
      // Check if this is a disallowed protocol. Per RFC2616, section 3.2.3
      // (URI Comparison) scheme comparison must be case-insensitive.
      if (!isset($allowed_protocols[strtolower($protocol)])) {
        $uri = substr($uri, $colonpos + 1);
      }
    }
  } while ($before != $uri);

  return $uri;
}

/**
 * Prepare a URL for use in an HTML attribute. Strips harmful protocols.
 */
function check_url($uri) {
  return filter_xss_bad_protocol($uri, FALSE);
}

/**
 * @defgroup format Formatting
 * @{
 * Functions to format numbers, strings, dates, etc.
 */

/**
 * Formats an RSS channel.
 *
 * Arbitrary elements may be added using the $args associative array.
 */
function format_rss_channel($title, $link, $description, $items, $langcode = NULL, $args = array()) {
  global $language;
  $langcode = $langcode ? $langcode : $language->language;

  $output = "<channel>\n";
  $output .= ' <title>'. check_plain($title) ."</title>\n";
  $output .= ' <link>'. check_url($link) ."</link>\n";

  // The RSS 2.0 "spec" doesn't indicate HTML can be used in the description.
  // We strip all HTML tags, but need to prevent double encoding from properly
  // escaped source data (such as &amp becoming &amp;amp;).
  $output .= ' <description>'. check_plain(decode_entities(strip_tags($description))) ."</description>\n";
  $output .= ' <language>'. check_plain($langcode) ."</language>\n";
  $output .= format_xml_elements($args);
  $output .= $items;
  $output .= "</channel>\n";

  return $output;
}

/**
 * Format a single RSS item.
 *
 * Arbitrary elements may be added using the $args associative array.
 */
function format_rss_item($title, $link, $description, $args = array()) {
  $output = "<item>\n";
  $output .= ' <title>'. check_plain($title) ."</title>\n";
  $output .= ' <link>'. check_url($link) ."</link>\n";
  $output .= ' <description>'. check_plain($description) ."</description>\n";
  $output .= format_xml_elements($args);
  $output .= "</item>\n";

  return $output;
}

/**
 * Format XML elements.
 *
 * @param $array
 *   An array where each item represent an element and is either a:
 *   - (key => value) pair (<key>value</key>)
 *   - Associative array with fields:
 *     - 'key': element name
 *     - 'value': element contents
 *     - 'attributes': associative array of element attributes
 *
 * In both cases, 'value' can be a simple string, or it can be another array
 * with the same format as $array itself for nesting.
 */
function format_xml_elements($array) {
  $output = '';
  foreach ($array as $key => $value) {
    if (is_numeric($key)) {
      if ($value['key']) {
        $output .= ' <'. $value['key'];
        if (isset($value['attributes']) && is_array($value['attributes'])) {
          $output .= drupal_attributes($value['attributes']);
        }

        if (isset($value['value']) && $value['value'] != '') {
          $output .= '>'. (is_array($value['value']) ? format_xml_elements($value['value']) : check_plain($value['value'])) .'</'. $value['key'] .">\n";
        }
        else {
          $output .= " />\n";
        }
      }
    }
    else {
      $output .= ' <'. $key .'>'. (is_array($value) ? format_xml_elements($value) : check_plain($value)) ."</$key>\n";
    }
  }
  return $output;
}

/**
 * Format a string containing a count of items.
 *
 * This function ensures that the string is pluralized correctly. Since t() is
 * called by this function, make sure not to pass already-localized strings to
 * it.
 *
 * For example:
 * @code
 *   $output = format_plural($node->comment_count, '1 comment', '@count comments');
 * @endcode
 *
 * Example with additional replacements:
 * @code
 *   $output = format_plural($update_count,
 *     'Changed the content type of 1 post from %old-type to %new-type.',
 *     'Changed the content type of @count posts from %old-type to %new-type.',
 *     array('%old-type' => $info->old_type, '%new-type' => $info->new_type)));
 * @endcode
 *
 * @param $count
 *   The item count to display.
 * @param $singular
 *   The string for the singular case. Please make sure it is clear this is
 *   singular, to ease translation (e.g. use "1 new comment" instead of "1 new").
 *   Do not use @count in the singular string.
 * @param $plural
 *   The string for the plural case. Please make sure it is clear this is plural,
 *   to ease translation. Use @count in place of the item count, as in "@count
 *   new comments".
 * @param $args
 *   An associative array of replacements to make after translation. Incidences
 *   of any key in this array are replaced with the corresponding value.
 *   Based on the first character of the key, the value is escaped and/or themed:
 *    - !variable: inserted as is
 *    - @variable: escape plain text to HTML (check_plain)
 *    - %variable: escape text and theme as a placeholder for user-submitted
 *      content (check_plain + theme_placeholder)
 *   Note that you do not need to include @count in this array.
 *   This replacement is done automatically for the plural case.
 * @param $langcode
 *   Optional language code to translate to a language other than
 *   what is used to display the page.
 * @return
 *   A translated string.
 */
function format_plural($count, $singular, $plural, $args = array(), $langcode = NULL) {
  $args['@count'] = $count;
  if ($count == 1) {
    return t($singular, $args, $langcode);
  }

  // Get the plural index through the gettext formula.
  $index = (function_exists('locale_get_plural')) ? locale_get_plural($count, $langcode) : -1;
  // Backwards compatibility.
  if ($index < 0) {
    return t($plural, $args, $langcode);
  }
  else {
    switch ($index) {
      case "0":
        return t($singular, $args, $langcode);
      case "1":
        return t($plural, $args, $langcode);
      default:
        unset($args['@count']);
        $args['@count['. $index .']'] = $count;
        return t(strtr($plural, array('@count' => '@count['. $index .']')), $args, $langcode);
    }
  }
}

/**
 * Parses a given byte count. Backport from D7.
 *
 * @param $size
 *   A size expressed as a number of bytes with optional SI or IEC binary unit
 *   prefix (e.g. 2, 3K, 5MB, 10G, 6GiB, 8 bytes, 9mbytes).
 *
 * @return
 *   An integer representation of the size in bytes.
 */
function parse_size($size) {
  $unit = preg_replace('/[^bkmgtpezy]/i', '', $size); // Remove the non-unit characters from the size.
  $size = preg_replace('/[^0-9\.]/', '', $size); // Remove the non-numeric characters from the size.
  if ($unit) {
    // Find the position of the unit in the ordered string which is the power of magnitude to multiply a kilobyte by.
    return round($size * pow(DRUPAL_KILOBYTE, stripos('bkmgtpezy', $unit[0])));
  }
  else {
    return round($size);
  }
}

/**
 * Generates a string representation for the given byte count. Backport from D7.
 *
 * @param $size
 *   A size in bytes.
 * @param $langcode
 *   Optional language code to translate to a language other than what is used
 *   to display the page.
 *
 * @return
 *   A translated string representation of the size.
 */
function format_size($size, $langcode = NULL) {
  if ($size < DRUPAL_KILOBYTE) {
    return format_plural($size, '1 byte', '@count bytes', array(), $langcode);
  }
  else {
    $size = $size / DRUPAL_KILOBYTE; // Convert bytes to kilobytes.
    $units = array(
      t('@size KB', array(), $langcode),
      t('@size MB', array(), $langcode),
      t('@size GB', array(), $langcode),
      t('@size TB', array(), $langcode),
      t('@size PB', array(), $langcode),
      t('@size EB', array(), $langcode),
      t('@size ZB', array(), $langcode),
      t('@size YB', array(), $langcode),
    );
    foreach ($units as $unit) {
      if (round($size, 2) >= DRUPAL_KILOBYTE) {
        $size = $size / DRUPAL_KILOBYTE;
      }
      else {
        break;
      }
    }
    return str_replace('@size', round($size, 2), $unit);
  }
}

/**
 * Formats a time interval with the requested granularity. Backport from D7.
 *
 * @param $interval
 *   The length of the interval in seconds.
 * @param $granularity
 *   How many different units to display in the string.
 * @param $langcode
 *   Optional language code to translate to a language other than
 *   what is used to display the page.
 *
 * @return
 *   A translated string representation of the interval.
 */
function format_interval($interval, $granularity = 2, $langcode = NULL) {
  $units = array(
    '1 year|@count years' => 31536000,
    '1 month|@count months' => 2592000,
    '1 week|@count weeks' => 604800,
    '1 day|@count days' => 86400,
    '1 hour|@count hours' => 3600,
    '1 min|@count min' => 60,
    '1 sec|@count sec' => 1
  );
  $output = '';
  foreach ($units as $key => $value) {
    $key = explode('|', $key);
    if ($interval >= $value) {
      $output .= ($output ? ' ' : '') . format_plural(floor($interval / $value), $key[0], $key[1], array(), $langcode);
      $interval %= $value;
      $granularity--;
    }

    if ($granularity == 0) {
      break;
    }
  }
  return $output ? $output : t('0 sec', array(), $langcode);
}

/**
 * Formats a date, using a date type or a custom date format string.
 * Backport from D7.
 *
 * @param $timestamp
 *   A UNIX timestamp to format.
 * @param $type
 *   (optional) The format to use, one of:
 *   - 'short', 'medium', or 'long' (the corresponding built-in date formats).
 *   - The name of a date type defined by a module in hook_date_format_types(),
 *     if it's been assigned a format.
 *   - The machine name of an administrator-defined date format.
 *   - 'custom', to use $format.
 *   Defaults to 'medium'.
 * @param $format
 *   (optional) If $type is 'custom', a PHP date format string suitable for
 *   input to date(). Use a backslash to escape ordinary text, so it does not
 *   get interpreted as date format characters.
 * @param $timezone
 *   (optional) Time zone identifier, as described at
 *   http://php.net/manual/timezones.php Defaults to the time zone used to
 *   display the page.
 *   Numeric TZ offset as in D6 works either.
 * @param $langcode
 *   (optional) Language code to translate to. Defaults to the language used to
 *   display the page.
 *
 * @return
 *   A translated date string in the requested format.
 */
function format_date($timestamp, $type = 'medium', $format = '', $timezone = NULL, $langcode = NULL) {
  // Use the advanced drupal_static() pattern, since this is called very often.
  static $drupal_static_fast;
  if (!isset($drupal_static_fast)) {
    $drupal_static_fast['timezones'] = &drupal_static(__FUNCTION__);
  }

  if (!is_numeric($timezone)) {
    $timezones = &$drupal_static_fast['timezones'];

    if (!isset($timezone)) {
      $timezone = date_default_timezone_get();
    }
    // Store DateTimeZone objects in an array rather than repeatedly
    // constructing identical objects over the life of a request.
    if (!isset($timezones[$timezone])) {
      $timezones[$timezone] = timezone_open($timezone);
    }
  }

  // Use the default langcode if none is set.
  global $language;
  if (empty($langcode)) {
    $langcode = isset($language->language) ? $language->language : 'en';
  }

  switch ($type) {
    case 'short':
    case 'small': //D6 compatibility
      $format = variable_get('date_format_short', 'm/d/Y - H:i');
      break;

    case 'long':
      $format = variable_get('date_format_long', 'l, F j, Y - H:i');
      break;

    case 'custom':
      // No change to format.
      break;

    case 'medium':
    default:
      // Retrieve the format of the custom $type passed.
      if ($type != 'medium') {
        $format = variable_get('date_format_' . $type, '');
      }
      // Fall back to 'medium'.
      if ($format === '') {
        $format = variable_get('date_format_medium', 'D, m/d/Y - H:i');
      }
      break;
  }

  if (is_numeric($timezone)) {
    // D6 compatibility
    $timestamp = $timestamp + $timezone;
    $date_time = date_create('@' . $timestamp);

  } else {
    // Create a DateTime object from the timestamp.
    $date_time = date_create('@' . $timestamp);
    // Set the time zone for the DateTime object.
    date_timezone_set($date_time, $timezones[$timezone]);
  }

  // Encode markers that should be translated. 'A' becomes '\xEF\AA\xFF'.
  // xEF and xFF are invalid UTF-8 sequences, and we assume they are not in the
  // input string.
  // Paired backslashes are isolated to prevent errors in read-ahead evaluation.
  // The read-ahead expression ensures that A matches, but not \A.
  $format = preg_replace(array('/\\\\\\\\/', '/(?<!\\\\)([AaeDlMTF])/'), array("\xEF\\\\\\\\\xFF", "\xEF\\\\\$1\$1\xFF"), $format);

  // Call date_format().
  $format = date_format($date_time, $format);

  // Pass the langcode to _format_date_callback().
  _format_date_callback(NULL, $langcode);

  // Translate the marked sequences.
  return preg_replace_callback('/\xEF([AaeDlMTF]?)(.*?)\xFF/', '_format_date_callback', $format);
}

/**
 * Returns an ISO8601 formatted date based on the given date.
 * Backport from D7.
 *
 * Callback for use within hook_rdf_mapping() implementations.
 *
 * @param $date
 *   A UNIX timestamp.
 *
 * @return string
 *   An ISO8601 formatted date.
 */
function date_iso8601($date) {
  // The DATE_ISO8601 constant cannot be used here because it does not match
  // date('c') and produces invalid RDF markup.
  return date('c', $date);
}

/**
 * Translates a formatted date string.
 * Backport from D7.
 *
 * Callback for preg_replace_callback() within format_date().
 */
function _format_date_callback(array $matches = NULL, $new_langcode = NULL) {
  // We cache translations to avoid redundant and rather costly calls to t().
  static $cache, $langcode;

  if (!isset($matches)) {
    $langcode = $new_langcode;
    return;
  }

  $code = $matches[1];
  $string = $matches[2];

  if (!isset($cache[$langcode][$code][$string])) {
    $context = '';

    if ($code == 'F') {
      $context = 'Long month name';
    }

    if ($code == '') {
      $cache[$langcode][$code][$string] = $string;
    }
    else {
      $cache[$langcode][$code][$string] = t($string, array(), $langcode, $context);
    }
  }
  return $cache[$langcode][$code][$string];
}

/**
 * @} End of "defgroup format".
 */

/**
 * Generates an internal or external URL.
 *
 * When creating links in modules, consider whether l() could be a better
 * alternative than url().
 *
 * @param $path
 *   (optional) The internal path or external URL being linked to, such as
 *   "node/34" or "http://example.com/foo". The default value is equivalent to
 *   passing in '<front>'. A few notes:
 *   - If you provide a full URL, it will be considered an external URL.
 *   - If you provide only the path (e.g. "node/34"), it will be
 *     considered an internal link. In this case, it should be a system URL,
 *     and it will be replaced with the alias, if one exists. Additional query
 *     arguments for internal paths must be supplied in $options['query'], not
 *     included in $path.
 *   - If you provide an internal path and $options['alias'] is set to TRUE, the
 *     path is assumed already to be the correct path alias, and the alias is
 *     not looked up.
 *   - The special string '<front>' generates a link to the site's base URL.
 *   - If your external URL contains a query (e.g. http://example.com/foo?a=b),
 *     then you can either URL encode the query keys and values yourself and
 *     include them in $path, or use $options['query'] to let this function
 *     URL encode them.
 * @param $options
 *   (optional) An associative array of additional options, with the following
 *   elements:
 *   - 'query': An array of query key/value-pairs (without any URL-encoding) to
 *     append to the URL.
 *   - 'fragment': A fragment identifier (named anchor) to append to the URL.
 *     Do not include the leading '#' character.
 *   - 'absolute': Defaults to FALSE. Whether to force the output to be an
 *     absolute link (beginning with http:). Useful for links that will be
 *     displayed outside the site, such as in an RSS feed.
 *   - 'alias': Defaults to FALSE. Whether the given path is a URL alias
 *     already.
 *   - 'external': Whether the given path is an external URL.
 *   - 'language': An optional language object. If the path being linked to is
 *     internal to the site, $options['language'] is used to look up the alias
 *     for the URL. If $options['language'] is omitted, the global $language_url
 *     will be used.
 *   - 'https': Whether this URL should point to a secure location. If not
 *     defined, the current scheme is used, so the user stays on HTTP or HTTPS
 *     respectively. TRUE enforces HTTPS and FALSE enforces HTTP, but HTTPS can
 *     only be enforced when the variable 'https' is set to TRUE.
 *   - 'base_url': Only used internally, to modify the base URL when a language
 *     dependent URL requires so.
 *   - 'prefix': Only used internally, to modify the path when a language
 *     dependent URL requires so.
 *   - 'script': The script filename in Drupal's root directory to use when
 *     clean URLs are disabled, such as 'index.php'. Defaults to an empty
 *     string, as most modern web servers automatically find 'index.php'. If
 *     clean URLs are disabled, the value of $path is appended as query
 *     parameter 'q' to $options['script'] in the returned URL. When deploying
 *     Drupal on a web server that cannot be configured to automatically find
 *     index.php, then hook_url_outbound_alter() can be implemented to force
 *     this value to 'index.php'.
 *   - 'entity_type': The entity type of the object that called url(). Only
 *     set if url() is invoked by entity_uri().
 *   - 'entity': The entity object (such as a node) for which the URL is being
 *     generated. Only set if url() is invoked by entity_uri().
 *
 * @return
 *   A string containing a URL to the given path.
 */
function url($path = NULL, array $options = array()) {
  // Merge in defaults.
  $options += array(
    'fragment' => '',
    'query' => array(),
    'absolute' => FALSE,
    'alias' => FALSE,
    'prefix' => ''
  );

  // A duplicate of the code from url_is_external() to avoid needing another
  // function call, since performance inside url() is critical.
  if (!isset($options['external'])) {
    // Return an external link if $path contains an allowed absolute URL. Avoid
    // calling drupal_strip_dangerous_protocols() if there is any slash (/),
    // hash (#) or question_mark (?) before the colon (:) occurrence - if any -
    // as this would clearly mean it is not a URL. If the path starts with 2
    // slashes then it is always considered an external URL without an explicit
    // protocol part.
    $colonpos = strpos($path, ':');
    $options['external'] = (strpos($path, '//') === 0)
      || ($colonpos !== FALSE
        && !preg_match('![/?#]!', substr($path, 0, $colonpos))
        && drupal_strip_dangerous_protocols($path) == $path);
  }

  // Preserve the original path before altering or aliasing.
  $original_path = $path;

  // Allow other modules to alter the outbound URL and options.
  drupal_alter('url_outbound', $path, $options, $original_path);

  if (isset($options['fragment']) && $options['fragment'] !== '') {
    $options['fragment'] = '#' . $options['fragment'];
  }

  if ($options['external']) {
    // Split off the fragment.
    if (strpos($path, '#') !== FALSE) {
      list($path, $old_fragment) = explode('#', $path, 2);
      // If $options contains no fragment, take it over from the path.
      if (isset($old_fragment) && !$options['fragment']) {
        $options['fragment'] = '#' . $old_fragment;
      }
    }
    // Append the query.
    if (is_string($options['query'])) {
      // Backward compatibility with D6
      $path .= (strpos($path, '?') !== FALSE ? '&' : '?') . $options['query'];
    }
    else if ($options['query']) {
      $path .= (strpos($path, '?') !== FALSE ? '&' : '?') . drupal_http_build_query($options['query']);
    }
    if (isset($options['https']) && variable_get('https', FALSE)) {
      if ($options['https'] === TRUE) {
        $path = str_replace('http://', 'https://', $path);
      }
      elseif ($options['https'] === FALSE) {
        $path = str_replace('https://', 'http://', $path);
      }
    }
    // Reassemble.
    return $path . $options['fragment'];
  }

  // Strip leading slashes from internal paths to prevent them becoming external
  // URLs without protocol. /example.com should not be turned into
  // //example.com.
  $path = ltrim($path, '/');

  global $base_url, $base_secure_url, $base_insecure_url;

  // The base_url might be rewritten from the language rewrite in domain mode.
  if (!isset($options['base_url'])) {
    if (isset($options['https']) && variable_get('https', FALSE)) {
      if ($options['https'] === TRUE) {
        $options['base_url'] = $base_secure_url;
        $options['absolute'] = TRUE;
      }
      elseif ($options['https'] === FALSE) {
        $options['base_url'] = $base_insecure_url;
        $options['absolute'] = TRUE;
      }
    }
    else {
      $options['base_url'] = $base_url;
    }
  }

  // The special path '<front>' links to the default front page.
  if ($path == '<front>') {
    $path = '';
  }
  elseif (!empty($path) && !$options['alias']) {
    $language = isset($options['language']) && isset($options['language']->language) ? $options['language']->language : '';
    $alias = drupal_get_path_alias($original_path, $language);
    if ($alias != $original_path) {
      $path = $alias;
    }
  }

  $base = $options['absolute'] ? $options['base_url'] . '/' : base_path();
  $prefix = empty($path) ? rtrim($options['prefix'], '/') : $options['prefix'];

  // With Clean URLs.
  if (!empty($GLOBALS['conf']['clean_url'])) {
    $path = drupal_encode_path($prefix . $path);
    if (is_string($options['query'])) {
      // Backward compatibility with D6
      return $base . $path . '?' . $options['query'] . $options['fragment'];
    }
    else if ($options['query']) {
      return $base . $path . '?' . drupal_http_build_query($options['query']) . $options['fragment'];
    }
    else {
      return $base . $path . $options['fragment'];
    }
  }
  // Without Clean URLs.
  else {
    $path = $prefix . $path;
    $query = array();
    if (!empty($path)) {
      $query['q'] = $path;
    }
    if (is_string($options['query'])) {
      $query = $query ? ('?' . drupal_http_build_query($query) . '&' . $options['query']) : '';
    }
    else {
      if ($options['query']) {
        // We do not use array_merge() here to prevent overriding $path via query
        // parameters.
        $query += $options['query'];
      }
      $query = $query ? ('?' . drupal_http_build_query($query)) : '';
    }
    $script = isset($options['script']) ? $options['script'] : '';
    return $base . $script . $query . $options['fragment'];
  }
}

/**
 * Returns TRUE if a path is external to Drupal (e.g. http://example.com).
 * Backport from D7.
 *
 * If a path cannot be assessed by Drupal's menu handler, then we must
 * treat it as potentially insecure.
 *
 * @param $path
 *   The internal path or external URL being linked to, such as "node/34" or
 *   "http://example.com/foo".
 *
 * @return
 *   Boolean TRUE or FALSE, where TRUE indicates an external path.
 */
function url_is_external($path) {
  $colonpos = strpos($path, ':');
  // Avoid calling drupal_strip_dangerous_protocols() if there is any slash (/),
  // hash (#) or question_mark (?) before the colon (:) occurrence - if any - as
  // this would clearly mean it is not a URL. If the path starts with 2 slashes
  // then it is always considered an external URL without an explicit protocol
  // part.
  return (strpos($path, '//') === 0)
    || ($colonpos !== FALSE
      && !preg_match('![/?#]!', substr($path, 0, $colonpos))
      && drupal_strip_dangerous_protocols($path) == $path);
}

/**
 * Format an attribute string to insert in a tag.
 *
 * @param $attributes
 *   An associative array of HTML attributes.
 * @return
 *   An HTML string ready for insertion in a tag.
 */
function drupal_attributes($attributes = array()) {
  if (is_array($attributes)) {
    $t = '';
    foreach ($attributes as $key => $value) {
      if ($key == 'class' && is_array($value)) {
        $t .= " $key=".'"'. check_plain(implode(' ', $value)) .'"';
      } else {
        $t .= " $key=".'"'. check_plain($value) .'"';
      }
    }
    return $t;
  }
}

/**
 * Formats an internal or external URL link as an HTML anchor tag.
 *
 * This function correctly handles aliased paths, and adds an 'active' class
 * attribute to links that point to the current page (for theming), so all
 * internal links output by modules should be generated by this function if
 * possible.
 *
 * However, for links enclosed in translatable text you should use t() and
 * embed the HTML anchor tag directly in the translated string. For example:
 * @code
 * t('Visit the <a href="@url">settings</a> page', array('@url' => url('admin')));
 * @endcode
 * This keeps the context of the link title ('settings' in the example) for
 * translators.
 *
 * @param $text
 *   The link text for the anchor tag.
 * @param $path
 *   The internal path or external URL being linked to, such as "node/34" or
 *   "http://example.com/foo". After the url() function is called to construct
 *   the URL from $path and $options, the resulting URL is passed through
 *   check_url() before it is inserted into the HTML anchor tag, to ensure
 *   well-formed HTML. See url() for more information and notes.
 * @param $options
 *   An associative array of additional options, with the following elements:
 *   - 'attributes': An associative array of HTML attributes to apply to the
 *     anchor tag.
 *   - 'html' (default FALSE): Whether $text is HTML or just plain-text. For
 *     example, to make an image tag into a link, this must be set to TRUE, or
 *     you will see the escaped HTML image tag.
 *   - 'language': An optional language object. If the path being linked to is
 *     internal to the site, $options['language'] is used to look up the alias
 *     for the URL, and to determine whether the link is "active", or pointing
 *     to the current page (the language as well as the path must match).This
 *     element is also used by url().
 *   - Additional $options elements used by the url() function.
 *
 * @return
 *   An HTML string containing a link to the given path.
 */
function l($text, $path, $options = array()) {
  global $language;

  // Merge in defaults.
  $options += array(
      'attributes' => array(),
      'html' => FALSE,
    );

  if (isset($options['attributes']['class']) && is_array($options['attributes']['class'])) {
    // compatibility with D7 syntax
    $options['attributes']['class'] = implode(' ', $options['attributes']['class']);
  }
  // Append active class.
  if (($path == $_GET['q'] || ($path == '<front>' && drupal_is_front_page())) &&
      (empty($options['language']) || $options['language']->language == $language->language)) {
    if (isset($options['attributes']['class'])) {
      $options['attributes']['class'] .= ' active';
    }
    else {
      $options['attributes']['class'] = 'active';
    }
  }

  // Remove all HTML and PHP tags from a tooltip. For best performance, we act only
  // if a quick strpos() pre-check gave a suspicion (because strip_tags() is expensive).
  if (isset($options['attributes']['title']) && strpos($options['attributes']['title'], '<') !== FALSE) {
    $options['attributes']['title'] = strip_tags($options['attributes']['title']);
  }

  return '<a href="'. check_url(url($path, $options)) .'"'. drupal_attributes($options['attributes']) .'>'. ($options['html'] ? $text : check_plain($text)) .'</a>';
}

/**
 * Perform end-of-request tasks.
 *
 * This function sets the page cache if appropriate, and allows modules to
 * react to the closing of the page by calling hook_exit().
 */
function drupal_page_footer() {
  module_invoke_all('exit');

  // Commit the user session, if needed.
  drupal_session_commit();

  if (variable_get('cache', 0) && ($cache = drupal_page_set_cache())) {
    drupal_serve_page_from_cache($cache);
  }
  else {
    ob_flush();
  }

  module_implements_write_cache();
}

/**
 * Form an associative array from a linear array.
 *
 * This function walks through the provided array and constructs an associative
 * array out of it. The keys of the resulting array will be the values of the
 * input array. The values will be the same as the keys unless a function is
 * specified, in which case the output of the function is used for the values
 * instead.
 *
 * @param $array
 *   A linear array.
 * @param $function
 *   A name of a function to apply to all values before output.
 *
 * @return
 *   An associative array.
 */
function drupal_map_assoc($array, $function = NULL) {
  if (!isset($function)) {
    $result = array();
    foreach ($array as $value) {
      $result[$value] = $value;
    }
    return $result;
  }
  elseif (function_exists($function)) {
    $result = array();
    foreach ($array as $value) {
      $result[$value] = $function($value);
    }
    return $result;
  }
}

/**
 * Evaluate a string of PHP code.
 *
 * This is a wrapper around PHP's eval(). It uses output buffering to capture both
 * returned and printed text. Unlike eval(), we require code to be surrounded by
 * <?php ?> tags; in other words, we evaluate the code as if it were a stand-alone
 * PHP file.
 *
 * Using this wrapper also ensures that the PHP code which is evaluated can not
 * overwrite any variables in the calling code, unlike a regular eval() call.
 *
 * @param $code
 *   The code to evaluate.
 * @return
 *   A string containing the printed output of the code, followed by the returned
 *   output of the code.
 */
function drupal_eval($code) {
  global $theme_path, $theme_info, $conf;

  // Store current theme path.
  $old_theme_path = $theme_path;

  // Restore theme_path to the theme, as long as drupal_eval() executes,
  // so code evaluted will not see the caller module as the current theme.
  // If theme info is not initialized get the path from theme_default.
  if (!isset($theme_info)) {
    $theme_path = drupal_get_path('theme', $conf['theme_default']);
  }
  else {
    $theme_path = dirname($theme_info->filename);
  }

  ob_start();
  print eval('?>'. $code);
  $output = ob_get_contents();
  ob_end_clean();

  // Recover original theme path.
  $theme_path = $old_theme_path;

  return $output;
}

/**
 * Returns the path to a system item (module, theme, etc.).
 *
 * @param $type
 *   The type of the item (i.e. theme, theme_engine, module, profile).
 * @param $name
 *   The name of the item for which the path is requested.
 *
 * @return
 *   The path to the requested item.
 */
function drupal_get_path($type, $name) {
  return dirname(drupal_get_filename($type, $name));
}

/**
 * Returns the base URL path of the Drupal installation.
 * At the very least, this will always default to /.
 */
function base_path() {
  return $GLOBALS['base_path'];
}

/**
 * Provide a substitute clone() function for PHP4.
 */
function drupal_clone($object) {
  return version_compare(phpversion(), '5.0') < 0 ? $object : clone($object);
}

/**
 * Add a <link> tag to the page's HEAD.
 */
function drupal_add_link($attributes) {
  drupal_set_html_head('<link'. drupal_attributes($attributes) .' />');
}

/**
 * Adds a cascading stylesheet to the stylesheet queue.
 *
 * Calling drupal_static_reset('drupal_add_css') will clear all cascading
 * stylesheets added so far.
 *
 * If CSS aggregation/compression is enabled, all cascading style sheets added
 * with $options['preprocess'] set to TRUE will be merged into one aggregate
 * file and compressed by removing all extraneous white space.
 * Preprocessed inline stylesheets will not be aggregated into this single file;
 * instead, they are just compressed upon output on the page. Externally hosted
 * stylesheets are never aggregated or compressed.
 *
 * The reason for aggregating the files is outlined quite thoroughly here:
 * http://www.die.net/musings/page_load_time/ "Load fewer external objects. Due
 * to request overhead, one bigger file just loads faster than two smaller ones
 * half its size."
 *
 * $options['preprocess'] should be only set to TRUE when a file is required for
 * all typical visitors and most pages of a site. It is critical that all
 * preprocessed files are added unconditionally on every page, even if the
 * files do not happen to be needed on a page. This is normally done by calling
 * drupal_add_css() in a hook_init() implementation.
 *
 * Non-preprocessed files should only be added to the page when they are
 * actually needed.
 *
 * @param $data
 *   (optional) The stylesheet data to be added, depending on what is passed
 *   through to the $options['type'] parameter:
 *   - 'file': The path to the CSS file relative to the base_path(), or a
 *     stream wrapper URI. For example: "modules/devel/devel.css" or
 *     "public://generated_css/stylesheet_1.css". Note that Modules should
 *     always prefix the names of their CSS files with the module name; for
 *     example, system-menus.css rather than simply menus.css. Themes can
 *     override module-supplied CSS files based on their filenames, and this
 *     prefixing helps prevent confusing name collisions for theme developers.
 *     See drupal_get_css() where the overrides are performed. Also, if the
 *     direction of the current language is right-to-left (Hebrew, Arabic,
 *     etc.), the function will also look for an RTL CSS file and append it to
 *     the list. The name of this file should have an '-rtl.css' suffix. For
 *     example, a CSS file called 'mymodule-name.css' will have a
 *     'mymodule-name-rtl.css' file added to the list, if exists in the same
 *     directory. This CSS file should contain overrides for properties which
 *     should be reversed or otherwise different in a right-to-left display.
 *   - 'inline': A string of CSS that should be placed in the given scope. Note
 *     that it is better practice to use 'file' stylesheets, rather than
 *     'inline', as the CSS would then be aggregated and cached.
 *   - 'external': The absolute path to an external CSS file that is not hosted
 *     on the local server. These files will not be aggregated if CSS
 *     aggregation is enabled.
 * @param $options
 *   (optional) A string defining the 'type' of CSS that is being added in the
 *   $data parameter ('file', 'inline', or 'external'), or an array which can
 *   have any or all of the following keys:
 *   - 'type': The type of stylesheet being added. Available options are 'file',
 *     'inline' or 'external'. Defaults to 'file'.
 *   - 'basename': Force a basename for the file being added. Modules are
 *     expected to use stylesheets with unique filenames, but integration of
 *     external libraries may make this impossible. The basename of
 *     'modules/node/node.css' is 'node.css'. If the external library "node.js"
 *     ships with a 'node.css', then a different, unique basename would be
 *     'node.js.css'.
 *   - 'group': A number identifying the group in which to add the stylesheet.
 *     Available constants are:
 *     - CSS_SYSTEM: Any system-layer CSS.
 *     - CSS_DEFAULT: (default) Any module-layer CSS.
 *     - CSS_THEME: Any theme-layer CSS.
 *     The group number serves as a weight: the markup for loading a stylesheet
 *     within a lower weight group is output to the page before the markup for
 *     loading a stylesheet within a higher weight group, so CSS within higher
 *     weight groups take precendence over CSS within lower weight groups.
 *   - 'every_page': For optimal front-end performance when aggregation is
 *     enabled, this should be set to TRUE if the stylesheet is present on every
 *     page of the website for users for whom it is present at all. This
 *     defaults to FALSE. It is set to TRUE for stylesheets added via module and
 *     theme .info files. Modules that add stylesheets within hook_init()
 *     implementations, or from other code that ensures that the stylesheet is
 *     added to all website pages, should also set this flag to TRUE. All
 *     stylesheets within the same group that have the 'every_page' flag set to
 *     TRUE and do not have 'preprocess' set to FALSE are aggregated together
 *     into a single aggregate file, and that aggregate file can be reused
 *     across a user's entire site visit, leading to faster navigation between
 *     pages. However, stylesheets that are only needed on pages less frequently
 *     visited, can be added by code that only runs for those particular pages,
 *     and that code should not set the 'every_page' flag. This minimizes the
 *     size of the aggregate file that the user needs to download when first
 *     visiting the website. Stylesheets without the 'every_page' flag are
 *     aggregated into a separate aggregate file. This other aggregate file is
 *     likely to change from page to page, and each new aggregate file needs to
 *     be downloaded when first encountered, so it should be kept relatively
 *     small by ensuring that most commonly needed stylesheets are added to
 *     every page.
 *   - 'weight': The weight of the stylesheet specifies the order in which the
 *     CSS will appear relative to other stylesheets with the same group and
 *     'every_page' flag. The exact ordering of stylesheets is as follows:
 *     - First by group.
 *     - Then by the 'every_page' flag, with TRUE coming before FALSE.
 *     - Then by weight.
 *     - Then by the order in which the CSS was added. For example, all else
 *       being the same, a stylesheet added by a call to drupal_add_css() that
 *       happened later in the page request gets added to the page after one for
 *       which drupal_add_css() happened earlier in the page request.
 *   - 'media': The media type for the stylesheet, e.g., all, print, screen.
 *     Defaults to 'all'.
 *   - 'preprocess': If TRUE and CSS aggregation/compression is enabled, the
 *     styles will be aggregated and compressed. Defaults to TRUE.
 *   - 'browsers': An array containing information specifying which browsers
 *     should load the CSS item. See drupal_pre_render_conditional_comments()
 *     for details.
 *
 * @return
 *   An array of queued cascading stylesheets.
 *
 * @see drupal_get_css()
 */
function drupal_add_css($data = NULL, $options = NULL) {
          // D6 Compat: $path = NULL, $type = 'module', $media = 'all', $preprocess = TRUE
  $css = &drupal_static(__FUNCTION__, array());

  // Construct the options, taking the defaults into consideration.
  if (isset($options)) {
    if (!is_array($options)) {
      $options = array('type' => $options);

      // Drupal6 compatibility
      $args = func_get_args();
      if (isset($args[2])) {
        $options['media'] = $args[2];
      }
      if (isset($args[3])) {
        $options['preprocess'] = $args[3];
        if (!$options['preprocess'] && strpos($data, '://') === FALSE && $data{0} != '/') {
          // D6 modules may add files with preprocess = FALSE and don't provide valid url.
          $data = base_path() . $data;
        }
      }

      if ($options['type'] == 'theme') {
        $options['group'] = CSS_THEME;
        $options['every_page'] = TRUE;
      }
      if ($options['type'] == 'module') {
        $options['type'] = 'file';
      }
    }
  }
  else {
    $options = array();
  }

  // Create an array of CSS files for each media type first, since each type needs to be served
  // to the browser differently.
  if (isset($data)) {
    $options += array(
      'type' => 'file',
      'group' => CSS_DEFAULT,
      'weight' => 0,
      'every_page' => FALSE,
      'media' => 'all',
      'preprocess' => TRUE,
      'data' => $data,
      'browsers' => array(),
    );
    $options['browsers'] += array(
      'IE' => TRUE,
      '!IE' => TRUE,
    );

    // Files with a query string cannot be preprocessed.
    if ($options['type'] === 'file' && $options['preprocess'] && strpos($options['data'], '?') !== FALSE) {
      $options['preprocess'] = FALSE;
    }

    // Always add a tiny value to the weight, to conserve the insertion order.
    $options['weight'] += count($css) / 1000;

    // Add the data to the CSS array depending on the type.
    switch ($options['type']) {
      case 'inline':
        // For inline stylesheets, we don't want to use the $data as the array
        // key as $data could be a very long string of CSS.
        $css[] = $options;
        break;
      default:
        // Local and external files must keep their name as the associative key
        // so the same CSS file is not be added twice.
        $css[$data] = $options;
    }
  }

  return $css;
}

/**
 * Returns a themed representation of all stylesheets to attach to the page. 
 * Backpor from D7.
 *
 * It loads the CSS in order, with 'module' first, then 'theme' afterwards.
 * This ensures proper cascading of styles so themes can easily override
 * module styles through CSS selectors.
 *
 * Themes may replace module-defined CSS files by adding a stylesheet with the
 * same filename. For example, themes/bartik/system-menus.css would replace
 * modules/system/system-menus.css. This allows themes to override complete
 * CSS files, rather than specific selectors, when necessary.
 *
 * If the original CSS file is being overridden by a theme, the theme is
 * responsible for supplying an accompanying RTL CSS file to replace the
 * module's.
 *
 * @param $css
 *   (optional) An array of CSS files. If no array is provided, the default
 *   stylesheets array is used instead.
 * @param $skip_alter
 *   (optional) If set to TRUE, this function skips calling drupal_alter() on
 *   $css, useful when the calling function passes a $css array that has already
 *   been altered.
 *
 * @return
 *   A string of XHTML CSS tags.
 *
 * @see drupal_add_css()
 */
function drupal_get_css($css = NULL, $skip_alter = FALSE) {
  if (!isset($css)) {
    $css = drupal_add_css();
  }

  // Allow modules and themes to alter the CSS items.
  if (!$skip_alter) {
    drupal_alter('css', $css);
  }

  // Sort CSS items, so that they appear in the correct order.
  uasort($css, 'drupal_sort_css_js');

  // Provide the page with information about the individual CSS files used,
  // information not otherwise available when CSS aggregation is enabled. The
  // setting is attached later in this function, but is set here, so that CSS
  // files removed below are still considered "used" and prevented from being
  // added in a later AJAX request.
  // Skip if no files were added to the page or jQuery.extend() will overwrite
  // the Drupal.settings.ajaxPageState.css object with an empty array.
  if (!empty($css)) {
    // Cast the array to an object to be on the safe side even if not empty.
    $setting['ajaxPageState']['css'] = (object) array_fill_keys(array_keys($css), 1);
  }

  // Remove the overridden CSS files. Later CSS files override former ones.
  $previous_item = array();
  foreach ($css as $key => $item) {
    if ($item['type'] == 'file') {
      // If defined, force a unique basename for this file.
      $basename = isset($item['basename']) ? $item['basename'] : basename($item['data']);
      if (isset($previous_item[$basename])) {
        // Remove the previous item that shared the same base name.
        unset($css[$previous_item[$basename]]);
      }
      $previous_item[$basename] = $key;
    }
  }

  // Render the HTML needed to load the CSS.
  $styles = array(
    '#type' => 'styles',
    '#items' => $css,
  );

  if (!empty($setting)) {
    $styles['#attached']['js'][] = array('type' => 'setting', 'data' => $setting);
  }
  $output = drupal_render($styles);
  return $output;
}

/**
 * Sorts CSS and JavaScript resources. Backport from D7.
 *
 * Callback for uasort() within:
 * - drupal_get_css()
 * - drupal_get_js()
 *
 * This sort order helps optimize front-end performance while providing modules
 * and themes with the necessary control for ordering the CSS and JavaScript
 * appearing on a page.
 *
 * @param $a
 *   First item for comparison. The compared items should be associative arrays
 *   of member items from drupal_add_css() or drupal_add_js().
 * @param $b
 *   Second item for comparison.
 *
 * @see drupal_add_css()
 * @see drupal_add_js()
 */
function drupal_sort_css_js($a, $b) {
  // First order by group, so that, for example, all items in the CSS_SYSTEM
  // group appear before items in the CSS_DEFAULT group, which appear before
  // all items in the CSS_THEME group. Modules may create additional groups by
  // defining their own constants.
  if ($a['group'] < $b['group']) {
    return -1;
  }
  elseif ($a['group'] > $b['group']) {
    return 1;
  }
  // Within a group, order all infrequently needed, page-specific files after
  // common files needed throughout the website. Separating this way allows for
  // the aggregate file generated for all of the common files to be reused
  // across a site visit without being cut by a page using a less common file.
  elseif ($a['every_page'] && !$b['every_page']) {
    return -1;
  }
  elseif (!$a['every_page'] && $b['every_page']) {
    return 1;
  }
  // Finally, order by weight.
  elseif ($a['weight'] < $b['weight']) {
    return -1;
  }
  elseif ($a['weight'] > $b['weight']) {
    return 1;
  }
  else {
    return 0;
  }
}

/**
 * Default callback to group CSS items. Backport from D7.
 *
 * This function arranges the CSS items that are in the #items property of the
 * styles element into groups. Arranging the CSS items into groups serves two
 * purposes. When aggregation is enabled, files within a group are aggregated
 * into a single file, significantly improving page loading performance by
 * minimizing network traffic overhead. When aggregation is disabled, grouping
 * allows multiple files to be loaded from a single STYLE tag, enabling sites
 * with many modules enabled or a complex theme being used to stay within IE's
 * 31 CSS inclusion tag limit: http://drupal.org/node/228818.
 *
 * This function puts multiple items into the same group if they are groupable
 * and if they are for the same 'media' and 'browsers'. Items of the 'file' type
 * are groupable if their 'preprocess' flag is TRUE, items of the 'inline' type
 * are always groupable, and items of the 'external' type are never groupable.
 * This function also ensures that the process of grouping items does not change
 * their relative order. This requirement may result in multiple groups for the
 * same type, media, and browsers, if needed to accommodate other items in
 * between.
 *
 * @param $css
 *   An array of CSS items, as returned by drupal_add_css(), but after
 *   alteration performed by drupal_get_css().
 *
 * @return
 *   An array of CSS groups. Each group contains the same keys (e.g., 'media',
 *   'data', etc.) as a CSS item from the $css parameter, with the value of
 *   each key applying to the group as a whole. Each group also contains an
 *   'items' key, which is the subset of items from $css that are in the group.
 *
 * @see drupal_pre_render_styles()
 * @see system_element_info()
 */
function drupal_group_css($css) {
  $groups = array();
  // If a group can contain multiple items, we track the information that must
  // be the same for each item in the group, so that when we iterate the next
  // item, we can determine if it can be put into the current group, or if a
  // new group needs to be made for it.
  $current_group_keys = NULL;
  // When creating a new group, we pre-increment $i, so by initializing it to
  // -1, the first group will have index 0.
  $i = -1;
  foreach ($css as $item) {
    // The browsers for which the CSS item needs to be loaded is part of the
    // information that determines when a new group is needed, but the order of
    // keys in the array doesn't matter, and we don't want a new group if all
    // that's different is that order.
    ksort($item['browsers']);

    // If the item can be grouped with other items, set $group_keys to an array
    // of information that must be the same for all items in its group. If the
    // item can't be grouped with other items, set $group_keys to FALSE. We
    // put items into a group that can be aggregated together: whether they will
    // be aggregated is up to the _drupal_css_aggregate() function or an
    // override of that function specified in hook_css_alter(), but regardless
    // of the details of that function, a group represents items that can be
    // aggregated. Since a group may be rendered with a single HTML tag, all
    // items in the group must share the same information that would need to be
    // part of that HTML tag.
    switch ($item['type']) {
      case 'file':
      case 'module':
        // Group file items if their 'preprocess' flag is TRUE.
        // Help ensure maximum reuse of aggregate files by only grouping
        // together items that share the same 'group' value and 'every_page'
        // flag. See drupal_add_css() for details about that.
        $group_keys = $item['preprocess'] ? array($item['type'], $item['group'], $item['every_page'], $item['media'], $item['browsers']) : FALSE;
        break;
      case 'inline':
        // Always group inline items.
        $group_keys = array($item['type'], $item['media'], $item['browsers']);
        break;
      case 'external':
        // Do not group external items.
        $group_keys = FALSE;
        break;
    }

    // If the group keys don't match the most recent group we're working with,
    // then a new group must be made.
    if ($group_keys !== $current_group_keys) {
      $i++;
      // Initialize the new group with the same properties as the first item
      // being placed into it. The item's 'data' and 'weight' properties are
      // unique to the item and should not be carried over to the group.
      $groups[$i] = $item;
      unset($groups[$i]['data'], $groups[$i]['weight']);
      $groups[$i]['items'] = array();
      $current_group_keys = $group_keys ? $group_keys : NULL;
    }

    // Add the item to the current group.
    $groups[$i]['items'][] = $item;
  }
  return $groups;
}

/**
 * Default callback to aggregate CSS files and inline content.
 * Backport from D7.
 *
 * Having the browser load fewer CSS files results in much faster page loads
 * than when it loads many small files. This function aggregates files within
 * the same group into a single file unless the site-wide setting to do so is
 * disabled (commonly the case during site development). To optimize download,
 * it also compresses the aggregate files by removing comments, whitespace, and
 * other unnecessary content. Additionally, this functions aggregates inline
 * content together, regardless of the site-wide aggregation setting.
 *
 * @param $css_groups
 *   An array of CSS groups as returned by drupal_group_css(). This function
 *   modifies the group's 'data' property for each group that is aggregated.
 *
 * @see drupal_group_css()
 * @see drupal_pre_render_styles()
 * @see system_element_info()
 */
function drupal_aggregate_css(&$css_groups) {
  $preprocess_css = (variable_get('preprocess_css', FALSE) && (!defined('MAINTENANCE_MODE') || MAINTENANCE_MODE != 'update'));

  // For each group that needs aggregation, aggregate its items.
  foreach ($css_groups as $key => $group) {
    switch ($group['type']) {
      // If a file group can be aggregated into a single file, do so, and set
      // the group's data property to the file path of the aggregate file.
      case 'file':
        if ($group['preprocess'] && $preprocess_css) {
          $css_groups[$key]['data'] = drupal_build_css_cache($group['items']);
        }
        break;
      // Aggregate all inline CSS content into the group's data property.
      case 'inline':
        $css_groups[$key]['data'] = '';
        foreach ($group['items'] as $item) {
          $css_groups[$key]['data'] .= drupal_load_stylesheet_content($item['data'], $item['preprocess']);
        }
        break;
    }
  }
}

/**
 * #pre_render callback to add the elements needed for CSS tags to be rendered.
 * Backport from D7.
 *
 * For production websites, LINK tags are preferable to STYLE tags with @import
 * statements, because:
 * - They are the standard tag intended for linking to a resource.
 * - On Firefox 2 and perhaps other browsers, CSS files included with @import
 *   statements don't get saved when saving the complete web page for offline
 *   use: http://drupal.org/node/145218.
 * - On IE, if only LINK tags and no @import statements are used, all the CSS
 *   files are downloaded in parallel, resulting in faster page load, but if
 *   @import statements are used and span across multiple STYLE tags, all the
 *   ones from one STYLE tag must be downloaded before downloading begins for
 *   the next STYLE tag. Furthermore, IE7 does not support media declaration on
 *   the @import statement, so multiple STYLE tags must be used when different
 *   files are for different media types. Non-IE browsers always download in
 *   parallel, so this is an IE-specific performance quirk:
 *   http://www.stevesouders.com/blog/2009/04/09/dont-use-import/.
 *
 * However, IE has an annoying limit of 31 total CSS inclusion tags
 * (http://drupal.org/node/228818) and LINK tags are limited to one file per
 * tag, whereas STYLE tags can contain multiple @import statements allowing
 * multiple files to be loaded per tag. When CSS aggregation is disabled, a
 * Drupal site can easily have more than 31 CSS files that need to be loaded, so
 * using LINK tags exclusively would result in a site that would display
 * incorrectly in IE. Depending on different needs, different strategies can be
 * employed to decide when to use LINK tags and when to use STYLE tags.
 *
 * The strategy employed by this function is to use LINK tags for all aggregate
 * files and for all files that cannot be aggregated (e.g., if 'preprocess' is
 * set to FALSE or the type is 'external'), and to use STYLE tags for groups
 * of files that could be aggregated together but aren't (e.g., if the site-wide
 * aggregation setting is disabled). This results in all LINK tags when
 * aggregation is enabled, a guarantee that as many or only slightly more tags
 * are used with aggregation disabled than enabled (so that if the limit were to
 * be crossed with aggregation enabled, the site developer would also notice the
 * problem while aggregation is disabled), and an easy way for a developer to
 * view HTML source while aggregation is disabled and know what files will be
 * aggregated together when aggregation becomes enabled.
 *
 * This function evaluates the aggregation enabled/disabled condition on a group
 * by group basis by testing whether an aggregate file has been made for the
 * group rather than by testing the site-wide aggregation setting. This allows
 * this function to work correctly even if modules have implemented custom
 * logic for grouping and aggregating files.
 *
 * @param $element
 *   A render array containing:
 *   - '#items': The CSS items as returned by drupal_add_css() and altered by
 *     drupal_get_css().
 *   - '#group_callback': A function to call to group #items to enable the use
 *     of fewer tags by aggregating files and/or using multiple @import
 *     statements within a single tag.
 *   - '#aggregate_callback': A function to call to aggregate the items within
 *     the groups arranged by the #group_callback function.
 *
 * @return
 *   A render array that will render to a string of XHTML CSS tags.
 *
 * @see drupal_get_css()
 */
function drupal_pre_render_styles($elements) {
  global $base_path;

  // Group and aggregate the items.
  if (isset($elements['#group_callback'])) {
    $elements['#groups'] = $elements['#group_callback']($elements['#items']);
  }
  if (isset($elements['#aggregate_callback'])) {
    $elements['#aggregate_callback']($elements['#groups']);
  }

  // A dummy query-string is added to filenames, to gain control over
  // browser-caching. The string changes on every update or full cache
  // flush, forcing browsers to load a new copy of the files, as the
  // URL changed.
  $query_string = variable_get('css_js_query_string', '0');

  // For inline CSS to validate as XHTML, all CSS containing XHTML needs to be
  // wrapped in CDATA. To make that backwards compatible with HTML 4, we need to
  // comment out the CDATA-tag.
  $embed_prefix = "\n<!--/*--><![CDATA[/*><!--*/\n";
  $embed_suffix = "\n/*]]>*/-->\n";

  // Defaults for LINK and STYLE elements.
  $link_element_defaults = array(
    '#type' => 'html_tag',
    '#tag' => 'link',
    '#attributes' => array(
      'type' => 'text/css',
      'rel' => 'stylesheet',
    ),
  );
  $style_element_defaults = array(
    '#type' => 'html_tag',
    '#tag' => 'style',
    '#attributes' => array(
      'type' => 'text/css',
    ),
  );

  // Loop through each group.
  foreach ($elements['#groups'] as $group) {
    switch ($group['type']) {
      // For file items, there are three possibilites.
      // - The group has been aggregated: in this case, output a LINK tag for
      //   the aggregate file.
      // - The group can be aggregated but has not been (most likely because
      //   the site administrator disabled the site-wide setting): in this case,
      //   output as few STYLE tags for the group as possible, using @import
      //   statement for each file in the group. This enables us to stay within
      //   IE's limit of 31 total CSS inclusion tags.
      // - The group contains items not eligible for aggregation (their
      //   'preprocess' flag has been set to FALSE): in this case, output a LINK
      //   tag for each file.
      case 'file':
        // The group has been aggregated into a single file: output a LINK tag
        // for the aggregate file.
        if (isset($group['data'])) {
          $element = $link_element_defaults;
          $element['#attributes']['href'] = url($group['data']);
          $element['#attributes']['media'] = $group['media'];
          $element['#browsers'] = $group['browsers'];
          $element['#weight'] = $group['group'];
          $elements[] = $element;
        }
        // The group can be aggregated, but hasn't been: combine multiple items
        // into as few STYLE tags as possible.
        elseif ($group['preprocess']) {
          $import = array();
          foreach ($group['items'] as $item) {
            // A theme's .info file may have an entry for a file that doesn't
            // exist as a way of overriding a module or base theme CSS file from
            // being added to the page. Normally, file_exists() calls that need
            // to run for every page request should be minimized, but this one
            // is okay, because it only runs when CSS aggregation is disabled.
            // On a server under heavy enough load that file_exists() calls need
            // to be minimized, CSS aggregation should be enabled, in which case
            // this code is not run. When aggregation is enabled,
            // drupal_load_stylesheet() checks file_exists(), but only when
            // building the aggregate file, which is then reused for many page
            // requests.
            if (file_exists($item['data'])) {
              // The dummy query string needs to be added to the URL to control
              // browser-caching. IE7 does not support a media type on the
              // @import statement, so we instead specify the media for the
              // group on the STYLE tag.
              $import[] = '@import url("' . base_path() . $item['data'] . '?' . $query_string . '");';
            }
          }
          // In addition to IE's limit of 31 total CSS inclusion tags, it also
          // has a limit of 31 @import statements per STYLE tag.
          while (!empty($import)) {
            $import_batch = array_slice($import, 0, 31);
            $import = array_slice($import, 31);
            $element = $style_element_defaults;
            // This simplifies the JavaScript regex, allowing each line
            // (separated by \n) to be treated as a completely different string.
            // This means that we can use ^ and $ on one line at a time, and not
            // worry about style tags since they'll never match the regex.
            $element['#value'] = "\n" . implode("\n", $import_batch) . "\n";
            $element['#attributes']['media'] = $group['media'];
            $element['#browsers'] = $group['browsers'];
            $element['#weight'] = $group['group'];
            $elements[] = $element;
          }
        }
        // The group contains items ineligible for aggregation: output a LINK
        // tag for each file.
        else {
          foreach ($group['items'] as $item) {
            $element = $link_element_defaults;
            // We do not check file_exists() here, because this code runs for
            // files whose 'preprocess' is set to FALSE, and therefore, even
            // when aggregation is enabled, and we want to avoid needlessly
            // taxing a server that may be under heavy load. The file_exists()
            // performed above for files whose 'preprocess' is TRUE is done for
            // the benefit of theme .info files, but code that deals with files
            // whose 'preprocess' is FALSE is responsible for ensuring the file
            // exists.
            // The dummy query string needs to be added to the URL to control
            // browser-caching.
            $query_string_separator = (strpos($item['data'], '?') !== FALSE) ? '&' : '?';
            $element['#attributes']['href'] = $item['data'] . $query_string_separator . $query_string;
            $element['#attributes']['media'] = $item['media'];
            $element['#browsers'] = $group['browsers'];
            $elements[] = $element;
          }
        }
        break;
      // For inline content, the 'data' property contains the CSS content. If
      // the group's 'data' property is set, then output it in a single STYLE
      // tag. Otherwise, output a separate STYLE tag for each item.
      case 'inline':
        if (isset($group['data'])) {
          $element = $style_element_defaults;
          $element['#value'] = $group['data'];
          $element['#value_prefix'] = $embed_prefix;
          $element['#value_suffix'] = $embed_suffix;
          $element['#attributes']['media'] = $group['media'];
          $element['#browsers'] = $group['browsers'];
            $element['#weight'] = $group['group'];
          $elements[] = $element;
        }
        else {
          foreach ($group['items'] as $item) {
            $element = $style_element_defaults;
            $element['#value'] = $item['data'];
            $element['#value_prefix'] = $embed_prefix;
            $element['#value_suffix'] = $embed_suffix;
            $element['#attributes']['media'] = $item['media'];
            $element['#browsers'] = $group['browsers'];
            $element['#weight'] = $group['group'];
            $elements[] = $element;
          }
        }
        break;
      // Output a LINK tag for each external item. The item's 'data' property
      // contains the full URL.
      case 'external':
        foreach ($group['items'] as $item) {
          $element = $link_element_defaults;
          $element['#attributes']['href'] = $item['data'];
          $element['#attributes']['media'] = $item['media'];
          $element['#browsers'] = $group['browsers'];
          $element['#weight'] = $group['group'];
          $elements[] = $element;
        }
        break;
    }
  }

  return $elements;
}

/**
 * Aggregates and optimizes CSS files into a cache file in the files directory.
 * Backport from D7.
 *
 * The file name for the CSS cache file is generated from the hash of the
 * aggregated contents of the files in $css. This forces proxies and browsers
 * to download new CSS when the CSS changes.
 *
 * The cache file name is retrieved on a page load via a lookup variable that
 * contains an associative array. The array key is the hash of the file names
 * in $css while the value is the cache file name. The cache file is generated
 * in two cases. First, if there is no file name value for the key, which will
 * happen if a new file name has been added to $css or after the lookup
 * variable is emptied to force a rebuild of the cache. Second, the cache file
 * is generated if it is missing on disk. Old cache files are not deleted
 * immediately when the lookup variable is emptied, but are deleted after a set
 * period by drupal_delete_file_if_stale(). This ensures that files referenced
 * by a cached page will still be available.
 *
 * @param $css
 *   An array of CSS files to aggregate and compress into one file.
 *
 * @return
 *   The URI of the CSS cache file, or FALSE if the file could not be saved.
 */
function drupal_build_css_cache($css) {
  $data = '';
  $uri = '';
  $map = variable_get('drupal_css_cache_files', array());
  // Create a new array so that only the file names are used to create the hash.
  // This prevents new aggregates from being created unnecessarily.
  $css_data = array();
  foreach ($css as $css_file) {
    $css_data[] = $css_file['data'];
  }
  $key = hash('sha256', serialize($css_data));
  if (isset($map[$key])) {
    $uri = $map[$key];
  }

  if (empty($uri) || !file_exists($uri)) {
    // Build aggregate CSS file.
    foreach ($css as $stylesheet) {
      // Only 'file' stylesheets can be aggregated.
      if ($stylesheet['type'] == 'file') {
        $contents = drupal_load_stylesheet($stylesheet['data'], TRUE);

        // Build the base URL of this CSS file: start with the full URL.
        $css_base_url = url($stylesheet['data']);
        // Move to the parent.
        $css_base_url = substr($css_base_url, 0, strrpos($css_base_url, '/'));
        // Simplify to a relative URL if the stylesheet URL starts with the
        // base URL of the website.
        if (substr($css_base_url, 0, strlen($GLOBALS['base_root'])) == $GLOBALS['base_root']) {
          $css_base_url = substr($css_base_url, strlen($GLOBALS['base_root']));
        }

        _drupal_build_css_path(NULL, $css_base_url . '/');
        // Anchor all paths in the CSS with its base URL, ignoring external and absolute paths.
        $data .= preg_replace_callback('/url\(\s*[\'"]?(?![a-z]+:|\/+)([^\'")]+)[\'"]?\s*\)/i', '_drupal_build_css_path', $contents);
      }
    }

    // Per the W3C specification at http://www.w3.org/TR/REC-CSS2/cascade.html#at-import,
    // @import rules must proceed any other style, so we move those to the top.
    $regexp = '/@import[^;]+;/i';
    preg_match_all($regexp, $data, $matches);
    $data = preg_replace($regexp, '', $data);
    $data = implode('', $matches[0]) . $data;

    // Prefix filename to prevent blocking by firewalls which reject files
    // starting with "ad*".
    $filename = 'css_' . drupal_hash_base64($data) . '.css';
    // Create the css/ within the files folder.
    $csspath = file_create_path('css');
    file_check_directory($csspath, FILE_CREATE_DIRECTORY);
    $uri = $csspath . '/' . $filename;
    // Create the CSS file.
    if (!file_exists($uri) && !file_save_data($data, $uri, FILE_EXISTS_REPLACE)) {
      return FALSE;
    }
    // If CSS gzip compression is enabled, clean URLs are enabled (which means
    // that rewrite rules are working) and the zlib extension is available then
    // create a gzipped version of this file. This file is served conditionally
    // to browsers that accept gzip using .htaccess rules.
    if (variable_get('css_gzip_compression', TRUE) && variable_get('clean_url', 0) && extension_loaded('zlib')) {
      if (!file_exists($uri . '.gz') && !file_save_data(gzencode($data, 9, FORCE_GZIP), $uri . '.gz', FILE_EXISTS_REPLACE)) {
        return FALSE;
      }
    }
    // Save the updated map.
    $map[$key] = $uri;
    variable_set('drupal_css_cache_files', $map);
  }
  return $uri;
}

/**
 * Helper function for drupal_build_css_cache().
 *
 * This function will prefix all paths within a CSS file.
 */
function _drupal_build_css_path($matches, $base = NULL) {
  static $_base;
  // Store base path for preg_replace_callback.
  if (isset($base)) {
    $_base = $base;
  }

  // Prefix with base and remove '../' segments where possible.
  $path = $_base . $matches[1];
  $last = '';
  while ($path != $last) {
    $last = $path;
    $path = preg_replace('`(^|/)(?!\.\./)([^/]+)/\.\./`', '$1', $path);
  }
  return 'url('. $path .')';
}

/**
 * Loads the stylesheet and resolves all @import commands.
 *
 * Loads a stylesheet and replaces @import commands with the contents of the
 * imported file. Use this instead of file_get_contents when processing
 * stylesheets.
 *
 * The returned contents are compressed removing white space and comments only
 * when CSS aggregation is enabled. This optimization will not apply for
 * color.module enabled themes with CSS aggregation turned off.
 *
 * @param $file
 *   Name of the stylesheet to be processed.
 * @param $optimize
 *   Defines if CSS contents should be compressed or not.
 * @param $reset_basepath
 *   Used internally to facilitate recursive resolution of @import commands.
 *
 * @return
 *   Contents of the stylesheet, including any resolved @import commands.
 */
function drupal_load_stylesheet($file, $optimize = NULL, $reset_basepath = TRUE) {
  // These statics are not cache variables, so we don't use drupal_static().
  static $_optimize, $basepath;
  if ($reset_basepath) {
    $basepath = '';
  }
  // Store the value of $optimize for preg_replace_callback with nested
  // @import loops.
  if (isset($optimize)) {
    $_optimize = $optimize;
  }

  // Stylesheets are relative one to each other. Start by adding a base path
  // prefix provided by the parent stylesheet (if necessary).
  if ($basepath) {
    $file = $basepath . '/' . $file;
  }
  // Store the parent base path to restore it later.
  $parent_base_path = $basepath;
  // Set the current base path to process possible child imports.
  $basepath = dirname($file);

  // Load the CSS stylesheet. We suppress errors because themes may specify
  // stylesheets in their .info file that don't exist in the theme's path,
  // but are merely there to disable certain module CSS files.
  $content = '';
  if ($contents = @file_get_contents($file)) {
    // Return the processed stylesheet.
    $content = drupal_load_stylesheet_content($contents, $_optimize);
  }

  // Restore the parent base path as the file and its childen are processed.
  $basepath = $parent_base_path;
  return $content;
}

/**
 * Processes the contents of a stylesheet for aggregation.
 *
 * @param $contents
 *   The contents of the stylesheet.
 * @param $optimize
 *   (optional) Boolean whether CSS contents should be minified. Defaults to
 *   FALSE.
 *
 * @return
 *   Contents of the stylesheet including the imported stylesheets.
 */
function drupal_load_stylesheet_content($contents, $optimize = FALSE) {
  // Remove multiple charset declarations for standards compliance (and fixing Safari problems).
  $contents = preg_replace('/^@charset\s+[\'"](\S*?)\b[\'"];/i', '', $contents);

  if ($optimize) {
    // Perform some safe CSS optimizations.
    // Regexp to match comment blocks.
    $comment     = '/\*[^*]*\*+(?:[^/*][^*]*\*+)*/';
    // Regexp to match double quoted strings.
    $double_quot = '"[^"\\\\]*(?:\\\\.[^"\\\\]*)*"';
    // Regexp to match single quoted strings.
    $single_quot = "'[^'\\\\]*(?:\\\\.[^'\\\\]*)*'";
    // Strip all comment blocks, but keep double/single quoted strings.
    $contents = preg_replace(
      "<($double_quot|$single_quot)|$comment>Ss",
      "$1",
      $contents
    );
    // Remove certain whitespace.
    // There are different conditions for removing leading and trailing
    // whitespace.
    // @see http://php.net/manual/regexp.reference.subpatterns.php
    $contents = preg_replace('<
      # Strip leading and trailing whitespace.
        \s*([@{};,])\s*
      # Strip only leading whitespace from:
      # - Closing parenthesis: Retain "@media (bar) and foo".
      | \s+([\)])
      # Strip only trailing whitespace from:
      # - Opening parenthesis: Retain "@media (bar) and foo".
      # - Colon: Retain :pseudo-selectors.
      | ([\(:])\s+
    >xS',
      // Only one of the three capturing groups will match, so its reference
      // will contain the wanted value and the references for the
      // two non-matching groups will be replaced with empty strings.
      '$1$2$3',
      $contents
    );
    // End the file with a new line.
    $contents = trim($contents);
    $contents .= "\n";
  }

  // Replaces @import commands with the actual stylesheet content.
  // This happens recursively but omits external files.
  $contents = preg_replace_callback('/@import\s*(?:url\(\s*)?[\'"]?(?![a-z]+:)(?!\/\/)([^\'"\()]+)[\'"]?\s*\)?\s*;/', '_drupal_load_stylesheet', $contents);
  return $contents;
}

/**
 * Loads stylesheets recursively and returns contents with corrected paths.
 *
 * This function is used for recursive loading of stylesheets and
 * returns the stylesheet content with all url() paths corrected.
 */
function _drupal_load_stylesheet($matches) {
  $filename = $matches[1];
  // Load the imported stylesheet and replace @import commands in there as well.
  $file = drupal_load_stylesheet($filename, NULL, FALSE);

  // Determine the file's directory.
  $directory = dirname($filename);
  // If the file is in the current directory, make sure '.' doesn't appear in
  // the url() path.
  $directory = $directory == '.' ? '' : $directory .'/';

  // Alter all internal url() paths. Leave external paths alone. We don't need
  // to normalize absolute paths here (i.e. remove folder/... segments) because
  // that will be done later.
  return preg_replace('/url\(\s*([\'"]?)(?![a-z]+:|\/+)([^\'")]+)([\'"]?)\s*\)/i', 'url(\1' . $directory . '\2\3)', $file);
}

/**
 * Delete all cached CSS files.
 */
function drupal_clear_css_cache() {
  file_scan_directory(file_create_path('css'), '.*', array('.', '..', 'CVS'), 'file_delete', TRUE);
}

/**
 * Adds a JavaScript file, setting, or inline code to the page.
 *
 * The behavior of this function depends on the parameters it is called with.
 * Generally, it handles the addition of JavaScript to the page, either as
 * reference to an existing file or as inline code. The following actions can be
 * performed using this function:
 * - Add a file ('file'): Adds a reference to a JavaScript file to the page.
 * - Add inline JavaScript code ('inline'): Executes a piece of JavaScript code
 *   on the current page by placing the code directly in the page (for example,
 *   to tell the user that a new message arrived, by opening a pop up, alert
 *   box, etc.). This should only be used for JavaScript that cannot be executed
 *   from a file. When adding inline code, make sure that you are not relying on
 *   $() being the jQuery function. Wrap your code in
 *   @code (function ($) {... })(jQuery); @endcode
 *   or use jQuery() instead of $().
 * - Add external JavaScript ('external'): Allows the inclusion of external
 *   JavaScript files that are not hosted on the local server. Note that these
 *   external JavaScript references do not get aggregated when preprocessing is
 *   on.
 * - Add settings ('setting'): Adds settings to Drupal's global storage of
 *   JavaScript settings. Per-page settings are required by some modules to
 *   function properly. All settings will be accessible at Drupal.settings.
 *
 * Examples:
 * @code
 *   drupal_add_js('misc/collapse.js');
 *   drupal_add_js('misc/collapse.js', 'file');
 *   drupal_add_js('jQuery(document).ready(function () { alert("Hello!"); });', 'inline');
 *   drupal_add_js('jQuery(document).ready(function () { alert("Hello!"); });',
 *     array('type' => 'inline', 'scope' => 'footer', 'weight' => 5)
 *   );
 *   drupal_add_js('http://example.com/example.js', 'external');
 *   drupal_add_js(array('myModule' => array('key' => 'value')), 'setting');
 * @endcode
 *
 * Calling drupal_static_reset('drupal_add_js') will clear all JavaScript added
 * so far.
 *
 * If JavaScript aggregation is enabled, all JavaScript files added with
 * $options['preprocess'] set to TRUE will be merged into one aggregate file.
 * Preprocessed inline JavaScript will not be aggregated into this single file.
 * Externally hosted JavaScripts are never aggregated.
 *
 * The reason for aggregating the files is outlined quite thoroughly here:
 * http://www.die.net/musings/page_load_time/ "Load fewer external objects. Due
 * to request overhead, one bigger file just loads faster than two smaller ones
 * half its size."
 *
 * $options['preprocess'] should be only set to TRUE when a file is required for
 * all typical visitors and most pages of a site. It is critical that all
 * preprocessed files are added unconditionally on every page, even if the
 * files are not needed on a page. This is normally done by calling
 * drupal_add_js() in a hook_init() implementation.
 *
 * Non-preprocessed files should only be added to the page when they are
 * actually needed.
 *
 * @param $data
 *   (optional) If given, the value depends on the $options parameter, or
 *   $options['type'] if $options is passed as an associative array:
 *   - 'file': Path to the file relative to base_path().
 *   - 'inline': The JavaScript code that should be placed in the given scope.
 *   - 'external': The absolute path to an external JavaScript file that is not
 *     hosted on the local server. These files will not be aggregated if
 *     JavaScript aggregation is enabled.
 *   - 'setting': An associative array with configuration options. The array is
 *     merged directly into Drupal.settings. All modules should wrap their
 *     actual configuration settings in another variable to prevent conflicts in
 *     the Drupal.settings namespace. Items added with a string key will replace
 *     existing settings with that key; items with numeric array keys will be
 *     added to the existing settings array.
 * @param $options
 *   (optional) A string defining the type of JavaScript that is being added in
 *   the $data parameter ('file'/'setting'/'inline'/'external'), or an
 *   associative array. JavaScript settings should always pass the string
 *   'setting' only. Other types can have the following elements in the array:
 *   - type: The type of JavaScript that is to be added to the page. Allowed
 *     values are 'file', 'inline', 'external' or 'setting'. Defaults
 *     to 'file'.
 *   - scope: The location in which you want to place the script. Possible
 *     values are 'header' or 'footer'. If your theme implements different
 *     regions, you can also use these. Defaults to 'header'.
 *   - group: A number identifying the group in which to add the JavaScript.
 *     Available constants are:
 *     - JS_LIBRARY: Any libraries, settings, or jQuery plugins.
 *     - JS_DEFAULT: Any module-layer JavaScript.
 *     - JS_THEME: Any theme-layer JavaScript.
 *     The group number serves as a weight: JavaScript within a lower weight
 *     group is presented on the page before JavaScript within a higher weight
 *     group.
 *   - every_page: For optimal front-end performance when aggregation is
 *     enabled, this should be set to TRUE if the JavaScript is present on every
 *     page of the website for users for whom it is present at all. This
 *     defaults to FALSE. It is set to TRUE for JavaScript files that are added
 *     via module and theme .info files. Modules that add JavaScript within
 *     hook_init() implementations, or from other code that ensures that the
 *     JavaScript is added to all website pages, should also set this flag to
 *     TRUE. All JavaScript files within the same group and that have the
 *     'every_page' flag set to TRUE and do not have 'preprocess' set to FALSE
 *     are aggregated together into a single aggregate file, and that aggregate
 *     file can be reused across a user's entire site visit, leading to faster
 *     navigation between pages. However, JavaScript that is only needed on
 *     pages less frequently visited, can be added by code that only runs for
 *     those particular pages, and that code should not set the 'every_page'
 *     flag. This minimizes the size of the aggregate file that the user needs
 *     to download when first visiting the website. JavaScript without the
 *     'every_page' flag is aggregated into a separate aggregate file. This
 *     other aggregate file is likely to change from page to page, and each new
 *     aggregate file needs to be downloaded when first encountered, so it
 *     should be kept relatively small by ensuring that most commonly needed
 *     JavaScript is added to every page.
 *   - weight: A number defining the order in which the JavaScript is added to
 *     the page relative to other JavaScript with the same 'scope', 'group',
 *     and 'every_page' value. In some cases, the order in which the JavaScript
 *     is presented on the page is very important. jQuery, for example, must be
 *     added to the page before any jQuery code is run, so jquery.js uses the
 *     JS_LIBRARY group and a weight of -20, jquery.once.js (a library drupal.js
 *     depends on) uses the JS_LIBRARY group and a weight of -19, drupal.js uses
 *     the JS_LIBRARY group and a weight of -1, other libraries use the
 *     JS_LIBRARY group and a weight of 0 or higher, and all other scripts use
 *     one of the other group constants. The exact ordering of JavaScript is as
 *     follows:
 *     - First by scope, with 'header' first, 'footer' last, and any other
 *       scopes provided by a custom theme coming in between, as determined by
 *       the theme.
 *     - Then by group.
 *     - Then by the 'every_page' flag, with TRUE coming before FALSE.
 *     - Then by weight.
 *     - Then by the order in which the JavaScript was added. For example, all
 *       else being the same, JavaScript added by a call to drupal_add_js() that
 *       happened later in the page request gets added to the page after one for
 *       which drupal_add_js() happened earlier in the page request.
 *   - requires_jquery: Set this to FALSE if the JavaScript you are adding does
 *     not have a dependency on jQuery. Defaults to TRUE, except for JavaScript
 *     settings where it defaults to FALSE. This is used on sites that have the
 *     'javascript_always_use_jquery' variable set to FALSE; on those sites, if
 *     all the JavaScript added to the page by drupal_add_js() does not have a
 *     dependency on jQuery, then for improved front-end performance Drupal
 *     will not add jQuery and related libraries and settings to the page.
 *   - defer: If set to TRUE, the defer attribute is set on the <script>
 *     tag. Defaults to FALSE.
 *   - cache: If set to FALSE, the JavaScript file is loaded anew on every page
 *     call; in other words, it is not cached. Used only when 'type' references
 *     a JavaScript file. Defaults to TRUE.
 *   - preprocess: If TRUE and JavaScript aggregation is enabled, the script
 *     file will be aggregated. Defaults to TRUE.
 *
 * @return
 *   The current array of JavaScript files, settings, and in-line code,
 *   including Drupal defaults, anything previously added with calls to
 *   drupal_add_js(), and this function call's additions.
 *
 * @see drupal_get_js()
 */
function drupal_add_js($data = NULL, $options = NULL) {
         // D6 Compat: $data = NULL, $type = 'module', $scope = 'header', $defer = FALSE, $cache = TRUE, $preprocess = TRUE
  $javascript = &drupal_static(__FUNCTION__, array());
  $jquery_added = &drupal_static(__FUNCTION__ . ':jquery_added', FALSE);

  // If the $javascript variable has been reset with drupal_static_reset(),
  // jQuery and related files will have been removed from the list, so set the
  // variable back to FALSE to indicate they have not yet been added.
  if (empty($javascript)) {
    $jquery_added = FALSE;
  }

  // Construct the options, taking the defaults into consideration.
  if (isset($options)) {
    if (!is_array($options)) {
      $options = array('type' => $options);

      // Drupal6 compatibility
      $args = func_get_args();
      if (isset($args[2])) {
        $options['media'] = $args[2];
      }
      if (isset($args[3]) && !empty($args[3])) {
        $options['attributes']['defer'] = 'defer';
      }
      if (isset($args[4])) {
        $options['cache'] = $args[4];
      }
      if (isset($args[5])) {
        $options['preprocess'] = $args[5];
      }

      if ($options['type'] == 'theme') {
        $options['group'] = JS_THEME;
        $options['every_page'] = TRUE;
      }
      else if ($options['type'] == 'core') {
        $options['type'] = 'file';
      }
    }
  }
  else {
    $options = array();
  }
  if (isset($options['type']) && $options['type'] == 'setting') {
    $options += array('requires_jquery' => FALSE);
  }
  $options += drupal_js_defaults($data);

  // Preprocess can only be set if caching is enabled.
  $options['preprocess'] = $options['cache'] ? $options['preprocess'] : FALSE;

  // Tweak the weight so that files of the same weight are included in the
  // order of the calls to drupal_add_js().
  $options['weight'] += count($javascript) / 1000;

  if (isset($data)) {
    // Add jquery.js, drupal.js, and related files and settings if they have
    // not been added yet. However, if the 'javascript_always_use_jquery'
    // variable is set to FALSE (indicating that the site does not want jQuery
    // automatically added on all pages) then only add it if a file or setting
    // that requires jQuery is being added also.
    if (!$jquery_added && (variable_get('javascript_always_use_jquery', TRUE) || $options['requires_jquery'])) {
      $jquery_added = TRUE;
      // url() generates the prefix using hook_url_outbound_alter(). Instead of
      // running the hook_url_outbound_alter() again here, extract the prefix
      // from url().
      url('', array('prefix' => &$prefix));
      $default_javascript = array(
        'settings' => array(
          'data' => array(
            array('basePath' => base_path()),
            array('pathPrefix' => empty($prefix) ? '' : $prefix),
          ),
          'type' => 'setting',
          'scope' => 'header',
          'group' => JS_LIBRARY,
          'every_page' => TRUE,
          'weight' => 0,
        ),
        'misc/drupal.js' => array(
          'data' => 'misc/drupal.js',
          'type' => 'file',
          'scope' => 'header',
          'group' => JS_LIBRARY,
          'every_page' => TRUE,
          'weight' => -1,
          'requires_jquery' => TRUE,
          'preprocess' => TRUE,
          'cache' => TRUE,
          'defer' => FALSE,
        ),
      );
      $javascript = drupal_array_merge_deep($javascript, $default_javascript);
      // Register all required libraries.
      drupal_add_library('system', 'jquery', TRUE);
      drupal_add_library('system', 'jquery.once', TRUE);
    }

    switch ($options['type']) {
      case 'setting':
        // All JavaScript settings are placed in the header of the page with
        // the library weight so that inline scripts appear afterwards.
        $javascript['settings']['data'][] = $data;
        break;

      case 'inline':
        $javascript[] = $options;
        break;

      default: // 'file' and 'external'
        // Local and external files must keep their name as the associative key
        // so the same JavaScript file is not added twice.
        $javascript[$options['data']] = $options;
    }
  }
  return $javascript;
}
/*

  static $javascript = array();

  if (isset($data)) {

    // Add jquery.js and drupal.js, as well as the basePath setting, the
    // first time a Javascript file is added.
    if (empty($javascript)) {
      $javascript['header'] = array(
        'core' => array(
          'misc/jquery.js' => array('cache' => TRUE, 'defer' => FALSE, 'preprocess' => TRUE),
          'misc/drupal.js' => array('cache' => TRUE, 'defer' => FALSE, 'preprocess' => TRUE),
        ),
        'module' => array(),
        'theme' => array(),
        'setting' => array(
          array('basePath' => base_path()),
        ),
        'inline' => array(),
      );
    }

    if (isset($scope) && !isset($javascript[$scope])) {
      $javascript[$scope] = array('core' => array(), 'module' => array(), 'theme' => array(), 'setting' => array(), 'inline' => array());
    }

    if (isset($type) && isset($scope) && !isset($javascript[$scope][$type])) {
      $javascript[$scope][$type] = array();
    }

    switch ($type) {
      case 'setting':
        $javascript[$scope][$type][] = $data;
        break;
      case 'inline':
        $javascript[$scope][$type][] = array('code' => $data, 'defer' => $defer);
        break;
      default:
        // If cache is FALSE, don't preprocess the JS file.
        $javascript[$scope][$type][$data] = array('cache' => $cache, 'defer' => $defer, 'preprocess' => (!$cache ? FALSE : $preprocess));
    }
  }

  if (isset($scope)) {

    if (isset($javascript[$scope])) {
      return $javascript[$scope];
    }
    else {
      return array();
    }
  }
  else {
    return $javascript;
  }
}*/

/**
 * Constructs an array of the defaults that are used for JavaScript items.
 * Backport from D7.
 *
 * @param $data
 *   (optional) The default data parameter for the JavaScript item array.
 *
 * @see drupal_get_js()
 * @see drupal_add_js()
 */
function drupal_js_defaults($data = NULL) {
  return array(
    'type' => 'file',
    'group' => JS_DEFAULT,
    'every_page' => FALSE,
    'weight' => 0,
    'requires_jquery' => TRUE,
    'scope' => 'header',
    'cache' => TRUE,
    'defer' => FALSE,
    'preprocess' => TRUE,
    'version' => NULL,
    'data' => $data,
  );
}

/**
 * Returns a themed presentation of all JavaScript code for the current page.
 * Backport from D7.
 *
 * References to JavaScript files are placed in a certain order: first, all
 * 'core' files, then all 'module' and finally all 'theme' JavaScript files
 * are added to the page. Then, all settings are output, followed by 'inline'
 * JavaScript code. If running update.php, all preprocessing is disabled.
 *
 * Note that hook_js_alter(&$javascript) is called during this function call
 * to allow alterations of the JavaScript during its presentation. Calls to
 * drupal_add_js() from hook_js_alter() will not be added to the output
 * presentation. The correct way to add JavaScript during hook_js_alter()
 * is to add another element to the $javascript array, deriving from
 * drupal_js_defaults(). See locale_js_alter() for an example of this.
 *
 * @param $scope
 *   (optional) The scope for which the JavaScript rules should be returned.
 *   Defaults to 'header'.
 * @param $javascript
 *   (optional) An array with all JavaScript code. Defaults to the default
 *   JavaScript array for the given scope.
 * @param $skip_alter
 *   (optional) If set to TRUE, this function skips calling drupal_alter() on
 *   $javascript, useful when the calling function passes a $javascript array
 *   that has already been altered.
 *
 * @return
 *   All JavaScript code segments and includes for the scope as HTML tags.
 *
 * @see drupal_add_js()
 * @see locale_js_alter()
 * @see drupal_js_defaults()
 */
function drupal_get_js($scope = 'header', $javascript = NULL, $skip_alter = FALSE) {
  if (!isset($javascript)) {
    $javascript = drupal_add_js();
  }

  // If no JavaScript items have been added, or if the only JavaScript items
  // that have been added are JavaScript settings (which don't do anything
  // without any JavaScript code to use them), then no JavaScript code should
  // be added to the page.
  if (empty($javascript) || (isset($javascript['settings']) && count($javascript) == 1)) {
    return '';
  }

  // Allow modules to alter the JavaScript.
  if (!$skip_alter) {
    drupal_alter('js', $javascript);
  }

  // Filter out elements of the given scope.
  $items = array();
  foreach ($javascript as $key => $item) {
    if ($item['scope'] == $scope) {
      $items[$key] = $item;
    }
  }

  $output = '';
  // The index counter is used to keep aggregated and non-aggregated files in
  // order by weight.
  $index = 1;
  $processed = array();
  $files = array();
  $preprocess_js = (variable_get('preprocess_js', FALSE) && (!defined('MAINTENANCE_MODE') || MAINTENANCE_MODE != 'update'));

  // A dummy query-string is added to filenames, to gain control over
  // browser-caching. The string changes on every update or full cache
  // flush, forcing browsers to load a new copy of the files, as the
  // URL changed. Files that should not be cached (see drupal_add_js())
  // get REQUEST_TIME as query-string instead, to enforce reload on every
  // page request.
  $default_query_string = variable_get('css_js_query_string', '0');

  // For inline JavaScript to validate as XHTML, all JavaScript containing
  // XHTML needs to be wrapped in CDATA. To make that backwards compatible
  // with HTML 4, we need to comment out the CDATA-tag.
  $embed_prefix = "\n<!--//--><![CDATA[//><!--\n";
  $embed_suffix = "\n//--><!]]>\n";

  // Since JavaScript may look for arguments in the URL and act on them, some
  // third-party code might require the use of a different query string.
  $js_version_string = variable_get('drupal_js_version_query_string', 'v=');

  // Sort the JavaScript so that it appears in the correct order.
  uasort($items, 'drupal_sort_css_js');

  // Provide the page with information about the individual JavaScript files
  // used, information not otherwise available when aggregation is enabled.
  $setting['ajaxPageState']['js'] = array_fill_keys(array_keys($items), 1);
  unset($setting['ajaxPageState']['js']['settings']);
  drupal_add_js($setting, 'setting');

  // If we're outputting the header scope, then this might be the final time
  // that drupal_get_js() is running, so add the setting to this output as well
  // as to the drupal_add_js() cache. If $items['settings'] doesn't exist, it's
  // because drupal_get_js() was intentionally passed a $javascript argument
  // stripped off settings, potentially in order to override how settings get
  // output, so in this case, do not add the setting to this output.
  if ($scope == 'header' && isset($items['settings'])) {
    $items['settings']['data'][] = $setting;
  }

  // Loop through the JavaScript to construct the rendered output.
  $element = array(
    '#tag' => 'script',
    '#value' => '',
    '#attributes' => array(
      'type' => 'text/javascript',
    ),
  );
  foreach ($items as $item) {
    $query_string =  empty($item['version']) ? $default_query_string : $js_version_string . $item['version'];

    switch ($item['type']) {
      case 'setting':
        $js_element = $element;
        $js_element['#value_prefix'] = $embed_prefix;
        $js_element['#value'] = 'jQuery.extend(Drupal.settings, ' . drupal_json_encode(drupal_array_merge_deep_array($item['data'])) . ");";
        $js_element['#value_suffix'] = $embed_suffix;
        $output .= theme('html_tag', $js_element);
        break;

      case 'inline':
        $js_element = $element;
        if ($item['defer']) {
          $js_element['#attributes']['defer'] = 'defer';
        }
        $js_element['#value_prefix'] = $embed_prefix;
        $js_element['#value'] = $item['data'];
        $js_element['#value_suffix'] = $embed_suffix;
        $processed[$index++] = theme('html_tag', $js_element);
        break;

      case 'file':
        $js_element = $element;
        if (!$item['preprocess'] || !$preprocess_js) {
          if ($item['defer']) {
            $js_element['#attributes']['defer'] = 'defer';
          }
          $query_string_separator = (strpos($item['data'], '?') !== FALSE) ? '&' : '?';
          $js_element['#attributes']['src'] = base_path() . $item['data'] . $query_string_separator . ($item['cache'] ? $query_string : REQUEST_TIME);
          $processed[$index++] = theme('html_tag', $js_element);
        }
        else {
          // By increasing the index for each aggregated file, we maintain
          // the relative ordering of JS by weight. We also set the key such
          // that groups are split by items sharing the same 'group' value and
          // 'every_page' flag. While this potentially results in more aggregate
          // files, it helps make each one more reusable across a site visit,
          // leading to better front-end performance of a website as a whole.
          // See drupal_add_js() for details.
          $key = 'aggregate_' . $item['group'] . '_' . $item['every_page'] . '_' . $index;
          $processed[$key] = '';
          $files[$key][$item['data']] = $item;
        }
        break;

      case 'external':
        $js_element = $element;
        // Preprocessing for external JavaScript files is ignored.
        if ($item['defer']) {
          $js_element['#attributes']['defer'] = 'defer';
        }
        $js_element['#attributes']['src'] = $item['data'];
        $processed[$index++] = theme('html_tag', $js_element);
        break;
    }
  }

  // Aggregate any remaining JS files that haven't already been output.
  if ($preprocess_js && count($files) > 0) {
    foreach ($files as $key => $file_set) {
      $uri = drupal_build_js_cache($file_set);
      // Only include the file if was written successfully. Errors are logged
      // using watchdog.
      if ($uri) {
        $preprocess_file = file_create_url($uri);
        $js_element = $element;
        $js_element['#attributes']['src'] = $preprocess_file;
        $processed[$key] = theme('html_tag', $js_element);
      }
    }
  }

  // Keep the order of JS files consistent as some are preprocessed and others are not.
  // Make sure any inline or JS setting variables appear last after libraries have loaded.
  return implode('', $processed) . $output;
}

/**
 * Adds attachments to a render() structure. Backport from D7.
 *
 * Libraries, JavaScript, CSS and other types of custom structures are attached
 * to elements using the #attached property. The #attached property is an
 * associative array, where the keys are the attachment types and the values are
 * the attached data. For example:
 * @code
 * $build['#attached'] = array(
 *   'js' => array(drupal_get_path('module', 'taxonomy') . '/taxonomy.js'),
 *   'css' => array(drupal_get_path('module', 'taxonomy') . '/taxonomy.css'),
 * );
 * @endcode
 *
 * 'js', 'css', and 'library' are types that get special handling. For any
 * other kind of attached data, the array key must be the full name of the
 * callback function and each value an array of arguments. For example:
 * @code
 * $build['#attached']['drupal_add_http_header'] = array(
 *   array('Content-Type', 'application/rss+xml; charset=utf-8'),
 * );
 * @endcode
 *
 * External 'js' and 'css' files can also be loaded. For example:
 * @code
 * $build['#attached']['js'] = array(
 *   'http://code.jquery.com/jquery-1.4.2.min.js' => array(
 *     'type' => 'external',
 *   ),
 * );
 * @endcode
 *
 * @param $elements
 *   The structured array describing the data being rendered.
 * @param $group
 *   The default group of JavaScript and CSS being added. This is only applied
 *   to the stylesheets and JavaScript items that don't have an explicit group
 *   assigned to them.
 * @param $dependency_check
 *   When TRUE, will exit if a given library's dependencies are missing. When
 *   set to FALSE, will continue to add the libraries, even though one or more
 *   dependencies are missing. Defaults to FALSE.
 * @param $every_page
 *   Set to TRUE to indicate that the attachments are added to every page on the
 *   site. Only attachments with the every_page flag set to TRUE can participate
 *   in JavaScript/CSS aggregation.
 *
 * @return
 *   FALSE if there were any missing library dependencies; TRUE if all library
 *   dependencies were met.
 *
 * @see drupal_add_library()
 * @see drupal_add_js()
 * @see drupal_add_css()
 * @see drupal_render()
 */
function drupal_process_attached($elements, $group = JS_DEFAULT, $dependency_check = FALSE, $every_page = NULL) {
  // Add defaults to the special attached structures that should be processed differently.
  $elements['#attached'] += array(
    'library' => array(),
    'js' => array(),
    'css' => array(),
  );

  // Add the libraries first.
  $success = TRUE;
  foreach ($elements['#attached']['library'] as $library) {
    if (drupal_add_library($library[0], $library[1], $every_page) === FALSE) {
      $success = FALSE;
      // Exit if the dependency is missing.
      if ($dependency_check) {
        return $success;
      }
    }
  }
  unset($elements['#attached']['library']);

  // Add both the JavaScript and the CSS.
  // The parameters for drupal_add_js() and drupal_add_css() require special
  // handling.
  foreach (array('js', 'css') as $type) {
    foreach ($elements['#attached'][$type] as $data => $options) {
      // If the value is not an array, it's a filename and passed as first
      // (and only) argument.
      if (!is_array($options)) {
        $data = $options;
        $options = NULL;
      }
      // In some cases, the first parameter ($data) is an array. Arrays can't be
      // passed as keys in PHP, so we have to get $data from the value array.
      if (is_numeric($data)) {
        $data = $options['data'];
        unset($options['data']);
      }
      // Apply the default group if it isn't explicitly given.
      if (!isset($options['group'])) {
        $options['group'] = $group;
      }
      // Set the every_page flag if one was passed.
      if (isset($every_page)) {
        $options['every_page'] = $every_page;
      }
      call_user_func('drupal_add_' . $type, $data, $options);
    }
    unset($elements['#attached'][$type]);
  }

  // Add additional types of attachments specified in the render() structure.
  // Libraries, JavaScript and CSS have been added already, as they require
  // special handling.
  foreach ($elements['#attached'] as $callback => $options) {
    if (function_exists($callback)) {
      foreach ($elements['#attached'][$callback] as $args) {
        call_user_func_array($callback, $args);
      }
    }
  }

  return $success;
}

/**
 * Adds multiple JavaScript or CSS files at the same time. Backport from D7.
 *
 * A library defines a set of JavaScript and/or CSS files, optionally using
 * settings, and optionally requiring another library. For example, a library
 * can be a jQuery plugin, a JavaScript framework, or a CSS framework. This
 * function allows modules to load a library defined/shipped by itself or a
 * depending module, without having to add all files of the library separately.
 * Each library is only loaded once.
 *
 * @param $module
 *   The name of the module that registered the library.
 * @param $name
 *   The name of the library to add.
 * @param $every_page
 *   Set to TRUE if this library is added to every page on the site. Only items
 *   with the every_page flag set to TRUE can participate in aggregation.
 *
 * @return
 *   TRUE if the library was successfully added; FALSE if the library or one of
 *   its dependencies could not be added.
 *
 * @see drupal_get_library()
 * @see hook_library()
 * @see hook_library_alter()
 */
function drupal_add_library($module, $name, $every_page = NULL) {
  $added = &drupal_static(__FUNCTION__, array());

  // Only process the library if it exists and it was not added already.
  if (!isset($added[$module][$name])) {
    if ($library = drupal_get_library($module, $name)) {
      // Add all components within the library.
      $elements['#attached'] = array(
        'library' => $library['dependencies'],
        'js' => $library['js'],
        'css' => $library['css'],
      );
      $added[$module][$name] = drupal_process_attached($elements, JS_LIBRARY, TRUE, $every_page);
    }
    else {
      // Requested library does not exist.
      $added[$module][$name] = FALSE;
    }
  }

  return $added[$module][$name];
}

/**
 * Retrieves information for a JavaScript/CSS library. Backport from D7.
 *
 * Library information is statically cached. Libraries are keyed by module for
 * several reasons:
 * - Libraries are not unique. Multiple modules might ship with the same library
 *   in a different version or variant. This registry cannot (and does not
 *   attempt to) prevent library conflicts.
 * - Modules implementing and thereby depending on a library that is registered
 *   by another module can only rely on that module's library.
 * - Two (or more) modules can still register the same library and use it
 *   without conflicts in case the libraries are loaded on certain pages only.
 *
 * @param $module
 *   The name of a module that registered a library.
 * @param $name
 *   (optional) The name of a registered library to retrieve. By default, all
 *   libraries registered by $module are returned.
 *
 * @return
 *   The definition of the requested library, if $name was passed and it exists,
 *   or FALSE if it does not exist. If no $name was passed, an associative array
 *   of libraries registered by $module is returned (which may be empty).
 *
 * @see drupal_add_library()
 * @see hook_library()
 * @see hook_library_alter()
 *
 * @todo The purpose of drupal_get_*() is completely different to other page
 *   requisite API functions; find and use a different name.
 */
function drupal_get_library($module, $name = NULL) {
  $libraries = &drupal_static(__FUNCTION__, array());

  if (!isset($libraries[$module])) {
    // Retrieve all libraries associated with the module.
    $module_libraries = module_invoke($module, 'library');
    if (empty($module_libraries)) {
      $module_libraries = array();
    }
    // Allow modules to alter the module's registered libraries.
    drupal_alter('library', $module_libraries, $module);

    foreach ($module_libraries as $key => $data) {
      if (is_array($data)) {
        // Add default elements to allow for easier processing.
        $module_libraries[$key] += array('dependencies' => array(), 'js' => array(), 'css' => array());
        foreach ($module_libraries[$key]['js'] as $file => $options) {
          $module_libraries[$key]['js'][$file]['version'] = $module_libraries[$key]['version'];
        }
      }
    }
    $libraries[$module] = $module_libraries;
  }
  if (isset($name)) {
    if (!isset($libraries[$module][$name])) {
      $libraries[$module][$name] = FALSE;
    }
    return $libraries[$module][$name];
  }
  return $libraries[$module];
}

/**
 * Assist in adding the tableDrag JavaScript behavior to a themed table.
 *
 * Draggable tables should be used wherever an outline or list of sortable items
 * needs to be arranged by an end-user. Draggable tables are very flexible and
 * can manipulate the value of form elements placed within individual columns.
 *
 * To set up a table to use drag and drop in place of weight select-lists or
 * in place of a form that contains parent relationships, the form must be
 * themed into a table. The table must have an id attribute set. If using
 * theme_table(), the id may be set as such:
 * @code
 * $output = theme('table', $header, $rows, array('id' => 'my-module-table'));
 * return $output;
 * @endcode
 *
 * In the theme function for the form, a special class must be added to each
 * form element within the same column, "grouping" them together.
 *
 * In a situation where a single weight column is being sorted in the table, the
 * classes could be added like this (in the theme function):
 * @code
 * $form['my_elements'][$delta]['weight']['#attributes']['class'] = "my-elements-weight";
 * @endcode
 *
 * Each row of the table must also have a class of "draggable" in order to enable the
 * drag handles:
 * @code
 * $row = array(...);
 * $rows[] = array(
 *   'data' => $row,
 *   'class' => 'draggable',
 * );
 * @endcode
 *
 * When tree relationships are present, the two additional classes
 * 'tabledrag-leaf' and 'tabledrag-root' can be used to refine the behavior:
 * - Rows with the 'tabledrag-leaf' class cannot have child rows.
 * - Rows with the 'tabledrag-root' class cannot be nested under a parent row.
 *
 * Calling drupal_add_tabledrag() would then be written as such:
 * @code
 * drupal_add_tabledrag('my-module-table', 'order', 'sibling', 'my-elements-weight');
 * @endcode
 *
 * In a more complex case where there are several groups in one column (such as
 * the block regions on the admin/build/block page), a separate subgroup class
 * must also be added to differentiate the groups.
 * @code
 * $form['my_elements'][$region][$delta]['weight']['#attributes']['class'] = "my-elements-weight my-elements-weight-". $region;
 * @endcode
 *
 * $group is still 'my-element-weight', and the additional $subgroup variable
 * will be passed in as 'my-elements-weight-'. $region. This also means that
 * you'll need to call drupal_add_tabledrag() once for every region added.
 *
 * @code
 * foreach ($regions as $region) {
 *   drupal_add_tabledrag('my-module-table', 'order', 'sibling', 'my-elements-weight', 'my-elements-weight-'. $region);
 * }
 * @endcode
 *
 * In a situation where tree relationships are present, adding multiple
 * subgroups is not necessary, because the table will contain indentations that
 * provide enough information about the sibling and parent relationships.
 * See theme_menu_overview_form() for an example creating a table containing
 * parent relationships.
 *
 * Please note that this function should be called from the theme layer, such as
 * in a .tpl.php file, theme_ function, or in a template_preprocess function,
 * not in a form declartion. Though the same JavaScript could be added to the
 * page using drupal_add_js() directly, this function helps keep template files
 * clean and readable. It also prevents tabledrag.js from being added twice
 * accidentally.
 *
 * @param $table_id
 *   String containing the target table's id attribute. If the table does not
 *   have an id, one will need to be set, such as <table id="my-module-table">.
 * @param $action
 *   String describing the action to be done on the form item. Either 'match'
 *   'depth', or 'order'. Match is typically used for parent relationships.
 *   Order is typically used to set weights on other form elements with the same
 *   group. Depth updates the target element with the current indentation.
 * @param $relationship
 *   String describing where the $action variable should be performed. Either
 *   'parent', 'sibling', 'group', or 'self'. Parent will only look for fields
 *   up the tree. Sibling will look for fields in the same group in rows above
 *   and below it. Self affects the dragged row itself. Group affects the
 *   dragged row, plus any children below it (the entire dragged group).
 * @param $group
 *   A class name applied on all related form elements for this action.
 * @param $subgroup
 *   (optional) If the group has several subgroups within it, this string should
 *   contain the class name identifying fields in the same subgroup.
 * @param $source
 *   (optional) If the $action is 'match', this string should contain the class
 *   name identifying what field will be used as the source value when matching
 *   the value in $subgroup.
 * @param $hidden
 *   (optional) The column containing the field elements may be entirely hidden
 *   from view dynamically when the JavaScript is loaded. Set to FALSE if the
 *   column should not be hidden.
 * @param $limit
 *   (optional) Limit the maximum amount of parenting in this table.
 * @see block-admin-display-form.tpl.php
 * @see theme_menu_overview_form()
 */
function drupal_add_tabledrag($table_id, $action, $relationship, $group, $subgroup = NULL, $source = NULL, $hidden = TRUE, $limit = 0) {
  static $js_added = FALSE;
  if (!$js_added) {
    drupal_add_js('misc/tabledrag.js', 'core');
    $js_added = TRUE;
  }

  // If a subgroup or source isn't set, assume it is the same as the group.
  $target = isset($subgroup) ? $subgroup : $group;
  $source = isset($source) ? $source : $target;
  $settings['tableDrag'][$table_id][$group][] = array(
    'target' => $target,
    'source' => $source,
    'relationship' => $relationship,
    'action' => $action,
    'hidden' => $hidden,
    'limit' => $limit,
  );
  drupal_add_js($settings, 'setting');
}

/**
 * Aggregates JavaScript files into a cache file in the files directory.
 * Backport from D7.
 *
 * The file name for the JavaScript cache file is generated from the hash of
 * the aggregated contents of the files in $files. This forces proxies and
 * browsers to download new JavaScript when the JavaScript changes.
 *
 * The cache file name is retrieved on a page load via a lookup variable that
 * contains an associative array. The array key is the hash of the names in
 * $files while the value is the cache file name. The cache file is generated
 * in two cases. First, if there is no file name value for the key, which will
 * happen if a new file name has been added to $files or after the lookup
 * variable is emptied to force a rebuild of the cache. Second, the cache file
 * is generated if it is missing on disk. Old cache files are not deleted
 * immediately when the lookup variable is emptied, but are deleted after a set
 * period by drupal_delete_file_if_stale(). This ensures that files referenced
 * by a cached page will still be available.
 *
 * @param $files
 *   An array of JavaScript files to aggregate and compress into one file.
 *
 * @return
 *   The URI of the cache file, or FALSE if the file could not be saved.
 */
function drupal_build_js_cache($files) {
  $contents = '';
  $uri = '';
  $map = variable_get('drupal_js_cache_files', array());
  // Create a new array so that only the file names are used to create the hash.
  // This prevents new aggregates from being created unnecessarily.
  $js_data = array();
  foreach ($files as $file) {
    $js_data[] = $file['data'];
  }
  $key = hash('sha256', serialize($js_data));
  if (isset($map[$key])) {
    $uri = $map[$key];
  }

  if (empty($uri) || !file_exists($uri)) {
    // Build aggregate JS file.
    foreach ($files as $path => $info) {
      if ($info['preprocess']) {
        // Append a ';' and a newline after each JS file to prevent them from running together.
        $contents .= file_get_contents($path) . ";\n";
      }
    }
    // Prefix filename to prevent blocking by firewalls which reject files
    // starting with "ad*".
    $filename = 'js_' . drupal_hash_base64($contents) . '.js';
    // Create the js/ within the files folder.
    $jspath = file_create_path('js');
    file_check_directory($jspath, FILE_CREATE_DIRECTORY);
    $uri = $jspath . '/' . $filename;
    // Create the JS file.
    if (!file_exists($uri) && !file_save_data($contents, $uri, FILE_EXISTS_REPLACE)) {
      return FALSE;
    }
    // If JS gzip compression is enabled, clean URLs are enabled (which means
    // that rewrite rules are working) and the zlib extension is available then
    // create a gzipped version of this file. This file is served conditionally
    // to browsers that accept gzip using .htaccess rules.
    if (variable_get('js_gzip_compression', TRUE) && variable_get('clean_url', 0) && extension_loaded('zlib')) {
      if (!file_exists($uri . '.gz') && !file_save_data(gzencode($contents, 9, FORCE_GZIP), $uri . '.gz', FILE_EXISTS_REPLACE)) {
        return FALSE;
      }
    }
    $map[$key] = $uri;
    variable_set('drupal_js_cache_files', $map);
  }
  return $uri;
}

/**
 * Delete all cached JS files.
 */
function drupal_clear_js_cache() {
  variable_set('javascript_parsed', array());
  variable_del('drupal_js_cache_files');
  file_scan_directory(file_create_path('js'), '.*', array('.', '..', 'CVS'), 'file_delete', TRUE);
}

/**
 * Converts a PHP variable into its Javascript equivalent. 
 * Left for D6 compatibility.
 *
 * We use HTML-safe strings, i.e. with <, > and & escaped.
 */
function drupal_to_js($var) {
  return drupal_json_encode($var);
}

/**
 * Converts a PHP variable into its JavaScript equivalent. Backport from D7.
 *
 * We use HTML-safe strings, with several characters escaped.
 *
 * @see drupal_json_decode()
 * @see drupal_json_encode_helper()
 * @ingroup php_wrappers
 */
function drupal_json_encode($var) {
  // The PHP version cannot change within a request.
  static $php530;

  if (!isset($php530)) {
    $php530 = version_compare(PHP_VERSION, '5.3.0', '>=');
  }

  if ($php530) {
    // Encode <, >, ', &, and " using the json_encode() options parameter.
    return json_encode($var, JSON_HEX_TAG | JSON_HEX_APOS | JSON_HEX_AMP | JSON_HEX_QUOT);
  }

  // json_encode() escapes <, >, ', &, and " using its options parameter, but
  // does not support this parameter prior to PHP 5.3.0.  Use a helper instead.
  include_once DRUPAL_ROOT . '/includes/json-encode.inc';
  return drupal_json_encode_helper($var);
}

/**
 * Converts an HTML-safe JSON string into its PHP equivalent. Backport from D7.
 *
 * @see drupal_json_encode()
 * @ingroup php_wrappers
 */
function drupal_json_decode($var) {
  return json_decode($var, TRUE);
}

/**
 * Return data in JSON format.
 *
 * This function should be used for JavaScript callback functions returning
 * data in JSON format. It sets the header for JavaScript output.
 *
 * @param $var
 *   (optional) If set, the variable will be converted to JSON and output.
 */
function drupal_json($var = NULL) {
  // We are returning JSON, so tell the browser.
  drupal_set_header('Content-Type: application/json');

  if (isset($var)) {
    echo drupal_to_js($var);
  }
}

/**
 * Wrapper around urlencode() which avoids Apache quirks.
 *
 * Should be used when placing arbitrary data in an URL. Note that Drupal paths
 * are urlencoded() when passed through url() and do not require urlencoding()
 * of individual components.
 *
 * Notes:
 * - For esthetic reasons, we do not escape slashes. This also avoids a 'feature'
 *   in Apache where it 404s on any path containing '%2F'.
 * - mod_rewrite unescapes %-encoded ampersands, hashes, and slashes when clean
 *   URLs are used, which are interpreted as delimiters by PHP. These
 *   characters are double escaped so PHP will still see the encoded version.
 * - With clean URLs, Apache changes '//' to '/', so every second slash is
 *   double escaped.
 * - This function should only be used on paths, not on query string arguments,
 *   otherwise unwanted double encoding will occur.
 *
 * @param $text
 *   String to encode
 */
function drupal_urlencode($text) {
  if (variable_get('clean_url', '0')) {
    return str_replace(array('%2F', '%26', '%23', '//'),
                       array('/', '%2526', '%2523', '/%252F'),
                       rawurlencode($text));
  }
  else {
    return str_replace('%2F', '/', rawurlencode($text));
  }
}

/**
 * Ensure the private key variable used to generate tokens is set.
 *
 * @return
 *   The private key.
 */
function drupal_get_private_key() {
  if (!($key = variable_get('drupal_private_key', 0))) {
    $key = drupal_random_key();
    variable_set('drupal_private_key', $key);
  }
  return $key;
}

/**
 * Generate a token based on $value, the current user session and private key.
 *
 * @param $value
 *   An additional value to base the token on.
 */
function drupal_get_token($value = '') {
  $private_key = drupal_get_private_key();
  return md5(session_id() . $value . $private_key);
}

/**
 * Validate a token based on $value, the current user session and private key.
 *
 * @param $token
 *   The token to be validated.
 * @param $value
 *   An additional value to base the token on.
 * @param $skip_anonymous
 *   Set to true to skip token validation for anonymous users.
 * @return
 *   True for a valid token, false for an invalid token. When $skip_anonymous
 *   is true, the return value will always be true for anonymous users.
 */
function drupal_valid_token($token, $value = '', $skip_anonymous = FALSE) {
  global $user;
  return (($skip_anonymous && $user->uid == 0) || ($token === md5(session_id() . $value . variable_get('drupal_private_key', ''))));
}

/**
 * Performs one or more XML-RPC request(s).
 *
 * @param $url
 *   An absolute URL of the XML-RPC endpoint.
 *     Example:
 *     http://www.example.com/xmlrpc.php
 * @param ...
 *   For one request:
 *     The method name followed by a variable number of arguments to the method.
 *   For multiple requests (system.multicall):
 *     An array of call arrays. Each call array follows the pattern of the single
 *     request: method name followed by the arguments to the method.
 * @return
 *   For one request:
 *     Either the return value of the method on success, or FALSE.
 *     If FALSE is returned, see xmlrpc_errno() and xmlrpc_error_msg().
 *   For multiple requests:
 *     An array of results. Each result will either be the result
 *     returned by the method called, or an xmlrpc_error object if the call
 *     failed. See xmlrpc_error().
 */
function xmlrpc($url) {
  require_once './includes/xmlrpc.inc';
  $args = func_get_args();
  return call_user_func_array('_xmlrpc', $args);
}

function _drupal_bootstrap_full() {
  static $called;

  if ($called) {
    return;
  }
  $called = 1;
  require_once './includes/theme.inc';
  require_once './includes/pager.inc';
  require_once './includes/menu.inc';
  require_once './includes/tablesort.inc';
  require_once './includes/file.inc';
  require_once './includes/unicode.inc';
  require_once './includes/image.inc';
  require_once './includes/form.inc';
  require_once './includes/mail.inc';
  require_once './includes/actions.inc';
  // Set the Drupal custom error handler.
  set_error_handler('_drupal_error_handler');
  set_exception_handler('_drupal_exception_handler');
  // Emit the correct charset HTTP header.
  drupal_set_header('Content-Type: text/html; charset=utf-8');
  // Detect string handling method
  unicode_check();
  // Undo magic quotes
  fix_gpc_magic();

  if (isset($_SERVER['HTTP_USER_AGENT']) && strpos($_SERVER['HTTP_USER_AGENT'], 'simpletest') !== FALSE) {
    // Valid SimpleTest user-agent, log fatal errors to test specific file
    // directory. The user-agent is validated in DRUPAL_BOOTSTRAP_DATABASE
    // phase so as long as it is a SimpleTest user-agent it is valid.
    ini_set('log_errors', 1);
    ini_set('error_log', file_directory_path() . '/error.log');
  }

  // Load all enabled modules
  module_load_all();
  // Ensure mt_rand is reseeded, to prevent random values from one page load
  // being exploited to predict random values in subsequent page loads.
  $seed = unpack("L", drupal_random_bytes(4));
  mt_srand($seed[1]);

  // Initialize $_GET['q'] prior to invoking hook_init().
  drupal_init_path();

  // Let all modules take action before menu system handles the request
  // We do not want this while running update.php.
  if (!defined('MAINTENANCE_MODE') || MAINTENANCE_MODE != 'update') {
    module_invoke_all('init');
  }
}

/**
 * Stores the current page in the cache. Backport from D7.
 *
 * If page_compression is enabled, a gzipped version of the page is stored in
 * the cache to avoid compressing the output on each request. The cache entry
 * is unzipped in the relatively rare event that the page is requested by a
 * client without gzip support.
 *
 * Page compression requires the PHP zlib extension
 * (http://php.net/manual/ref.zlib.php).
 *
 * @see drupal_page_header()
 */
function drupal_page_set_cache() {
  global $base_root;

  if (drupal_page_is_cacheable()) {

    // Check whether the current page might be compressed.
    $page_compressed = variable_get('page_compression', TRUE) && extension_loaded('zlib');

    $cache = (object) array(
      'cid' => $base_root . request_uri(),
      'data' => array(
        'path' => $_GET['q'],
        'body' => ob_get_clean(),
        'title' => drupal_get_title(),
        'headers' => array(),
        // We need to store whether page was compressed or not,
        // because by the time it is read, the configuration might change.
        'page_compressed' => $page_compressed,
      ),
      'expire' => CACHE_TEMPORARY,
      'created' => REQUEST_TIME,
    );

    // Restore preferred header names based on the lower-case names returned
    // by drupal_get_http_header().
    $header_names = _drupal_set_preferred_header_name();
    foreach (drupal_get_http_header() as $name_lower => $value) {
      $cache->data['headers'][$header_names[$name_lower]] = $value;
      if ($name_lower == 'expires') {
        // Use the actual timestamp from an Expires header if available.
        $cache->expire = strtotime($value);
      }
    }

    if ($cache->data['body']) {
      if ($page_compressed) {
        $cache->data['body'] = gzencode($cache->data['body'], 9, FORCE_GZIP);
      }
      cache_set($cache->cid, $cache->data, 'cache_page', $cache->expire);
    }
    return $cache;
  }
}

/**
 * Executes a cron run when called
 * @return
 * Returns TRUE if ran successfully
 */
function drupal_cron_run() {
  // Try to allocate enough time to run all the hook_cron implementations.
  if (function_exists('set_time_limit')) {
    @set_time_limit(240);
  }

  // Fetch the cron semaphore
  $semaphore = variable_get('cron_semaphore', FALSE);

  if ($semaphore) {
    if (time() - $semaphore > 3600) {
      // Either cron has been running for more than an hour or the semaphore
      // was not reset due to a database error.
      watchdog('cron', 'Cron has been running for more than an hour and is most likely stuck.', array(), WATCHDOG_ERROR);

      // Release cron semaphore
      variable_del('cron_semaphore');
    }
    else {
      // Cron is still running normally.
      watchdog('cron', 'Attempting to re-run cron while it is already running.', array(), WATCHDOG_WARNING);
    }
  }
  else {
    // Register shutdown callback
    drupal_register_shutdown_function('drupal_cron_cleanup');

    // Lock cron semaphore
    variable_set('cron_semaphore', time());

    // Iterate through the modules calling their cron handlers (if any):
    foreach (module_implements('cron') as $module) {
      // Do not let an exception thrown by one module disturb another.
      try {
        module_invoke($module, 'cron');
      }
      catch (Exception $e) {
        watchdog('cron', $e, array(), WATCHDOG_ERROR);
      }
    }

    // Record cron time
    variable_set('cron_last', time());
    watchdog('cron', 'Cron run completed.', array(), WATCHDOG_NOTICE);

    // Release cron semaphore
    variable_del('cron_semaphore');

    // Return TRUE so other functions can check if it did run successfully
    return TRUE;
  }
}

/**
 * Shutdown function for cron cleanup.
 */
function drupal_cron_cleanup() {
  // See if the semaphore is still locked.
  if (variable_get('cron_semaphore', FALSE)) {
    watchdog('cron', 'Cron run exceeded the time limit and was aborted.', array(), WATCHDOG_WARNING);

    // Release cron semaphore
    variable_del('cron_semaphore');
  }
}

/**
 * Returns information about system object files (modules, themes, etc.).
 *
 * This function is used to find all or some system object files (module files,
 * theme files, etc.) that exist on the site. It searches in several locations,
 * depending on what type of object you are looking for. For instance, if you
 * are looking for modules and call:
 * @code
 * drupal_system_listing("/\.module$/", "modules", 'name', 0);
 * @endcode
 * this function will search the site-wide modules directory (i.e., /modules/),
 * your installation profile's directory (i.e.,
 * /profiles/your_site_profile/modules/), the all-sites directory (i.e.,
 * /sites/all/modules/), and your site-specific directory (i.e.,
 * /sites/your_site_dir/modules/), in that order, and return information about
 * all of the files ending in .module in those directories.
 *
 * The information is returned in an associative array, which can be keyed on
 * the file name ($key = 'filename'), the file name without the extension ($key
 * = 'name'), or the full file stream URI ($key = 'uri'). If you use a key of
 * 'filename' or 'name', files found later in the search will take precedence
 * over files found earlier (unless they belong to a module or theme not
 * compatible with Drupal core); if you choose a key of 'uri', you will get all
 * files found.
 *
 * @param string $mask
 *   The preg_match() regular expression for the files to find.
 * @param string $directory
 *   The subdirectory name in which the files are found. For example,
 *   'modules' will search in sub-directories of the top-level /modules
 *   directory, sub-directories of /sites/all/modules/, etc.
 * @param string $key
 *   The key to be used for the associative array returned. Possible values are
 *   'uri', for the file's URI; 'filename', for the basename of the file; and
 *   'name' for the name of the file without the extension. If you choose 'name'
 *   or 'filename', only the highest-precedence file will be returned.
 * @param int $min_depth
 *   Minimum depth of directories to return files from, relative to each
 *   directory searched. For instance, a minimum depth of 2 would find modules
 *   inside /modules/node/tests, but not modules directly in /modules/node.
 *
 * @return array
 *   An associative array of file objects, keyed on the chosen key. Each element
 *   in the array is an object containing file information, with properties:
 *   - 'uri': Full URI of the file.
 *   - 'filename': File name.
 *   - 'name': Name of file without the extension.
 */
function drupal_system_listing($mask, $directory, $key = 'name', $min_depth = 1) {
  $config = conf_path();

  $searchdir = array($directory);
  $files = array();

  // The 'profiles' directory contains pristine collections of modules and
  // themes as organized by a distribution. It is pristine in the same way
  // that /modules is pristine for core; users should avoid changing anything
  // there in favor of sites/all or sites/<domain> directories.
  $profiles = array();
  $profile = drupal_get_profile();
  // For SimpleTest to be able to test modules packaged together with a
  // distribution we need to include the profile of the parent site (in which
  // test runs are triggered).
  if (drupal_valid_test_ua($_SERVER['HTTP_USER_AGENT'])) {
    $testing_profile = variable_get('simpletest_parent_profile', FALSE);
    if ($testing_profile && $testing_profile != $profile) {
      $profiles[] = $testing_profile;
    }
  }
  // In case both profile directories contain the same extension, the actual
  // profile always has precedence.
  $profiles[] = $profile;
  foreach ($profiles as $profile) {
    if (file_exists("profiles/$profile/$directory")) {
      $searchdir[] = "profiles/$profile/$directory";
    }
  }

  // Always search sites/all/* as well as the global directories.
  $searchdir[] = 'sites/all/' . $directory;

  if (file_exists("$config/$directory")) {
    $searchdir[] = "$config/$directory";
  }

  // Get current list of items.
  if (!function_exists('file_scan_directory')) {
    require_once DRUPAL_ROOT . '/includes/file.inc';
  }
  foreach ($searchdir as $dir) {
    $files_to_add = file_scan_directory($dir, $mask, array('key' => $key, 'min_depth' => $min_depth));

    // Duplicate files found in later search directories take precedence over
    // earlier ones, so we want them to overwrite keys in our resulting
    // $files array.
    // The exception to this is if the later file is from a module or theme not
    // compatible with Drupal core. This may occur during upgrades of Drupal
    // core when new modules exist in core while older contrib modules with the
    // same name exist in a directory such as sites/all/modules/.
    foreach (array_intersect_key($files_to_add, $files) as $file_key => $file) {
      // If it has no info file, then we just behave liberally and accept the
      // new resource on the list for merging.
      if (file_exists($info_file = dirname($file->filename) . '/' . $file->name . '.info')) {
        // Get the .info file for the module or theme this file belongs to.
        $info = drupal_parse_info_file($info_file);

        // If the module or theme is incompatible with Drupal core, remove it
        // from the array for the current search directory, so it is not
        // overwritten when merged with the $files array.
        if (isset($info['core']) && $info['core'] != DRUPAL_CORE_COMPATIBILITY) {
          unset($files_to_add[$file_key]);
        }
      }
    }
    $files = array_merge($files, $files_to_add);
  }

  return $files;
}

/**
 * #pre_render callback to render #browsers into #prefix and #suffix.
 * Backport from D7.
 *
 * @param $elements
 *   A render array with a '#browsers' property. The '#browsers' property can
 *   contain any or all of the following keys:
 *   - 'IE': If FALSE, the element is not rendered by Internet Explorer. If
 *     TRUE, the element is rendered by Internet Explorer. Can also be a string
 *     containing an expression for Internet Explorer to evaluate as part of a
 *     conditional comment. For example, this can be set to 'lt IE 7' for the
 *     element to be rendered in Internet Explorer 6, but not in Internet
 *     Explorer 7 or higher. Defaults to TRUE.
 *   - '!IE': If FALSE, the element is not rendered by browsers other than
 *     Internet Explorer. If TRUE, the element is rendered by those browsers.
 *     Defaults to TRUE.
 *   Examples:
 *   - To render an element in all browsers, '#browsers' can be left out or set
 *     to array('IE' => TRUE, '!IE' => TRUE).
 *   - To render an element in Internet Explorer only, '#browsers' can be set
 *     to array('!IE' => FALSE).
 *   - To render an element in Internet Explorer 6 only, '#browsers' can be set
 *     to array('IE' => 'lt IE 7', '!IE' => FALSE).
 *   - To render an element in Internet Explorer 8 and higher and in all other
 *     browsers, '#browsers' can be set to array('IE' => 'gte IE 8').
 *
 * @return
 *   The passed-in element with markup for conditional comments potentially
 *   added to '#prefix' and '#suffix'.
 */
function drupal_pre_render_conditional_comments($elements) {
  $browsers = isset($elements['#browsers']) ? $elements['#browsers'] : array();
  $browsers += array(
    'IE' => TRUE,
    '!IE' => TRUE,
  );

  // If rendering in all browsers, no need for conditional comments.
  if ($browsers['IE'] === TRUE && $browsers['!IE']) {
    return $elements;
  }

  // Determine the conditional comment expression for Internet Explorer to
  // evaluate.
  if ($browsers['IE'] === TRUE) {
    $expression = 'IE';
  }
  elseif ($browsers['IE'] === FALSE) {
    $expression = '!IE';
  }
  else {
    $expression = $browsers['IE'];
  }

  // Wrap the element's potentially existing #prefix and #suffix properties with
  // conditional comment markup. The conditional comment expression is evaluated
  // by Internet Explorer only. To control the rendering by other browsers,
  // either the "downlevel-hidden" or "downlevel-revealed" technique must be
  // used. See http://en.wikipedia.org/wiki/Conditional_comment for details.
  $elements += array(
    '#prefix' => '',
    '#suffix' => '',
  );
  if (!$browsers['!IE']) {
    // "downlevel-hidden".
    $elements['#prefix'] = "\n<!--[if $expression]>\n" . $elements['#prefix'];
    $elements['#suffix'] .= "<![endif]-->\n";
  }
  else {
    // "downlevel-revealed".
    $elements['#prefix'] = "\n<!--[if $expression]><!-->\n" . $elements['#prefix'];
    $elements['#suffix'] .= "<!--<![endif]-->\n";
  }

  return $elements;
}

/**
 * #pre_render callback to append contents in #markup to #children.
 *
 * This needs to be a #pre_render callback, because eventually assigned
 * #theme_wrappers will expect the element's rendered content in #children.
 * Note that if also a #theme is defined for the element, then the result of
 * the theme callback will override #children.
 *
 * @param $elements
 *   A structured array using the #markup key.
 *
 * @return
 *   The passed-in elements, but #markup appended to #children.
 *
 * @see drupal_render()
 */
function drupal_pre_render_markup($elements) {
  $elements['#children'] = isset($elements['#markup'])? $elements['#markup']: '';
  return $elements;
}

/**
 * Passes alterable variables to specific hook_TYPE_alter() implementations.
 *
 * This dispatch function hands off the passed-in variables to type-specific
 * hook_TYPE_alter() implementations in modules. It ensures a consistent
 * interface for all altering operations.
 *
 * A maximum of 2 alterable arguments is supported (a third is supported for
 * legacy reasons, but should not be used in new code). In case more arguments
 * need to be passed and alterable, modules provide additional variables
 * assigned by reference in the last $context argument:
 * @code
 *   $context = array(
 *     'alterable' => &$alterable,
 *     'unalterable' => $unalterable,
 *     'foo' => 'bar',
 *   );
 *   drupal_alter('mymodule_data', $alterable1, $alterable2, $context);
 * @endcode
 *
 * Note that objects are always passed by reference in PHP5. If it is absolutely
 * required that no implementation alters a passed object in $context, then an
 * object needs to be cloned:
 * @code
 *   $context = array(
 *     'unalterable_object' => clone $object,
 *   );
 *   drupal_alter('mymodule_data', $data, $context);
 * @endcode
 *
 * @param $type
 *   A string describing the type of the alterable $data. 'form', 'links',
 *   'node_content', and so on are several examples. Alternatively can be an
 *   array, in which case hook_TYPE_alter() is invoked for each value in the
 *   array, ordered first by module, and then for each module, in the order of
 *   values in $type. For example, when Form API is using drupal_alter() to
 *   execute both hook_form_alter() and hook_form_FORM_ID_alter()
 *   implementations, it passes array('form', 'form_' . $form_id) for $type.
 * @param $data
 *   The variable that will be passed to hook_TYPE_alter() implementations to be
 *   altered. The type of this variable depends on the value of the $type
 *   argument. For example, when altering a 'form', $data will be a structured
 *   array. When altering a 'profile', $data will be an object.
 * @param $context1
 *   (optional) An additional variable that is passed by reference.
 * @param $context2
 *   (optional) An additional variable that is passed by reference. If more
 *   context needs to be provided to implementations, then this should be an
 *   associative array as described above.
 * @param $context3
 *   (optional) An additional variable that is passed by reference. This
 *   parameter is deprecated and will not exist in Drupal 8; consequently, it
 *   should not be used for new Drupal 7 code either. It is here only for
 *   backwards compatibility with older code that passed additional arguments
 *   to drupal_alter().
 */
function drupal_alter($type, &$data, &$context1 = NULL, &$context2 = NULL, &$context3 = NULL) {
  // Use the advanced drupal_static() pattern, since this is called very often.
  static $drupal_static_fast;
  if (!isset($drupal_static_fast)) {
    $drupal_static_fast['functions'] = &drupal_static(__FUNCTION__);
  }
  $functions = &$drupal_static_fast['functions'];

  // Most of the time, $type is passed as a string, so for performance,
  // normalize it to that. When passed as an array, usually the first item in
  // the array is a generic type, and additional items in the array are more
  // specific variants of it, as in the case of array('form', 'form_FORM_ID').
  if (is_array($type)) {
    $cid = implode(',', $type);
    $extra_types = $type;
    $type = array_shift($extra_types);
    // Allow if statements in this function to use the faster isset() rather
    // than !empty() both when $type is passed as a string, or as an array with
    // one item.
    if (empty($extra_types)) {
      unset($extra_types);
    }
  }
  else {
    $cid = $type;
  }

  // Some alter hooks are invoked many times per page request, so statically
  // cache the list of functions to call, and on subsequent calls, iterate
  // through them quickly.
  if (!isset($functions[$cid])) {
    $functions[$cid] = array();
    $hook = $type . '_alter';
    $modules = module_implements($hook);
    if (!isset($extra_types)) {
      // For the more common case of a single hook, we do not need to call
      // function_exists(), since module_implements() returns only modules with
      // implementations.
      foreach ($modules as $module) {
        $functions[$cid][] = $module . '_' . $hook;
      }
    }
    else {
      // For multiple hooks, we need $modules to contain every module that
      // implements at least one of them.
      $extra_modules = array();
      foreach ($extra_types as $extra_type) {
        $extra_modules = array_merge($extra_modules, module_implements($extra_type . '_alter'));
      }
      // If any modules implement one of the extra hooks that do not implement
      // the primary hook, we need to add them to the $modules array in their
      // appropriate order. module_implements() can only return ordered
      // implementations of a single hook. To get the ordered implementations
      // of multiple hooks, we mimic the module_implements() logic of first
      // ordering by module_list(), and then calling
      // drupal_alter('module_implements').
      if (array_diff($extra_modules, $modules)) {
        // Merge the arrays and order by module_list().
        $modules = array_intersect(module_list(), array_merge($modules, $extra_modules));
        // Since module_implements() already took care of loading the necessary
        // include files, we can safely pass FALSE for the array values.
        $implementations = array_fill_keys($modules, FALSE);
        // Let modules adjust the order solely based on the primary hook. This
        // ensures the same module order regardless of whether this if block
        // runs. Calling drupal_alter() recursively in this way does not result
        // in an infinite loop, because this call is for a single $type, so we
        // won't end up in this code block again.
        drupal_alter('module_implements', $implementations, $hook);
        $modules = array_keys($implementations);
      }
      foreach ($modules as $module) {
        // Since $modules is a merged array, for any given module, we do not
        // know whether it has any particular implementation, so we need a
        // function_exists().
        $function = $module . '_' . $hook;
        if (function_exists($function)) {
          $functions[$cid][] = $function;
        }
        foreach ($extra_types as $extra_type) {
          $function = $module . '_' . $extra_type . '_alter';
          if (function_exists($function)) {
            $functions[$cid][] = $function;
          }
        }
      }
    }
    // Allow the theme to alter variables after the theme system has been
    // initialized.
    global $theme, $base_theme_info;
    if (isset($theme)) {
      $theme_keys = array();
      if (isset($base_theme_info)) {
        foreach ($base_theme_info as $base) {
          $theme_keys[] = $base->name;
        }
      }
      $theme_keys[] = $theme;
      foreach ($theme_keys as $theme_key) {
        $function = $theme_key . '_' . $hook;
        if (function_exists($function)) {
          $functions[$cid][] = $function;
        }
        if (isset($extra_types)) {
          foreach ($extra_types as $extra_type) {
            $function = $theme_key . '_' . $extra_type . '_alter';
            if (function_exists($function)) {
              $functions[$cid][] = $function;
            }
          }
        }
      }
    }
  }

  foreach ($functions[$cid] as $function) {
    $function($data, $context1, $context2, $context3);
  }
}


/**
 * Renders HTML given a structured array tree.
 *
 * Recursively iterates over each of the array elements, generating HTML code.
 * This function is usually called from within another function, like
 * drupal_get_form() or node_view().
 *
 * drupal_render() flags each element with a '#printed' status to indicate that
 * the element has been rendered, which allows individual elements of a given
 * array to be rendered independently. This prevents elements from being
 * rendered more than once on subsequent calls to drupal_render() if, for example,
 * they are part of a larger array. If the same array or array element is passed
 * more than once to drupal_render(), it simply returns a NULL value.
 *
 * @param $elements
 *   The structured array describing the data to be rendered.
 * @return
 *   The rendered HTML.
 */
function drupal_render(&$elements) {
  // Early-return nothing if user does not have access.
  if (empty($elements) || (isset($elements['#access']) && !$elements['#access'])) {
    return '';
  }

  // Do not print elements twice.
  if (!empty($elements['#printed'])) {
    return '';
  }

  // If #markup is set, ensure #type is set. This allows to specify just #markup
  // on an element without setting #type.
  if (isset($elements['#markup']) && !isset($elements['#type'])) {
    $elements['#type'] = 'markup';
  }

  // If the default values for this element haven't been loaded yet, populate
  // them.
  if (!isset($elements['#defaults_loaded']) || !$elements['#defaults_loaded']) {
    if ((!empty($elements['#type'])) && ($info = _element_info($elements['#type']))) {
      $elements += $info;
    }
  }

  // Make any final changes to the element before it is rendered. This means
  // that the $element or the children can be altered or corrected before the
  // element is rendered into the final text.
  if (isset($elements['#pre_render'])) {
    foreach ($elements['#pre_render'] as $function) {
      if (function_exists($function)) {
        $elements = $function($elements);
      }
    }
  }

  // Allow #pre_render to abort rendering.
  if (!empty($elements['#printed'])) {
    return '';
  }

  $content = '';
  // Either the elements did not go through form_builder or one of the children
  // has a #weight.
  if (!isset($elements['#sorted'])) {
    uasort($elements, "element_sort");
  }
  $elements += array('#title' => NULL, '#description' => NULL);
  if (!isset($elements['#children'])) {
    $children = element_children($elements, TRUE);
    // Render all the children that use a theme function.
    if (isset($elements['#theme']) && empty($elements['#theme_used'])) {
      $elements['#theme_used'] = TRUE;

      $previous = array();
      foreach (array('#value', '#type', '#prefix', '#suffix') as $key) {
        $previous[$key] = isset($elements[$key]) ? $elements[$key] : NULL;
      }
      // If we rendered a single element, then we will skip the renderer.
      if (empty($children)) {
        $elements['#printed'] = TRUE;
      }
      else {
        $elements['#value'] = '';
      }
      $elements['#type'] = 'markup';

      unset($elements['#prefix'], $elements['#suffix']);
      $content = theme($elements['#theme'], $elements);

      foreach (array('#value', '#type', '#prefix', '#suffix') as $key) {
        $elements[$key] = isset($previous[$key]) ? $previous[$key] : NULL;
      }
    }
    // Render each of the children using drupal_render and concatenate them.
    if (!isset($content) || $content === '') {
      foreach ($children as $key) {
        $content .= drupal_render($elements[$key]);
      }
    }
  }
  if (isset($content) && $content !== '') {
    $elements['#children'] = $content;
  }

  // Add additional libraries, CSS, JavaScript an other custom
  // attached data associated with this element.
  // Backport from D7.
  if (!empty($elements['#attached'])) {
    drupal_process_attached($elements);
  }

  // Until now, we rendered the children, here we render the element itself
  if (!isset($elements['#printed'])) {
    $content = theme(!empty($elements['#type']) ? $elements['#type'] : 'markup', $elements);
    $elements['#printed'] = TRUE;
  }
  if (isset($content) && $content !== '') {
    // Filter the outputted content and make any last changes before the
    // content is sent to the browser. The changes are made on $content
    // which allows the output'ed text to be filtered.
    if (isset($elements['#post_render'])) {
      foreach ($elements['#post_render'] as $function) {
        if (function_exists($function)) {
          $content = $function($content, $elements);
        }
      }
    }
    $prefix = isset($elements['#prefix']) ? $elements['#prefix'] : '';
    $suffix = isset($elements['#suffix']) ? $elements['#suffix'] : '';
    return $prefix . $content . $suffix;
  }
}

/**
 * Function used by uasort to sort structured arrays by weight.
 */
function element_sort($a, $b) {
  $a_weight = (is_array($a) && isset($a['#weight'])) ? $a['#weight'] : 0;
  $b_weight = (is_array($b) && isset($b['#weight'])) ? $b['#weight'] : 0;
  if ($a_weight == $b_weight) {
    return 0;
  }
  return ($a_weight < $b_weight) ? -1 : 1;
}

/**
 * Check if the key is a property.
 */
function element_property($key) {
  return $key[0] == '#';
}

/**
 * Get properties of a structured array element. Properties begin with '#'.
 */
function element_properties($element) {
  return array_filter(array_keys((array) $element), 'element_property');
}

/**
 * Check if the key is a child.
 */
function element_child($key) {
  return !isset($key[0]) || $key[0] != '#';
}

/**
 * Identifies the children of an element array, optionally sorted by weight.
 *
 * The children of a element array are those key/value pairs whose key does
 * not start with a '#'. See drupal_render() for details.
 *
 * @param $elements
 *   The element array whose children are to be identified.
 * @param $sort
 *   Boolean to indicate whether the children should be sorted by weight.
 *
 * @return
 *   The array keys of the element's children.
 */
function element_children(&$elements, $sort = FALSE) {
  // Do not attempt to sort elements which have already been sorted.
  $sort = isset($elements['#sorted']) ? !$elements['#sorted'] : $sort;

  // Filter out properties from the element, leaving only children.
  $children = array();
  $sortable = FALSE;
  foreach ($elements as $key => $value) {
    if ($key === '' || $key[0] !== '#') {
      $children[$key] = $value;
      if (is_array($value) && isset($value['#weight'])) {
        $sortable = TRUE;
      }
    }
  }
  // Sort the children if necessary.
  if ($sort && $sortable) {
    uasort($children, 'element_sort');
    // Put the sorted children back into $elements in the correct order, to
    // preserve sorting if the same element is passed through
    // element_children() twice.
    foreach ($children as $key => $child) {
      unset($elements[$key]);
      $elements[$key] = $child;
    }
    $elements['#sorted'] = TRUE;
  }

  return array_keys($children);
}

/**
 * Sets HTML attributes based on element properties. Backport from D7.
 *
 * @param $element
 *   The renderable element to process.
 * @param $map
 *   An associative array whose keys are element property names and whose values
 *   are the HTML attribute names to set for corresponding the property; e.g.,
 *   array('#propertyname' => 'attributename'). If both names are identical
 *   except for the leading '#', then an attribute name value is sufficient and
 *   no property name needs to be specified.
 */
function element_set_attributes(array &$element, array $map) {
  foreach ($map as $property => $attribute) {
    // If the key is numeric, the attribute name needs to be taken over.
    if (is_int($property)) {
      $property = '#' . $attribute;
    }
    // Do not overwrite already existing attributes.
    if (isset($element[$property]) && !isset($element['#attributes'][$attribute])) {
      $element['#attributes'][$attribute] = $element[$property];
    }
  }
}

/**
 * Sets a value in a nested array with variable depth.
 *
 * This helper function should be used when the depth of the array element you
 * are changing may vary (that is, the number of parent keys is variable). It
 * is primarily used for form structures and renderable arrays.
 *
 * Example:
 * @code
 * // Assume you have a 'signature' element somewhere in a form. It might be:
 * $form['signature_settings']['signature'] = array(
 *   '#type' => 'text_format',
 *   '#title' => t('Signature'),
 * );
 * // Or, it might be further nested:
 * $form['signature_settings']['user']['signature'] = array(
 *   '#type' => 'text_format',
 *   '#title' => t('Signature'),
 * );
 * @endcode
 *
 * To deal with the situation, the code needs to figure out the route to the
 * element, given an array of parents that is either
 * @code array('signature_settings', 'signature') @endcode in the first case or
 * @code array('signature_settings', 'user', 'signature') @endcode in the second
 * case.
 *
 * Without this helper function the only way to set the signature element in one
 * line would be using eval(), which should be avoided:
 * @code
 * // Do not do this! Avoid eval().
 * eval('$form[\'' . implode("']['", $parents) . '\'] = $element;');
 * @endcode
 *
 * Instead, use this helper function:
 * @code
 * drupal_array_set_nested_value($form, $parents, $element);
 * @endcode
 *
 * However if the number of array parent keys is static, the value should always
 * be set directly rather than calling this function. For instance, for the
 * first example we could just do:
 * @code
 * $form['signature_settings']['signature'] = $element;
 * @endcode
 *
 * @param $array
 *   A reference to the array to modify.
 * @param $parents
 *   An array of parent keys, starting with the outermost key.
 * @param $value
 *   The value to set.
 * @param $force
 *   (Optional) If TRUE, the value is forced into the structure even if it
 *   requires the deletion of an already existing non-array parent value. If
 *   FALSE, PHP throws an error if trying to add into a value that is not an
 *   array. Defaults to FALSE.
 *
 * @see drupal_array_get_nested_value()
 */
function drupal_array_set_nested_value(array &$array, array $parents, $value, $force = FALSE) {
  $ref = &$array;
  foreach ($parents as $parent) {
    // PHP auto-creates container arrays and NULL entries without error if $ref
    // is NULL, but throws an error if $ref is set, but not an array.
    if ($force && isset($ref) && !is_array($ref)) {
      $ref = array();
    }
    $ref = &$ref[$parent];
  }
  $ref = $value;
}

/**
 * Retrieves a value from a nested array with variable depth.
 * Backport from D7.
 *
 * This helper function should be used when the depth of the array element being
 * retrieved may vary (that is, the number of parent keys is variable). It is
 * primarily used for form structures and renderable arrays.
 *
 * Without this helper function the only way to get a nested array value with
 * variable depth in one line would be using eval(), which should be avoided:
 * @code
 * // Do not do this! Avoid eval().
 * // May also throw a PHP notice, if the variable array keys do not exist.
 * eval('$value = $array[\'' . implode("']['", $parents) . "'];");
 * @endcode
 *
 * Instead, use this helper function:
 * @code
 * $value = drupal_array_get_nested_value($form, $parents);
 * @endcode
 *
 * A return value of NULL is ambiguous, and can mean either that the requested
 * key does not exist, or that the actual value is NULL. If it is required to
 * know whether the nested array key actually exists, pass a third argument that
 * is altered by reference:
 * @code
 * $key_exists = NULL;
 * $value = drupal_array_get_nested_value($form, $parents, $key_exists);
 * if ($key_exists) {
 *   // ... do something with $value ...
 * }
 * @endcode
 *
 * However if the number of array parent keys is static, the value should always
 * be retrieved directly rather than calling this function. For instance:
 * @code
 * $value = $form['signature_settings']['signature'];
 * @endcode
 *
 * @param $array
 *   The array from which to get the value.
 * @param $parents
 *   An array of parent keys of the value, starting with the outermost key.
 * @param $key_exists
 *   (optional) If given, an already defined variable that is altered by
 *   reference.
 *
 * @return
 *   The requested nested value. Possibly NULL if the value is NULL or not all
 *   nested parent keys exist. $key_exists is altered by reference and is a
 *   Boolean that indicates whether all nested parent keys exist (TRUE) or not
 *   (FALSE). This allows to distinguish between the two possibilities when NULL
 *   is returned.
 *
 * @see drupal_array_set_nested_value()
 */
function &drupal_array_get_nested_value(array &$array, array $parents, &$key_exists = NULL) {
  $ref = &$array;
  foreach ($parents as $parent) {
    if (is_array($ref) && array_key_exists($parent, $ref)) {
      $ref = &$ref[$parent];
    }
    else {
      $key_exists = FALSE;
      $null = NULL;
      return $null;
    }
  }
  $key_exists = TRUE;
  return $ref;
}

/**
 * Provide theme registration for themes across .inc files.
 */
function drupal_common_theme() {
  return array(
    // theme.inc
    'placeholder' => array(
      'arguments' => array('text' => NULL)
    ),
    'page' => array(
      'arguments' => array('content' => NULL, 'show_blocks' => TRUE, 'show_messages' => TRUE),
      'template' => 'page',
    ),
    'maintenance_page' => array(
      'arguments' => array('content' => NULL, 'show_blocks' => TRUE, 'show_messages' => TRUE),
      'template' => 'maintenance-page',
    ),
    'update_page' => array(
      'arguments' => array('content' => NULL, 'show_messages' => TRUE),
    ),
    'install_page' => array(
      'arguments' => array('content' => NULL),
    ),
    'task_list' => array(
      'arguments' => array('items' => NULL, 'active' => NULL),
    ),
    'status_messages' => array(
      'arguments' => array('display' => NULL),
    ),
    'links' => array(
      'arguments' => array('links' => NULL, 'attributes' => array('class' => 'links')),
    ),
    'image' => array(
      'arguments' => array('path' => NULL, 'alt' => '', 'title' => '', 'attributes' => NULL, 'getsize' => TRUE),
    ),
    'breadcrumb' => array(
      'arguments' => array('breadcrumb' => NULL),
    ),
    'help' => array(
      'arguments' => array(),
    ),
    'submenu' => array(
      'arguments' => array('links' => NULL),
    ),
    'table' => array(
      'arguments' => array('header' => NULL, 'rows' => NULL, 'attributes' => array(), 'caption' => NULL),
    ),
    'table_select_header_cell' => array(
      'arguments' => array(),
    ),
    'tablesort_indicator' => array(
      'arguments' => array('style' => NULL),
    ),
    'box' => array(
      'arguments' => array('title' => NULL, 'content' => NULL, 'region' => 'main'),
      'template' => 'box',
    ),
    'block' => array(
      'arguments' => array('block' => NULL),
      'template' => 'block',
    ),
    'mark' => array(
      'arguments' => array('type' => MARK_NEW),
    ),
    'item_list' => array(
      'arguments' => array('items' => array(), 'title' => NULL, 'type' => 'ul', 'attributes' => NULL),
    ),
    'more_help_link' => array(
      'arguments' => array('url' => NULL),
    ),
    'xml_icon' => array(
      'arguments' => array('url' => NULL),
    ),
    'feed_icon' => array(
      'arguments' => array('url' => NULL, 'title' => NULL),
    ),
    'more_link' => array(
      'arguments' => array('url' => NULL, 'title' => NULL)
    ),
    'closure' => array(
      'arguments' => array('main' => 0),
    ),
    'blocks' => array(
      'arguments' => array('region' => NULL),
    ),
    'username' => array(
      'arguments' => array('object' => NULL),
    ),
    'progress_bar' => array(
      'arguments' => array('percent' => NULL, 'message' => NULL),
    ),
    'indentation' => array(
      'arguments' => array('size' => 1),
    ),
    // Backport from D7
    'html_tag' => array(
      'arguments' => array('element' => NULL),
    ),
    'styles' => array(
      'arguments' => array('styles' => NULL),
    ),
    // from pager.inc
    'pager' => array(
      'arguments' => array('tags' => array(), 'limit' => 10, 'element' => 0, 'parameters' => array()),
    ),
    'pager_first' => array(
      'arguments' => array('text' => NULL, 'limit' => NULL, 'element' => 0, 'parameters' => array()),
    ),
    'pager_previous' => array(
      'arguments' => array('text' => NULL, 'limit' => NULL, 'element' => 0, 'interval' => 1, 'parameters' => array()),
    ),
    'pager_next' => array(
      'arguments' => array('text' => NULL, 'limit' => NULL, 'element' => 0, 'interval' => 1, 'parameters' => array()),
    ),
    'pager_last' => array(
      'arguments' => array('text' => NULL, 'limit' => NULL, 'element' => 0, 'parameters' => array()),
    ),
    'pager_link' => array(
      'arguments' => array('text' => NULL, 'page_new' => NULL, 'element' => NULL, 'parameters' => array(), 'attributes' => array()),
    ),
    // from menu.inc
    'menu_item_link' => array(
      'arguments' => array('item' => NULL),
    ),
    'menu_tree' => array(
      'arguments' => array('tree' => NULL),
    ),
    'menu_item' => array(
      'arguments' => array('link' => NULL, 'has_children' => NULL, 'menu' => ''),
    ),
    'menu_local_task' => array(
      'arguments' => array('link' => NULL, 'active' => FALSE),
    ),
    'menu_local_tasks' => array(
      'arguments' => array(),
    ),
    // from form.inc
    'select' => array(
      'arguments' => array('element' => NULL),
    ),
    'fieldset' => array(
      'arguments' => array('element' => NULL),
    ),
    'radio' => array(
      'arguments' => array('element' => NULL),
    ),
    'radios' => array(
      'arguments' => array('element' => NULL),
    ),
    'password_confirm' => array(
      'arguments' => array('element' => NULL),
    ),
    'date' => array(
      'arguments' => array('element' => NULL),
    ),
    'item' => array(
      'arguments' => array('element' => NULL),
    ),
    'checkbox' => array(
      'arguments' => array('element' => NULL),
    ),
    'checkboxes' => array(
      'arguments' => array('element' => NULL),
    ),
    'submit' => array(
      'arguments' => array('element' => NULL),
    ),
    'button' => array(
      'arguments' => array('element' => NULL),
    ),
    'image_button' => array(
      'arguments' => array('element' => NULL),
    ),
    'hidden' => array(
      'arguments' => array('element' => NULL),
    ),
    'token' => array(
      'arguments' => array('element' => NULL),
    ),
    'textfield' => array(
      'arguments' => array('element' => NULL),
    ),
    'form' => array(
      'arguments' => array('element' => NULL),
    ),
    'textarea' => array(
      'arguments' => array('element' => NULL),
    ),
    'markup' => array(
      'arguments' => array('element' => NULL),
    ),
    'password' => array(
      'arguments' => array('element' => NULL),
    ),
    'file' => array(
      'arguments' => array('element' => NULL),
    ),
    'form_element' => array(
      'arguments' => array('element' => NULL, 'value' => NULL),
    ),
  );
}

/**
 * @ingroup schemaapi
 * @{
 */

/**
 * Get the schema definition of a table, or the whole database schema.
 *
 * The returned schema will include any modifications made by any
 * module that implements hook_schema_alter().
 *
 * @param $table
 *   The name of the table. If not given, the schema of all tables is returned.
 * @param $rebuild
 *   If true, the schema will be rebuilt instead of retrieved from the cache.
 */
function drupal_get_schema($table = NULL, $rebuild = FALSE) {
  static $schema = array();

  if (empty($schema) || $rebuild) {
    // Try to load the schema from cache.
    if (!$rebuild && $cached = cache_get('schema')) {
      $schema = $cached->data;
    }
    // Otherwise, rebuild the schema cache.
    else {
      $schema = array();
      // Load the .install files to get hook_schema.
      module_load_all_includes('install');

      // Invoke hook_schema for all modules.
      foreach (module_implements('schema') as $module) {
        // Cast the result of hook_schema() to an array, as a NULL return value
        // would cause array_merge() to set the $schema variable to NULL as well.
        // That would break modules which use $schema further down the line.
        $current = (array) module_invoke($module, 'schema');
        _drupal_initialize_schema($module, $current);
        $schema = array_merge($schema, $current);
      }

      drupal_alter('schema', $schema);
      cache_set('schema', $schema);
    }
  }

  if (!isset($table)) {
    return $schema;
  }
  elseif (isset($schema[$table])) {
    return $schema[$table];
  }
  else {
    return FALSE;
  }
}

/**
 * Create all tables that a module defines in its hook_schema().
 *
 * Note: This function does not pass the module's schema through
 * hook_schema_alter(). The module's tables will be created exactly as the
 * module defines them.
 *
 * @param $module
 *   The module for which the tables will be created.
 * @return
 *   An array of arrays with the following key/value pairs:
 *    - success: a boolean indicating whether the query succeeded.
 *    - query: the SQL query(s) executed, passed through check_plain().
 */
function drupal_install_schema($module) {
  $schema = drupal_get_schema_unprocessed($module);
  _drupal_initialize_schema($module, $schema);

  $ret = array();
  foreach ($schema as $name => $table) {
    db_create_table($ret, $name, $table);
  }
  return $ret;
}

/**
 * Remove all tables that a module defines in its hook_schema().
 *
 * Note: This function does not pass the module's schema through
 * hook_schema_alter(). The module's tables will be created exactly as the
 * module defines them.
 *
 * @param $module
 *   The module for which the tables will be removed.
 * @return
 *   An array of arrays with the following key/value pairs:
 *    - success: a boolean indicating whether the query succeeded.
 *    - query: the SQL query(s) executed, passed through check_plain().
 */
function drupal_uninstall_schema($module) {
  $schema = drupal_get_schema_unprocessed($module);
  _drupal_initialize_schema($module, $schema);

  $ret = array();
  foreach ($schema as $table) {
    db_drop_table($ret, $table['name']);
  }
  return $ret;
}

/**
 * Returns the unprocessed and unaltered version of a module's schema.
 *
 * Use this function only if you explicitly need the original
 * specification of a schema, as it was defined in a module's
 * hook_schema(). No additional default values will be set,
 * hook_schema_alter() is not invoked and these unprocessed
 * definitions won't be cached.
 *
 * This function can be used to retrieve a schema specification in
 * hook_schema(), so it allows you to derive your tables from existing
 * specifications.
 *
 * It is also used by drupal_install_schema() and
 * drupal_uninstall_schema() to ensure that a module's tables are
 * created exactly as specified without any changes introduced by a
 * module that implements hook_schema_alter().
 *
 * @param $module
 *   The module to which the table belongs.
 * @param $table
 *   The name of the table. If not given, the module's complete schema
 *   is returned.
 */
function drupal_get_schema_unprocessed($module, $table = NULL) {
  // Load the .install file to get hook_schema.
  module_load_install($module);
  $schema = module_invoke($module, 'schema');

  if (!is_null($table) && isset($schema[$table])) {
    return $schema[$table];
  }
  elseif (!empty($schema)) {
    return $schema;
  }

  return array();
}

/**
 * Fill in required default values for table definitions returned by hook_schema().
 *
 * @param $module
 *   The module for which hook_schema() was invoked.
 * @param $schema
 *   The schema definition array as it was returned by the module's
 *   hook_schema().
 */
function _drupal_initialize_schema($module, &$schema) {
  // Set the name and module key for all tables.
  foreach ($schema as $name => $table) {
    if (empty($table['module'])) {
      $schema[$name]['module'] = $module;
    }
    if (!isset($table['name'])) {
      $schema[$name]['name'] = $name;
    }
  }
}

/**
 * Retrieve a list of fields from a table schema. The list is suitable for use in a SQL query.
 *
 * @param $table
 *   The name of the table from which to retrieve fields.
 * @param
 *   An optional prefix to to all fields.
 *
 * @return An array of fields.
 **/
function drupal_schema_fields_sql($table, $prefix = NULL) {
  $schema = drupal_get_schema($table);
  $fields = array_keys($schema['fields']);
  if ($prefix) {
    $columns = array();
    foreach ($fields as $field) {
      $columns[] = "$prefix.$field";
    }
    return $columns;
  }
  else {
    return $fields;
  }
}

/**
 * Save a record to the database based upon the schema.
 *
 * Default values are filled in for missing items, and 'serial' (auto increment)
 * types are filled in with IDs.
 *
 * @param $table
 *   The name of the table; this must exist in schema API.
 * @param $object
 *   The object to write. This is a reference, as defaults according to
 *   the schema may be filled in on the object, as well as ID on the serial
 *   type(s). Both array an object types may be passed.
 * @param $primary_keys
 *   To indicate that this is a new record to be inserted, omit this argument.
 *   If this is an update, this argument specifies the primary keys' field
 *   names. If there is only 1 field in the key, you may pass in a string; if
 *   there are multiple fields in the key, pass in an array.
 *
 * @return
 *   Failure to write a record will return FALSE. Otherwise SAVED_NEW or
 *   SAVED_UPDATED is returned depending on the operation performed. The
 *   $object parameter contains values for any serial fields defined by
 *   the $table. For example, $object->nid will be populated after inserting
 *   a new node.
 */
function drupal_write_record($table, &$object, $primary_keys = array()) {
  // Standardize $primary_keys to an array.
  if (is_string($primary_keys)) {
    $primary_keys = array($primary_keys);
  }

  $schema = drupal_get_schema($table);
  if (empty($schema)) {
    return FALSE;
  }

  // Convert to an object if needed.
  if (is_array($object)) {
    $object = (object) $object;
    $array = TRUE;
  }
  else {
    $array = FALSE;
  }

  $fields = $values = $serials = $placeholders = array();

  // Go through our schema, build SQL, and when inserting, fill in defaults for
  // fields that are not set.
  foreach ($schema['fields'] as $field => $info) {
    if ($info['type'] == 'serial') {
      // Skip serial types if we are updating.
      if (!empty($primary_keys)) {
        continue;
      }
      // Track serial field so we can helpfully populate them after the query.
      $serials[] = $field;

      // Unset null primary keys.
      if (property_exists($object, $field) && is_null($object->$field)) {
        continue;
      }
    }

    // Skip field if it is in $primary_keys as it is unnecessary to update a
    // field to the value it is already set to.
    if (in_array($field, $primary_keys)) {
      continue;
    }

    // For inserts, populate defaults from Schema if not already provided
    if (!property_exists($object, $field) && !count($primary_keys) && isset($info['default'])) {
      $object->$field = $info['default'];
    }

    if (!property_exists($object, $field)) {
      // Skip fields that are not provided, default values are already known
      // by the database.
      continue;
    }

    // Build array of fields to update or insert.
    if (empty($info['serialize'])) {
      $fields[$field] = $object->$field;
    }
    else {
      $fields[$field] = serialize($object->$field);
    }

    // Build arrays for the fields, placeholders, and values in our query.
    if (is_null($fields[$field]) && empty($info['not null'])) {
      /// field may take NULL values
      $placeholders[] = '%s';
      $values[] = 'NULL';
    }
    else {
      $placeholders[] = db_type_placeholder($info['type']);
      $values[] = $fields[$field];
    }
  }

  if (empty($fields)) {
    return;
  }

  // Build the SQL.
  $query = '';
  if (empty($primary_keys)) {
    $query = "INSERT INTO {". $table ."} (". implode(', ', array_keys($fields)) .') VALUES ('. implode(', ', $placeholders) .')';
    $return = SAVED_NEW;
  }
  else {
    $query = '';
    foreach (array_keys($fields) as $id => $field) {
      if ($query) {
        $query .= ', ';
      }
      $query .= $field .' = '. $placeholders[$id];
    }

    foreach ($primary_keys as $key){
      $conditions[] = "$key = ". db_type_placeholder($schema['fields'][$key]['type']);
      $values[] = $object->$key;
    }

    $query = "UPDATE {". $table ."} SET $query WHERE ". implode(' AND ', $conditions);
    $return = SAVED_UPDATED;
  }
  // Execute the SQL.
  if (db_query($query, $values)) {
    if (!empty($serials)) {
      // Get last insert ids and fill them in.
      foreach ($serials as $field) {
        // Don't populate primary key in the cases when function is called in 
        // the "Import" mode with non-empty primary key field and empty $primary_keys.
        if ((!isset($object->$field) || empty($object->$field)) && empty($primary_keys)) {
          $object->$field = db_last_insert_id($table, $field);
        }
      }
    }
  }
  else {
    $return = FALSE;
  }

  // If we began with an array, convert back so we don't surprise the caller.
  if ($array) {
    $object = (array) $object;
  }

  return $return;
}

/**
 * @} End of "ingroup schemaapi".
 */

/**
 * Parse Drupal info file format.
 *
 * Files should use an ini-like format to specify values.
 * White-space generally doesn't matter, except inside values.
 * e.g.
 *
 * @code
 *   key = value
 *   key = "value"
 *   key = 'value'
 *   key = "multi-line
 *
 *   value"
 *   key = 'multi-line
 *
 *   value'
 *   key
 *   =
 *   'value'
 * @endcode
 *
 * Arrays are created using a GET-like syntax:
 *
 * @code
 *   key[] = "numeric array"
 *   key[index] = "associative array"
 *   key[index][] = "nested numeric array"
 *   key[index][index] = "nested associative array"
 * @endcode
 *
 * PHP constants are substituted in, but only when used as the entire value:
 *
 * Comments should start with a semi-colon at the beginning of a line.
 *
 * This function is NOT for placing arbitrary module-specific settings. Use
 * variable_get() and variable_set() for that.
 *
 * Information stored in the module.info file:
 * - name: The real name of the module for display purposes.
 * - description: A brief description of the module.
 * - dependencies: An array of shortnames of other modules this module depends on.
 * - package: The name of the package of modules this module belongs to.
 *
 * Example of .info file:
 * @code
 *   name = Forum
 *   description = Enables threaded discussions about general topics.
 *   dependencies[] = taxonomy
 *   dependencies[] = comment
 *   package = Core - optional
 *   version = VERSION
 * @endcode
 *
 * @param $filename
 *   The file we are parsing. Accepts file with relative or absolute path.
 * @return
 *   The info array.
 */
function drupal_parse_info_file($filename) {
  $info = array();
  $constants = get_defined_constants();

  if (!file_exists($filename)) {
    return $info;
  }

  $data = file_get_contents($filename);
  if (preg_match_all('
    @^\s*                           # Start at the beginning of a line, ignoring leading whitespace
    ((?:
      [^=;\[\]]|                    # Key names cannot contain equal signs, semi-colons or square brackets,
      \[[^\[\]]*\]                  # unless they are balanced and not nested
    )+?)
    \s*=\s*                         # Key/value pairs are separated by equal signs (ignoring white-space)
    (?:
      ("(?:[^"]|(?<=\\\\)")*")|     # Double-quoted string, which may contain slash-escaped quotes/slashes
      (\'(?:[^\']|(?<=\\\\)\')*\')| # Single-quoted string, which may contain slash-escaped quotes/slashes
      ([^\r\n]*?)                   # Non-quoted string
    )\s*$                           # Stop at the next end of a line, ignoring trailing whitespace
    @msx', $data, $matches, PREG_SET_ORDER)) {
    foreach ($matches as $match) {
      // Fetch the key and value string
      $i = 0;
      foreach (array('key', 'value1', 'value2', 'value3') as $var) {
        $$var = isset($match[++$i]) ? $match[$i] : '';
      }
      $value = stripslashes(substr($value1, 1, -1)) . stripslashes(substr($value2, 1, -1)) . $value3;

      // Parse array syntax
      $keys = preg_split('/\]?\[/', rtrim($key, ']'));
      $last = array_pop($keys);
      $parent = &$info;

      // Create nested arrays
      foreach ($keys as $key) {
        if ($key == '') {
          $key = count($parent);
        }
        if (!isset($parent[$key]) || !is_array($parent[$key])) {
          $parent[$key] = array();
        }
        $parent = &$parent[$key];
      }

      // Handle PHP constants.
      if (isset($constants[$value])) {
        $value = $constants[$value];
      }

      // Insert actual value
      if ($last == '') {
        $last = count($parent);
      }
      $parent[$last] = $value;
    }
  }

  return $info;
}

/**
 * @return
 *   Array of the possible severity levels for log messages.
 *
 * @see watchdog
 */
function watchdog_severity_levels() {
  return array(
    WATCHDOG_EMERG    => t('emergency'),
    WATCHDOG_ALERT    => t('alert'),
    WATCHDOG_CRITICAL => t('critical'),
    WATCHDOG_ERROR    => t('error'),
    WATCHDOG_WARNING  => t('warning'),
    WATCHDOG_NOTICE   => t('notice'),
    WATCHDOG_INFO     => t('info'),
    WATCHDOG_DEBUG    => t('debug'),
  );
}


/**
 * Explode a string of given tags into an array.
 *
 * @see drupal_implode_tags()
 */
function drupal_explode_tags($tags) {
  // This regexp allows the following types of user input:
  // this, "somecompany, llc", "and ""this"" w,o.rks", foo bar
  $regexp = '%(?:^|,\ *)("(?>[^"]*)(?>""[^"]* )*"|(?: [^",]*))%x';
  preg_match_all($regexp, $tags, $matches);
  $typed_tags = array_unique($matches[1]);

  $tags = array();
  foreach ($typed_tags as $tag) {
    // If a user has escaped a term (to demonstrate that it is a group,
    // or includes a comma or quote character), we remove the escape
    // formatting so to save the term into the database as the user intends.
    $tag = trim(str_replace('""', '"', preg_replace('/^"(.*)"$/', '\1', $tag)));
    if ($tag != "") {
      $tags[] = $tag;
    }
  }

  return $tags;
}

/**
 * Implode an array of tags into a string.
 *
 * @see drupal_explode_tags()
 */
function drupal_implode_tags($tags) {
  $encoded_tags = array();
  foreach ($tags as $tag) {
    // Commas and quotes in tag names are special cases, so encode them.
    if (strpos($tag, ',') !== FALSE || strpos($tag, '"') !== FALSE) {
      $tag = '"'. str_replace('"', '""', $tag) .'"';
    }

    $encoded_tags[] = $tag;
  }
  return implode(', ', $encoded_tags);
}

/**
 * Flush all cached data on the site.
 *
 * Empties cache tables, rebuilds the menu cache and theme registries, and
 * invokes a hook so that other modules' cache data can be cleared as well.
 */
function drupal_flush_all_caches() {
  // Change query-strings on css/js files to enforce reload for all users.
  _drupal_flush_css_js();

  drupal_clear_css_cache();
  drupal_clear_js_cache();

  // If invoked from update.php, we must not update the theme information in the
  // database, or this will result in all themes being disabled.
  if (defined('MAINTENANCE_MODE') && MAINTENANCE_MODE == 'update') {
    _system_theme_data();
  }
  else {
    drupal_static_reset('_system_theme_data');
    drupal_static_reset('system_region_list');
    list_themes(TRUE);
    system_theme_data();
  }

  drupal_rebuild_theme_registry();
  menu_rebuild();
  node_types_rebuild();
  // Don't clear cache_form - in-progress form submissions may break.
  // Ordered so clearing the page cache will always be the last action.
  $core = array('cache', 'cache_block', 'cache_filter', 'cache_page');
  $cache_tables = array_merge(module_invoke_all('flush_caches'), $core);
  foreach ($cache_tables as $table) {
    cache_clear_all('*', $table, TRUE);
  }
}

/**
 * Helper function to change query-strings on css/js files.
 *
 * Changes the character added to all css/js files as dummy query-string,
 * so that all browsers are forced to reload fresh files. We keep
 * 20 characters history (FIFO) to avoid repeats, but only the first
 * (newest) character is actually used on URLs, to keep them short.
 * This is also called from update.php.
 */
function _drupal_flush_css_js() {
  $string_history = variable_get('css_js_query_string', '00000000000000000000');
  $new_character = $string_history[0];
  // Not including 'q' to allow certain JavaScripts to re-use query string.
  $characters = 'abcdefghijklmnoprstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789';
  while (strpos($string_history, $new_character) !== FALSE) {
    $new_character = $characters[mt_rand(0, strlen($characters) - 1)];
  }
  variable_set('css_js_query_string', $new_character . substr($string_history, 0, 19));
}

/**
 * Error reporting level: display no errors.
 */
define('ERROR_REPORTING_HIDE', 0);

/**
 * Error reporting level: display errors and warnings.
 */
define('ERROR_REPORTING_DISPLAY_SOME', 1);

/**
 * Error reporting level: display all messages.
 */
define('ERROR_REPORTING_DISPLAY_ALL', 2);

/**
 * Custom PHP error handler.
 *
 * @param $error_level
 *   The level of the error raised.
 * @param $message
 *   The error message.
 * @param $filename
 *   The filename that the error was raised in.
 * @param $line
 *   The line number the error was raised at.
 * @param $context
 *   An array that points to the active symbol table at the point the error occurred.
 */
function _drupal_error_handler($error_level, $message, $filename, $line, $context) {
  include_once(DRUPAL_ROOT . '/includes/errors.inc');
  if ($error_level & error_reporting()) {
    // All these constants are documented at http://php.net/manual/en/errorfunc.constants.php
    $types = array(
      E_ERROR => 'Error',
      E_WARNING => 'Warning',
      E_PARSE => 'Parse error',
      E_NOTICE => 'Notice',
      E_CORE_ERROR => 'Core error',
      E_CORE_WARNING => 'Core warning',
      E_COMPILE_ERROR => 'Compile error',
      E_COMPILE_WARNING => 'Compile warning',
      E_USER_ERROR => 'User error',
      E_USER_WARNING => 'User warning',
      E_USER_NOTICE => 'User notice',
      E_STRICT => 'Strict warning',
      E_RECOVERABLE_ERROR => 'Recoverable fatal error'
    );
    $caller = _drupal_get_last_caller(debug_backtrace());

    // We treat recoverable errors as fatal.
    _drupal_log_error(array(
      '%type' => isset($types[$error_level]) ? $types[$error_level] : 'Unknown error',
      '%message' => $message,
      '%function' => $caller['function'],
      '%file' => $caller['file'],
      '%line' => $caller['line'],
      'severity_level' => $error_level,
    ), $error_level == E_RECOVERABLE_ERROR);
  }
}

/**
 * Custom PHP exception handler.
 *
 * Uncaught exceptions are those not enclosed in a try/catch block. They are
 * always fatal: the execution of the script will stop as soon as the exception
 * handler exits.
 *
 * @param $exception
 *   The exception object that was thrown.
 */
function _drupal_exception_handler($exception) {
  include_once(DRUPAL_ROOT . '/includes/errors.inc');
  // Log the message to the watchdog and return an error page to the user.
  _drupal_log_error(_drupal_decode_exception($exception), TRUE);
}

/**
 * Debug function used for outputting debug information.
 *
 * The debug information is passed on to trigger_error() after being converted
 * to a string using _drupal_debug_message().
 *
 * @param $data
 *   Data to be output.
 * @param $label
 *   Label to prefix the data.
 * @param $print_r
 *   Flag to switch between print_r() and var_export() for data conversion to
 *   string. Set $print_r to TRUE when dealing with a recursive data structure
 *   as var_export() will generate an error.
 */
function debug($data, $label = NULL, $print_r = FALSE) {
  // Print $data contents to string.
  $string = $print_r ? print_r($data, TRUE) : var_export($data, TRUE);
  trigger_error(trim($label ? "$label: $string" : $string));
}

/**
 * Parses a dependency for comparison by drupal_check_incompatibility().
 * Backport from D7.
 *
 * @param $dependency
 *   A dependency string, for example 'foo (>=7.x-4.5-beta5, 3.x)'.
 *
 * @return
 *   An associative array with three keys:
 *   - 'name' includes the name of the thing to depend on (e.g. 'foo').
 *   - 'original_version' contains the original version string (which can be
 *     used in the UI for reporting incompatibilities).
 *   - 'versions' is a list of associative arrays, each containing the keys
 *     'op' and 'version'. 'op' can be one of: '=', '==', '!=', '<>', '<',
 *     '<=', '>', or '>='. 'version' is one piece like '4.5-beta3'.
 *   Callers should pass this structure to drupal_check_incompatibility().
 *
 * @see drupal_check_incompatibility()
 */
function drupal_parse_dependency($dependency) {
  // We use named subpatterns and support every op that version_compare
  // supports. Also, op is optional and defaults to equals.
  $p_op = '(?P<operation>!=|==|=|<|<=|>|>=|<>)?';
  // Core version is always optional: 7.x-2.x and 2.x is treated the same.
  $p_core = '(?:' . preg_quote(DRUPAL_CORE_COMPATIBILITY) . '-)?';
  $p_major = '(?P<major>\d+)';
  // By setting the minor version to x, branches can be matched.
  $p_minor = '(?P<minor>(?:\d+|x)(?:-[A-Za-z]+\d+)?)';
  $value = array();
  $parts = explode('(', $dependency, 2);
  $value['name'] = trim($parts[0]);
  if (isset($parts[1])) {
    $value['original_version'] = ' (' . $parts[1];
    foreach (explode(',', $parts[1]) as $version) {
      if (preg_match("/^\s*$p_op\s*$p_core$p_major\.$p_minor/", $version, $matches)) {
        $op = !empty($matches['operation']) ? $matches['operation'] : '=';
        if ($matches['minor'] == 'x') {
          // Drupal considers "2.x" to mean any version that begins with
          // "2" (e.g. 2.0, 2.9 are all "2.x"). PHP's version_compare(),
          // on the other hand, treats "x" as a string; so to
          // version_compare(), "2.x" is considered less than 2.0. This
          // means that >=2.x and <2.x are handled by version_compare()
          // as we need, but > and <= are not.
          if ($op == '>' || $op == '<=') {
            $matches['major']++;
          }
          // Equivalence can be checked by adding two restrictions.
          if ($op == '=' || $op == '==') {
            $value['versions'][] = array('op' => '<', 'version' => ($matches['major'] + 1) . '.x');
            $op = '>=';
          }
        }
        $value['versions'][] = array('op' => $op, 'version' => $matches['major'] . '.' . $matches['minor']);
      }
    }
  }
  return $value;
}

/**
 * Checks whether a version is compatible with a given dependency.
 * Backported from D7.
 *
 * @param $v
 *   The parsed dependency structure from drupal_parse_dependency().
 * @param $current_version
 *   The version to check against (like 4.2).
 *
 * @return
 *   NULL if compatible, otherwise the original dependency version string that
 *   caused the incompatibility.
 *
 * @see drupal_parse_dependency()
 */
function drupal_check_incompatibility($v, $current_version) {
  if (!empty($v['versions'])) {
    foreach ($v['versions'] as $required_version) {
      if ((isset($required_version['op']) && !version_compare($current_version, $required_version['version'], $required_version['op']))) {
        return $v['original_version'];
      }
    }
  }
}