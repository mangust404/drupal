<?php
// $Id: run-tests.sh,v 1.1.2.7 2009/10/19 01:49:41 boombatower Exp $
// Core: Id: run-tests.sh,v 1.35 2009/08/17 19:14:41 webchick Exp

/**
 * @file
 * Backport of Drupal 7 run-tests.sh with modifications, see BACKPORT.txt.
 * This file must be placed in the Drupal scripts folder in order for it to
 * work properly.
 *
 * Copyright 2008-2009 by Jimmy Berry ("boombatower", http://drupal.org/user/214218)
 */

define('SIMPLETEST_SCRIPT_COLOR_PASS', 32); // Green.
define('SIMPLETEST_SCRIPT_COLOR_FAIL', 31); // Red.
define('SIMPLETEST_SCRIPT_COLOR_EXCEPTION', 33); // Yellow.
define('SIMPLETEST_SCRIPT_COLOR_PENDING', 34); // Dark blue.

// Set defaults and get overrides.
list($args, $count) = simpletest_script_parse_args();

if ($args['help'] || $count == 0) {
  simpletest_script_help();
  exit;
}

if ($args['execute-batch']) {
  // Masquerade as Apache for running tests.
  simpletest_script_init("Apache");
  simpletest_script_execute_batch();
}
else {
  // Run administrative functions as CLI.
  simpletest_script_init(NULL);
}

// Bootstrap to perform initial validation or other operations.
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

ob_end_flush();

if (!module_exists('simpletest')) {
  simpletest_script_print_error("The simpletest module must be enabled before this script can run.");
  exit;
}

if ($args['clean']) {
  // Clean up left-over times and directories.
  simpletest_clean_environment();
  // Clean and rebuild up tests list cache.
  cache_clear_all('simpletest', 'cache');
  simpletest_test_get_all();

  echo "\nEnvironment cleaned.\n";

  // Get the status messages and print them.
  $messages = drupal_get_messages('status');
  foreach (array_pop($messages) as $text) {
    echo " - " . $text . "\n";
  }
  exit;
}

// Load SimpleTest files.
$groups = simpletest_test_get_all();
$all_tests = array();
foreach ($groups as $group => $tests) {
  $all_tests = array_merge($all_tests, array_keys($tests));
}
$test_list = array();

if ($args['list']) {
  // Display all available tests.
  echo "\nAvailable test groups & classes\n";
  echo   "-------------------------------\n\n";
  foreach ($groups as $group => $tests) {
    echo $group . "\n";
    foreach ($tests as $class => $info) {
      echo " - " . $info['name'] . ' (' . $class . ')' . "\n";
    }
  }
  exit;
}

$test_list = simpletest_script_get_test_list();

// Try to allocate unlimited time to run the tests.
//drupal_set_time_limit(0);
if (!ini_get('safe_mode')) {
  set_time_limit(0);
}

simpletest_script_reporter_init();

// Setup database for test results.
//$test_id = db_insert('simpletest_test_id')->useDefaults(array('test_id'))->execute();
db_query('INSERT INTO {simpletest_test_id} VALUES (default)');
$test_id = db_last_insert_id('simpletest_test_id', 'test_id');

// Index initialize
cache_set($test_id . '_index', 1, 'cache');

// Execute tests.
simpletest_script_command($args['concurrency'], $test_id, implode(",", $test_list));

// Retrieve the last database prefix used for testing and the last test class
// that was run from. Use the information to read the lgo file in case any
// fatal errors caused the test to crash.
list($last_prefix, $last_test_class) = simpletest_last_test_get($test_id);
simpletest_log_read($test_id, $last_prefix, $last_test_class);

// Display results before database is cleared.
simpletest_script_reporter_display_results();

// Cleanup our test results.
simpletest_clean_results_table($test_id);

cache_clear_all($test_id . '_index', 'cache');

/**
 * Print help text.
 */
function simpletest_script_help() {
  global $args;

  echo <<<EOF

Run Drupal tests from the shell.

Usage:        {$args['script']} [OPTIONS] <tests>
Example:      {$args['script']} Profile

All arguments are long options.

  --help      Print this page.

  --list      Display all available test groups.

  --clean     Cleans up database tables or directories from previous, failed,
              tests and then exits (no tests are run).
              Also resets tests list (useful while creating new tests).

  --url       Immediately preceeds a URL to set the host and path. You will
              need this parameter if Drupal is in a subdirectory on your
              localhost and you have not set \$base_url in settings.php.

  --php       The absolute path to the PHP executable. Usually not needed.

  --concurrency [num]

              Run tests in parallel, up to [num] tests at a time. This requires
              the Process Control Extension (PCNTL) to be compiled in PHP, not
              supported under Windows.

  --all       Run all available tests.

  --class     Run tests identified by specific class names, instead of group names.

  --name      Run tests identified by method name or by class name either.
              A part of the name and lowercase also works. You can also specify
              full test name as "MyTestCaseClass::testMyMethod"

  --file      Run tests identified by specific file names, instead of group names.
              Specify the path and the extension (i.e. 'modules/user/user.test').

  --color     Output the results with color highlighting.

  --verbose   Output detailed assertion messages in addition to summary.

  --errors    Show only failed examples when all the tests will be finished.

  <test1>[,<test2>[,<test3> ...]]

              One or more tests to be run. By default, these are interpreted
              as the names of test groups as shown at
              ?q=admin/build/testing.
              These group names typically correspond to module names like "User"
              or "Profile" or "System", but there is also a group "XML-RPC".
              If --class is specified then these are interpreted as the names of
              specific test classes whose test methods will be run. Tests must
              be separated by commas. Ignored if --all is specified.

To run this script you will normally invoke it from the root directory of your
Drupal installation as the webserver user (differs per configuration), or root:

sudo -u [wwwrun|www-data|etc] php ./scripts/{$args['script']}
  --url http://example.com/ --all
sudo -u [wwwrun|www-data|etc] php ./scripts/{$args['script']}
  --url http://example.com/ --class UploadTestCase
\n
EOF;
}

/**
 * Parse execution argument and ensure that all are valid.
 *
 * @return The list of arguments.
 */
function simpletest_script_parse_args() {
  // Set default values.
  $args = array(
    'script' => '',
    'help' => FALSE,
    'list' => FALSE,
    'clean' => FALSE,
    'url' => '',
    'php' => '',
    'concurrency' => 1,
    'all' => FALSE,
    'class' => FALSE,
    'name' => '',
    'file' => FALSE,
    'color' => FALSE,
    'verbose' => FALSE,
    'errors' => FALSE,
    'test_names' => array(),
    'display_class' => FALSE,
    // Used internally.
    'test-id' => NULL,
    'execute-batch' => FALSE
  );

  // Override with set values.
  $args['script'] = basename(array_shift($_SERVER['argv']));

  $count = 0;
  while ($arg = array_shift($_SERVER['argv'])) {
    if (preg_match('/--(\S+)/', $arg, $matches)) {
      // Argument found.
      if (array_key_exists($matches[1], $args)) {
        // Argument found in list.
        $previous_arg = $matches[1];
        if (is_bool($args[$previous_arg])) {
          $args[$matches[1]] = TRUE;
        }
        else {
          $args[$matches[1]] = array_shift($_SERVER['argv']);
        }
        // Clear extraneous values.
        $args['test_names'] = array();
        $count++;
      }
      else {
        // Argument not found in list.
        simpletest_script_print_error("Unknown argument '$arg'.");
        exit;
      }
    }
    else {
      // Values found without an argument should be test names.
      $args['test_names'] += explode(',', $arg);
      $count++;
    }
  }

  // Validate the concurrency argument
  if (!is_numeric($args['concurrency']) || $args['concurrency'] <= 0) {
    simpletest_script_print_error("--concurrency must be a strictly positive integer.");
    exit;
  }
  elseif ($args['concurrency'] > 1 && !function_exists('pcntl_fork')) {
    simpletest_script_print_error("Parallel test execution requires the Process Control extension to be compiled in PHP. Please see http://php.net/manual/en/intro.pcntl.php for more information.");
    exit;
  }

  return array($args, $count);
}

/**
 * Initialize script variables and perform general setup requirements.
 */
function simpletest_script_init($server_software) {
  global $args, $php;

  $host = 'localhost';
  $path = '';
  // Determine location of php command automatically, unless a command line argument is supplied.
  if (!empty($args['php'])) {
    $php = $args['php'];
  }
  elseif ($php_env = getenv('_')) {
    // '_' is an environment variable set by the shell. It contains the command that was executed.
    $php = $php_env;
  }
  elseif ($sudo = getenv('SUDO_COMMAND')) {
    // 'SUDO_COMMAND' is an environment variable set by the sudo program.
    // Extract only the PHP interpreter, not the rest of the command.
    list($php, ) = explode(' ', $sudo, 2);
  }
  else {
    simpletest_script_print_error('Unable to automatically determine the path to the PHP interpreter. Supply the --php command line argument.');
    simpletest_script_help();
    exit();
  }

  // Get url from arguments.
  if (!empty($args['url'])) {
    $parsed_url = parse_url($args['url']);
    $host = $parsed_url['host'] . (isset($parsed_url['port']) ? ':' . $parsed_url['port'] : '');
    $path = isset($parsed_url['path'])? $parsed_url['path']: '';
  }

  $_SERVER['HTTP_HOST'] = $host;
  $_SERVER['REMOTE_ADDR'] = '127.0.0.1';
  $_SERVER['SERVER_ADDR'] = '127.0.0.1';
  $_SERVER['SERVER_SOFTWARE'] = $server_software;
  $_SERVER['SERVER_NAME'] = 'localhost';
  $_SERVER['REQUEST_URI'] = $path .'/';
  $_SERVER['REQUEST_METHOD'] = 'GET';
  $_SERVER['SCRIPT_NAME'] = $path .'/index.php';
  $_SERVER['PHP_SELF'] = $path .'/index.php';
  $_SERVER['HTTP_USER_AGENT'] = 'Drupal command line';

  chdir(realpath(dirname(__FILE__) . '/..'));
  define('DRUPAL_ROOT', getcwd());
  require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
}

/**
 * Execute a batch of tests.
 */
function simpletest_script_execute_batch() {
  global $args;

  if (is_null($args['test-id'])) {
    simpletest_script_print_error("--execute-batch should not be called interactively.");
    exit;
  }
  if ($args['concurrency'] == 1) {
    // Fallback to mono-threaded execution.
    if (count($args['test_names']) > 1) {
      foreach ($args['test_names'] as $test_class) {
        // Execute each test in its separate Drupal environment.
        simpletest_script_command(1, $args['test-id'], $test_class);
      }
      exit;
    }
    else {
      // Execute an individual test.
      $test_class = array_shift($args['test_names']);
      drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
      simpletest_script_run_one_test($args['test-id'], $test_class);
      exit;
    }
  }
  else {
    // Multi-threaded execution.
    $children = array();
    while (!empty($args['test_names']) || !empty($children)) {
      // Fork children safely since Drupal is not bootstrapped yet.
      while (count($children) < $args['concurrency']) {
        if (empty($args['test_names'])) break;

        $child = array();
        $child['test_class'] = $test_class = array_shift($args['test_names']);
        $child['pid'] = pcntl_fork();
        if (!$child['pid']) {
          // This is the child process, bootstrap and execute the test.
          drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
          simpletest_script_run_one_test($args['test-id'], $test_class);
          exit;
        }
        else {
          // Register our new child.
          $children[] = $child;
        }
      }

      // Wait for children every 200ms.
      usleep(200000);

      // Check if some children finished.
      foreach ($children as $cid => $child) {
        if (pcntl_waitpid($child['pid'], $status, WUNTRACED | WNOHANG)) {
          // This particular child exited.
          unset($children[$cid]);
        }
      }
    }
    exit;
  }
}

/**
 * Run a single test (assume a Drupal bootstrapped environment).
 */
function simpletest_script_run_one_test($test_id, $test_class) {
  global $args;
  error_reporting(E_ALL & ~E_DEPRECATED);
  global $current_test_class, $current_test_method, $test_run_success;
  $current_test_class = $test_class;

  function simpletest_shutdown() {
    global $test_run_success, $current_test_class, $current_test_method;
    if (!$test_run_success) {
      print simpletest_script_print('FATAL: Test execution script exited unexpectedly: ' . (empty($current_test_class)? 'unknown_class': $current_test_class) . (empty($current_test_method)? '::unknown method': '::' . $current_test_method) . "\n", simpletest_script_color_code('fail'));
    }
  }

  register_shutdown_function('simpletest_shutdown');

  // Drupal 6.
  require_once drupal_get_path('module', 'simpletest') . '/drupal_web_test_case.php';
  drupal_load('module', 'simpletest');
  $classes = simpletest_test_get_all_classes();

  if (strpos($test_class, '::') !== FALSE) {
    // Run single method
    list($class, $method) = explode('::', $test_class);
    require_once $classes[$class]['file'];

    $current_test_class = $class;
    $test = new $class($test_id);
    $test->run($method);
  }
  else {
    require_once $classes[$test_class]['file'];

    $test = new $test_class($test_id);
    $test->run();
  }
  
  $test_run_success = TRUE;

  $info = $test->getInfo();

  $status = ((isset($test->results['#fail']) && $test->results['#fail'] > 0)
           || (isset($test->results['#exception']) && $test->results['#exception'] > 0) ? 'fail' : 'pass');

  $data = cache_get($test_id . '_index', 'cache');
  if (!empty($data->data)) {
    $index = $data->data;
    cache_set($test_id . '_index', $index + 1, 'cache');
  }
  simpletest_script_print((empty($index)? '': $index . '. ') . $info['name'] . ' ' . _simpletest_format_summary_line($test->results) . ($status == 'fail' || !empty($args['display_class'])? ' (' . $test_class . ')': '') . "\n", simpletest_script_color_code($status));
}

/**
 * Execute a command to run batch of tests in separate process.
 */
function simpletest_script_command($concurrency, $test_id, $tests) {
  global $args, $php;

  $command = "$php ./scripts/{$args['script']} --url {$args['url']}";
  if ($args['color']) {
    $command .= ' --color';
  }
  if (!empty($args['name'])) {
    $command .= ' --display_class';
  }

  $command .= " --php " . escapeshellarg($php) . " --concurrency $concurrency --test-id $test_id --execute-batch $tests";
  passthru($command);
}

/**
 * Get list of tests based on arguments. If --all specified then
 * returns all available tests, otherwise reads list of tests.
 *
 * Will print error and exit if no valid tests were found.
 *
 * @return List of tests.
 */
function simpletest_script_get_test_list() {
  global $args, $all_tests, $groups;

  $test_list = array();
  if ($args['all']) {
    $test_list = $all_tests;
  }
  else {
    if ($args['class']) {
      // Check for valid class names.
      foreach ($args['test_names'] as $class_name) {
        if (in_array($class_name, $all_tests)) {
          $test_list[] = $class_name;
        }
      }
    }
    elseif ($args['file']) {
      require_once drupal_get_path('module', 'simpletest') . '/drupal_web_test_case.php';
      $files = array();
      foreach ($args['test_names'] as $file) {
//        $files[drupal_realpath($file)] = 1;
        $files[realpath($file)] = 1;
        require_once realpath($file);
      }

      // Check for valid class names.
      foreach ($all_tests as $class_name) {
        if (class_exists($class_name, FALSE)) {
          $refclass = new ReflectionClass($class_name);
          if (isset($files[$refclass->getFileName()])) {
            $test_list[] = $class_name;
          }
        }
      }
    }
    elseif (!empty($args['name'])) {
      require_once drupal_get_path('module', 'simpletest') . '/drupal_web_test_case.php';
      if (strpos($args['name'], '::') !== FALSE) {
        list($class, $method) = explode('::', $args['name']);
        $all = simpletest_test_get_all_classes();
        if (isset($all[$class])) {
          require_once($all[$class]['file']);
          if (method_exists(new $class, $method)) {
            $test_list[] = $class . '::' . $method;
          }
        }
      }
      foreach (simpletest_test_get_all_classes() as $class => $info) {
        require_once($info['file']);
        if (method_exists($class, 'getInfo')) {
          $has_methods = FALSE;
          foreach (get_class_methods($class) as $method) {
            if (stripos($method, 'test') === 0 && stripos($method, $args['name']) !== FALSE) {
              $has_methods = TRUE;
              $test_list[] = $class . '::' . $method;
            }
          }
          if (!$has_methods && stripos($class, $args['name']) !== FALSE) {
            // Add entire class if no methods were added.
            $test_list[] = $class;
          }
        }
      }
    }
    else {
      // Check for valid group names and get all valid classes in group.
      foreach ($args['test_names'] as $group_name) {
        if (isset($groups[$group_name])) {
          foreach ($groups[$group_name] as $class_name => $info) {
            $test_list[] = $class_name;
          }
        }
      }
    }
  }

  if (empty($test_list)) {
    simpletest_script_print_error('No valid tests were specified.');
    exit;
  }
  return $test_list;
}

/**
 * Initialize the reporter.
 */
function simpletest_script_reporter_init() {
  global $args, $all_tests, $test_list, $db_type;

  echo "\n";
  echo "Drupal test run\n";
  echo "---------------\n";
  echo "\n";

  // Tell the user about what tests are to be run.
  if ($args['all']) {
    echo "All tests will run. Total: " . count($test_list) . ".\n";
  }
  else {
    echo "Tests to be run:\n";
    foreach ($test_list as $class_name) {
      $info = call_user_func(array($class_name, 'getInfo'));
      echo " - " . $info['name'] . ' (' . $class_name . ')' . "\n";
    }
  }
  echo "\n";
  echo "Database type: " . $db_type . "\n";
  echo "URL: " . $args['url'] . "\n";
  echo "\n";

  echo "Test run started: " . format_date($_SERVER['REQUEST_TIME'], 'long') . "\n";
  timer_start('run-tests');
  echo "\n";

  echo "Test summary:\n";
  echo "-------------\n";
  echo "\n";
}

/**
 * Display test results.
 */
function simpletest_script_reporter_display_results() {
  global $args, $test_id, $results_map;

  echo "\n";
  $end = timer_stop('run-tests');
  echo "Complete: " . $args['url'] . "\n";
  echo "Test run duration: " . format_interval($end['time'] / 1000);
  echo "\n\n\n";

  $results_map = array(
    'pass' => 'Pass',
    'fail' => 'Fail',
    'exception' => 'Exception',
    'pending' => 'Pending'
  );

  if ($args['verbose']) {
    // Report results.
    echo "Detailed test results:\n";
    echo "----------------------\n";
    echo "\n";

//    $results = db_query("SELECT * FROM {simpletest} WHERE test_id = :test_id ORDER BY test_class, message_id", array(':test_id' => $test_id));
    $results = db_query("SELECT * FROM {simpletest} WHERE test_id = %d ORDER BY test_class, message_id", $test_id);

    $test_class = '';
//    foreach ($results as $result) {
    while ($result = db_fetch_object($results)) {
      if (isset($results_map[$result->status])) {
        if ($result->test_class != $test_class) {
          // Display test class every time results are for new test class.
          echo "\n\n---- $result->test_class ----\n\n\n";
          $test_class = $result->test_class;
        }

        simpletest_script_format_result($result);
      }
    }
  }
  else if ($args['errors']) {
    $results = db_query("SELECT * FROM {simpletest} WHERE test_id = %d AND status != 'pass' ORDER BY test_class, message_id", $test_id);
    while ($result = db_fetch_object($results)) {
      if (isset($results_map[$result->status])) {
        if ($result->test_class != $test_class) {
          // Display test class every time results are for new test class.
          echo "\n\n---- $result->test_class ----\n\n\n";
          $test_class = $result->test_class;
        }

        simpletest_script_format_result($result);
      }
    }
  }
}

function simpletest_prettify_message($input) {
  return html_entity_decode(preg_replace_callback("/(&#[0-9]+;)/", function($m) { return mb_convert_encoding($m[1], "UTF-8", "HTML-ENTITIES"); }, $input)); 
}
/**
 * Format the result so that it fits within the default 80 character
 * terminal size.
 *
 * @param $result The result object to format.
 */
function simpletest_script_format_result($result) {
  global $results_map, $color;

  $summary = sprintf("%-10.10s %-10.10s %-30.30s %-5.5s %-20.20s\n",
    $results_map[$result->status], $result->message_group, basename($result->file), $result->line, isset($result->caller)? $result->caller: '(unknown)');

  simpletest_script_print($summary, simpletest_script_color_code($result->status));

  $lines = explode("\n", $result->message);
  $first_line = wordwrap(trim(strip_tags(array_shift($lines))), 76);
  echo "    $first_line\n";

  $lines = explode("\n", wordwrap(trim(strip_tags(implode("\n", $lines))), 76));
  foreach ($lines as $i => $line) {
    simpletest_script_print("    " . simpletest_prettify_message($line) . "\n", simpletest_script_color_code($result->status));
  }
  print "\n";
}

/**
 * Print error message prefixed with "  ERROR: " and displayed in fail color
 * if color output is enabled.
 *
 * @param $message The message to print.
 */
function simpletest_script_print_error($message) {
  simpletest_script_print("  ERROR: $message\n", SIMPLETEST_SCRIPT_COLOR_FAIL);
}

/**
 * Print a message to the console, if color is enabled then the specified
 * color code will be used.
 *
 * @param $message The message to print.
 * @param $color_code The color code to use for coloring.
 */
function simpletest_script_print($message, $color_code) {
  global $args;
  if ($args['color']) {
    echo "\033[" . $color_code . "m" . $message . "\033[0m";
  }
  else {
    echo $message;
  }
}

/**
 * Get the color code associated with the specified status.
 *
 * @param $status The status string to get code for.
 * @return Color code.
 */
function simpletest_script_color_code($status) {
  switch ($status) {
    case 'pass':
      return SIMPLETEST_SCRIPT_COLOR_PASS;
    case 'fail':
      return SIMPLETEST_SCRIPT_COLOR_FAIL;
    case 'exception':
      return SIMPLETEST_SCRIPT_COLOR_EXCEPTION;
    case 'pending':
      return SIMPLETEST_SCRIPT_COLOR_PENDING;
  }
  return 0; // Default formatting.
}
