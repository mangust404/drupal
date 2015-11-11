#!/usr/bin/php
<?php

# Set your development envirenment hostname here.
$testing_domain = 'drupal6.dev';

define('DRUPAL_ROOT', realpath(dirname(__FILE__) . '/../'));

define('SIMPLETEST_SCRIPT_COLOR_PASS', 32); // Green.
define('SIMPLETEST_SCRIPT_COLOR_FAIL', 31); // Red.
define('SIMPLETEST_SCRIPT_COLOR_EXCEPTION', 33); // Yellow.
define('SIMPLETEST_SCRIPT_COLOR_PENDING', 34); // Dark blue.
/**
 * Natural join, took from http://stackoverflow.com/a/2516779/3675705
 */
function array_cartesian() {
    $_ = func_get_args();
    if(count($_) == 0)
        return array(array());
    $a = array_shift($_);
    $c = call_user_func_array(__FUNCTION__, $_);
    $r = array();
    foreach($a as $v)
        foreach($c as $p)
            $r[] = array_merge(array($v), $p);
    return $r;
}

// Processors count is determine which concurrency level may be used.
$concurrency = intval(shell_exec('cat /proc/cpuinfo | grep processor | wc -l'));
if ($concurrency == 0) $concurrency = 1;

$domains = array();

$domains_variations = array(
  'database' => array('pgsql', 'mysql', 'mysqli'),		// database part
  'caching' => array('memcache', ''),				// caching part
  'subdir' => array('subdir', ''),				// subdirectory part
);

// Prepare filter parameters and additional parameters for run-test.sh passed by command line arguments.
$filter_params = array();
$configure_params = array('--color', '--all', '--concurrency ' . $concurrency);
for ($i = 1; $i < $argc; $i++) {
  $is_filter_param = FALSE;
  
  foreach ($domains_variations as $variation) {
    if (in_array($argv[$i], $variation)) {
      $is_filter_param = TRUE;
    }
  }
  if ($is_filter_param) {
    $filter_params[] = $argv[$i];
  }
  else if (!in_array($argv[$i], $configure_params)) {
    $configure_params[] = $argv[$i];
    if (strpos($argv[$i], '--name') !== FALSE || strpos($argv[$i], '--class') !== FALSE) {
      // If one of --name or --class parameters specified then we should remove '--all'.
      $all_index = array_search('--all', $configure_params);
      if ($all_index !== FALSE) {
        unset($configure_params[$all_index]);
      }
      $configure_params[] = $argv[$i + 1];
      $i++;
    }
  }
}

foreach (call_user_func_array('array_cartesian', $domains_variations) as $variation) {
  // Remove empty domain parts.
  $variation = array_filter($variation);
  if (count($filter_params) > 0 && count(array_intersect($filter_params, $variation)) < count($filter_params)) {
    // Skip domains which are not match the requested in command line.
    continue;
  }
  $domains[] = implode('.', $variation) . '.' . $testing_domain;
}

# Collect output data summary. It will be printed at the end of all tests.
$summary_total = array();

foreach ($domains as $domain) {
  echo "Running tests for domain $domain, concurrency: $concurrency";
  $path = '/';
  if (strpos($domain, 'subdir') !== FALSE) {
   $path = '/drupal/';
  }
  # cleanup files verbose output
  system('rm -rf ' . DRUPAL_ROOT . '/default/files/simpletest/*');
  # cleanup database
  system("php " . DRUPAL_ROOT . "/scripts/run-tests.sh --url http://$domain$path --clean");

  # Array of lines printed by test run script.
  $test_output = array();

  chdir(DRUPAL_ROOT);
  $cmd = "php " . "scripts/run-tests.sh --url http://$domain$path " . implode(' ', $configure_params);
  print "\n" . $cmd . "\n";
  # Run tests and get file pointer to STDOUT of the executed process.
  $pipes = array();
  $descriptorspec = array(
    0 => array("pipe", "r"),
    1 => array("pipe", "w"),
    2 => array("pipe", "w"),
  );
  $pp = proc_open($cmd, $descriptorspec, $pipes, DRUPAL_ROOT);
  
  $summary_total[$domain] = array(
    'pass' => 0,
    'fail' => 0,
    'exception' => 0,
    'error_classes' => array(),
  );

  # Get process STDOUT, print it to the screen and analyze output simultaneously.
  while (!feof($pipes[1])) {
    while($data = fread($pipes[1], 4096)) {
      print $data;

      $lines = explode("\n", $data);
      foreach ($lines as $line) {
        if (preg_match('/([0-9]+) pass[e]?[s]?, ([0-9]+) fail[s]?, (and )?([0-9]+) exception[s]?[^\(]*(\([^\)]+\))?/', $line, $matches)) {
          $summary_total[$domain]['pass'] += intval($matches[1]);
          $summary_total[$domain]['fail'] += intval($matches[2]);
          $summary_total[$domain]['exception'] += intval($matches[4]);

          if (!empty($matches[6])) {
            $summary_total[$domain]['error_classes'][] = $matches[6];
          }
        }
      }
    }
  }
  proc_close($pp);
}

print "\n\n-------------\n\nOverall summary:\n\n";

foreach ($summary_total as $domain => $summary) {
  $color = SIMPLETEST_SCRIPT_COLOR_PASS;
  if ($summary['fail'] > 0) {
    $color = SIMPLETEST_SCRIPT_COLOR_FAIL;
  }
  else if($summary['exception'] > 0) {
    $color = SIMPLETEST_SCRIPT_COLOR_EXCEPTION;
  }

  $args = array(
    '@pass' => $summary['pass'] == 1 ? '1 pass' : $summary['pass'] . ' passes',
    '@fail' => $summary['fail'] == 1 ? '1 fail' : $summary['fail'] . ' fails',
    '@exception' => $summary['exception'] == 1 ? '1 exception' : $summary['exception'] . ' exceptions',
  );

  simpletest_script_print($domain . ': ' . strtr('@pass, @fail, and @exception', $args), $color);
  if (count($summary['error_classes']) > 0) {
    print "\nError classes: " . implode(',', $summary['error_classes']) . "\n\n";
  }
  print "\n";
}


/**
 * Print a message to the console, if color is enabled then the specified
 * color code will be used.
 *
 * @param $message The message to print.
 * @param $color_code The color code to use for coloring.
 */
function simpletest_script_print($message, $color_code) {
  echo "\033[" . $color_code . "m" . $message . "\033[0m";
}
