<?php

/**
 * @file
 * Common cache library.
 * All the common cache functions doesn't rely on cache engine implementation and works
 * above it with only basic cache API: cache_get(), cache_set() and cache_clear_all().
 *
 * "Func Cache" engine intended to provide a transparent memoization of system and user
 * functions which should make database calls or a costly calculations.
 *
 * TODO: DRY bootstrap and main functions.
 *
 */

/**
 * How many cache entries may be builded by single fork/signle batch operation.
 */
define('FUNC_PROCESS_CACHE_PORTION_DEFAULT', 5000);
define('FUNC_CACHE_EMPTY', 'FUNC_CACHE_EMPTY');

/**
 * Helper function.
 * Print error message to the screen in CLI mode or write it to the watchdog.
 *
 * @param $msg
 *   A string message.
 */
function func_cache_notify($msg) {
  if (drupal_is_cli()) {
    print "\n$msg\n\n";
  }
  else if (drupal_bootstrap() >= DRUPAL_BOOTSTRAP_FULL) {
    watchdog('cache', $msg, array(), WATCHDOG_ERROR);
  }
}

/**
 * Helper method. Make cid (cache ID) for cached function by arguments.
 *
 * @param $func
 *   Cached function name.
 * @param $args
 *   Array of function's arguments.
 *
 * @return
 *   The string of cache ID.
 */
function func_cache_get_cid($func, $args = NULL, $func_info = NULL) {
  $cid = $func;
  if (!empty($args) && !empty($func_info['args'])) {
    if (is_array($args)) {
      foreach($args as $arg) {
        if (!is_array($arg)) {
          $arg = array($arg);
        }
        $cid .= '_' . implode('-', $arg);
      }
    }
    else if (!is_object($args)) {
      $cid .= '_' . $args;
    }
  }
  return $cid;
}

/**
 * Get cache setters registry.
 *
 * @return
 *   Array of cache setters.
 */
function func_get_setters() {
  $setters = &drupal_static(__FUNCTION__);

  if (empty($setters)) {
    // Initialize with bootstrap cache setters.
    $setters = drupal_static('func_get_setters_bootstrap');

    // Bootstrap is full, we may safely call module_implements() now.
    foreach (module_implements('cache_setters') as $module) {
      foreach (module_invoke($module, 'cache_setters') as $func => $func_info) {
        if (!isset($setters[$func])) {
          $func_info['module'] = $module;
          $setters[$func] = $func_info;
        }
      }
    }
  }
  return $setters;
}

/**
 * Special version of func_get_setters() that can be safely called
 * before drupal fully bootstraped.
 *
 * @return
 *   Array of system core modules cache setters.
 * @see func_get_setters()
 */
function func_get_setters_bootstrap() {
  $setters = &drupal_static(__FUNCTION__);

  if (empty($setters)) {
    include_once(DRUPAL_ROOT . '/includes/module.inc');
    // While bootstrap is less than DRUPAL_BOOTSTRAP_FULL we unable to call
    // module_implements() because it will cause infinite loop.
    // For a bootstrap process we should gather cache setters only from required
    // core modules.
    foreach (array('system', 'user', 'filter', 'block', 'node') as $module) {
      // Also unable to call drupal_load here.
      include_once(DRUPAL_ROOT . '/modules/' . $module . '/' . $module . '.module');
      $func_name = $module . '_cache_setters';
      if (function_exists($func_name)) {
        foreach ($func_name() as $func => $func_info) {
          $func_info['module'] = $module;
          $setters[$func] = $func_info;
        }
      }
    }
  }

  return $setters;
}

/**
 * Get cache tokens.
 *
 * @return
 *   Array of cache tokens.
 */
function func_get_tokens($token, $parent_token = NULL) {
  $tokens = &drupal_static(__FUNCTION__);

  $cid = $token;
  if (!empty($parent_token)) {
    $cid .= ':' . $parent_token;
  }

  if (isset($tokens[$cid])) {
    return $tokens[$cid];
  }

  $tokens[$cid] = array();

  if (drupal_bootstrap() >= DRUPAL_BOOTSTRAP_FULL) {
    $tokens[$cid] = module_invoke_all('cache_tokens', 'view', $token, $parent_token);
  }
  else {
    // Unable to call module_invoke_all() here.
    foreach (array('system', 'user', 'filter', 'block', 'node') as $module) {
      $func_name = $module . '_cache_tokens';

      if (function_exists($func_name)) {
        $result = $func_name('view', $token, $parent_token);
        if (is_array($result)) {
          $tokens[$cid] = array_merge($tokens, $result);
        }
      }
    }
  }
  return $tokens[$cid];
}

/**
 * Include required files for cached function.
 *
 * @param $func_info array
 *   Memoized function definition from hook_cache_setters().
 *
 * @see hook_cache_setters()
 */
function func_file_include(&$func_info) {
  if (!empty($func_info['#included'])) {
    return;
  }
  if (!empty($func_info['file'])) {
    if (file_exists($func_info['file'])) {
      // Root-relative or absolute path.
      include_once($func_info['file']);
    }
    else if (file_exists($file = drupal_get_path('module', $func_info['module']) . '/' . $func_info['file'])) {
      // Module-relative path.
      include_once($file);
    }
    
  }
  $func_info['#included'] = TRUE;
}

/**
 * Include required files for cached function while not fully bootstrapped.
 *
 * @param $func_info array
 *   Memoized function definition from hook_cache_setters().
 *
 * @see hook_cache_setters()
 */
function func_file_include_bootstrap(&$func_info) {
  if (!empty($func_info['#included'])) {
    return;
  }
  if (!empty($func_info['file'])) {
    if (file_exists($func_info['file'])) {
      include_once($func_info['file']);
    }
    else if (file_exists($file = DRUPAL_ROOT . '/modules/' . $func_info['module'] . '/' . $func_info['file'])) {
      include_once($file);
    }
  }
  $func_info['#included'] = TRUE;
}

/**
 * Get memoized function contents. Builds content if it's not cached yet and puts
 * it to the cache ("memoize" it).
 *
 * First argument should be always a cached function name. The rest arguments will be
 * used to make cid or will be passed to the function for rebuilding if cached data
 * is not present.
 *
 * @param ...
 *   Array of parameters.
 *
 * @return
 *   The function contents. Depends on cached function result.
 *
 * @see hook_cache_setters()
 */
function func_get_cache() {
  $static = &drupal_static(__FUNCTION__, array());

  $args = func_get_args();
  $func = array_shift($args);

  $cache_setters = func_get_setters();

  if (!isset($cache_setters[$func])) {
    func_cache_notify('ERROR: Function cache-setter is not found in setters registry: ' . $func);
    if (function_exists($func)) {
      // Fallback if cache setter function exists but not defined by hooks:
      // just call it and return result. The warning message was printed or logged.
      return call_user_func_array($func, $args);
    }
    return;
  }

  // Prepare cached function hook info.
  $func_info = &$cache_setters[$func];

  $cid = func_cache_get_cid($func, $args, $func_info);

  if (isset($static[$cid])) {
    // Return function contents from static cache for a better speed.
    $result = $static[$cid];
  }
  else {
    // Trying to fetch from the cache.
    $data = cache_get($cid, $func_info['bin']);
    if (empty($data)) {
      func_file_include($func_info);
      // No data in cache. Rebuild it.
      if (!function_exists($func)) {
        func_cache_notify('ERROR: Function cache-setter doesn\'t exist: ' . $func);
        return FALSE;
      }
      $result = call_user_func_array($func, $args);
      if ($result != FUNC_CACHE_EMPTY) {
        // We don't check the result. It also may be null or false but it should be cached.
        cache_set($cid, $result, $func_info['bin']);
      }
    }
    else {
      // Cache hit.
      $result = $data->data;
    }
  }

  // Invoke cache setter callback if provided.
  if (isset($func_info['callback'])) {
    func_file_include($func_info);
    if (function_exists($func_info['callback'])) {
      // Invoking callbacks only if exists.
      $args[] = &$result;
      call_user_func_array($func_info['callback'], $args);
    }
  }

  // Store the result in the static cache.
  if ($result != FUNC_CACHE_EMPTY) {
    $static[$cid] = $result;
    return $result;
  }
}

/**
 * Special version of func_get_cache() that can be safely called
 * before drupal fully bootstraped without unfinite loop calls.
 *
 * @param ...
 *   Array of parameters.
 *
 * @return
 *   The function contents. Depends on cached function result.
 * @see func_get_cache()
 */
function func_get_cache_bootstrap() {
  $static = &drupal_static(__FUNCTION__, array());

  $args = func_get_args();
  $func = array_shift($args);

  $cache_setters = func_get_setters_bootstrap();

  if (!isset($cache_setters[$func])) {
    if (function_exists($func)) {
      // Fallback if cache setter function exists but not defined by hooks:
      // just call it and return result. The warning message was printed or logged.
      return call_user_func_array($func, $args);
    }
    return;
  }

  // Prepare cached function hook info.
  $func_info = &$cache_setters[$func];

  $cid = func_cache_get_cid($func, $args, $func_info);

  if (isset($static[$cid])) {
    // Return function contents from static cache for a better speed.
    $result = $static[$cid];
  }
  else {
    // Trying to fetch from the cache.
    $data = cache_get($cid, $func_info['bin']);
    if (empty($data)) {
      // No data in cache. Rebuild it.
      func_file_include_bootstrap($func_info);
      if (!function_exists($func)) {
        return FALSE;
      }
      $result = call_user_func_array($func, $args);
      if ($result != FUNC_CACHE_EMPTY) {
        // We don't check the result. It also may be null or false but it should be cached.
        cache_set($cid, $result, $func_info['bin']);
      }
    }
    else {
      // Cache hit.
      $result = $data->data;
    }
  }

  if ($result != FUNC_CACHE_EMPTY) {
    // Store the result in the static cache.
    $static[$cid] = $result;
    return $result;
  }
}

/**
 * Get all possible arguments for cached function. It's a heart of cache warming engine.
 *
 * Cached functions arguments sets are defined by hook_cache_setters() and may conain a 
 * plain string or integer values, tokens and simple arrays of that values. Complex nested
 * arrays and objects are not allowed. Use object ID's as a tokens instead.
 * "Token" is a string which starts with "!" sign, for example "!nid" is a token.
 * All possible token values are being generated by hook_cache_tokens(). Each module may
 * provide ther own set of tokens. Modules may use tokens provided by other modules.
 *
 * @param $func_args
 *   Array of function parameters defined by hook_cache_setters().
 *
 * @return
 *   Array of arrays with all possible function arguments.
 *
 * @see hook_cache_tokens()
 * @see hook_cache_setters()
 */
function func_cache_get_arguments($func_args, $parent_token = NULL) {
  $tokens_info = &drupal_static('tokens_info', NULL);
  $tokens_info_complete = &drupal_static('tokens_info_complete', FALSE);

  if (!is_array($func_args)) {
    // No need in complex calculations if function arguments is a plain value.
    return array($func_args);
  }
  if (count($func_args) == 0) {
    return array(array());
  }

  // Extract tokens from provided arguments.
  $tokens = array();
  foreach($func_args as $args) {
    if (is_string($args) && $args{0} == '!' && !in_array($args, $tokens)) {
      // Single argument is a token.
      $tokens[] = $args;
    }
    else if (is_array($args)) {
      foreach($args as $arg) {
        if (is_string($arg) && $arg{0} == '!' && !in_array($arg, $tokens)) {
          // Single argument from the arguments list is a token.
          $tokens[] = $arg;
        }
      }
    }
  }

  // Fill tokens info.
  if (drupal_bootstrap() >= DRUPAL_BOOTSTRAP_FULL) {
    // When bootstrap is complete the entire tokens registry should be rebuilded.
    if (!$tokens_info_complete) {
      $tokens_info = module_invoke_all('cache_tokens', 'list');
      $tokens_info_complete = TRUE;
    }
  }
  elseif (empty($tokens_info)) {
    $tokens_info_complete = FALSE;
    $tokens_info = array();
    // Unable to call module_invoke_all() here. Invoking hook_cache_tokens only
    // for system core modules.
    foreach (array('system', 'user', 'filter', 'block', 'node') as $module) {
      $func_name = $module . '_cache_tokens';

      if (function_exists($func_name)) {
        $result = $func_name('list');
        if (is_array($result)) {
          $tokens_info = array_merge($tokens_info, $result);
        }
      }
    }
  }

  $first_arg = array_shift($func_args);

  if (is_string($first_arg) && $first_arg{0} == '!') {
    // It's a token. Get all possible token values.
    // Parent token could be specified by previous call of the function.
    $items = func_get_tokens($first_arg, $parent_token);

    if (empty($items)) {
      // At least one NULL element should exists.
      $items = array(NULL);
    }
  }
  else if (is_object($first_arg)) {
    // That shouldn't happen.
    // Unable to handle object values due they can't be converted to string
    // when generating cid for cache entry.
    // TODO: objects may be supported but only when there will be a good way
    // to identify objects (convert to string).
    trigger_error('Object values for tokens are not supported', E_USER_ERROR);
    return array();
  }
  else if (!is_array($first_arg)) {
    // It's not an array, just single string or numeric parameter.
    $items = array($first_arg);
  }
  else {
    // It's not a token, just an array of parameters.
    $items = $first_arg;
  }

  $result = array();

  // Checking for the next argument.
  $next_arg = reset($func_args);

  // If next argument is a token and it has a 'parent' definition.
  if (!empty($next_arg) && is_string($next_arg) && $next_arg{0} == '!' && isset($tokens_info[$next_arg]) && is_array($tokens_info[$next_arg]) && !empty($tokens_info[$next_arg]['parent']) && $tokens_info[$next_arg]['parent'] == $first_arg) {
    // Work with nested tokens in a special way.
    foreach ($items as $item) {
      // Get all subelements recursively for the current item as parent token.
      $subitems = func_cache_get_arguments($func_args, $item);

      // Append all subitems to the result.
      foreach ($subitems as $subitem) {
        $result[] = array_merge(array($item), $subitem);
      }
    }
  }
  else {
    // Get all subelements recursively. No parent token.
    $subitems = func_cache_get_arguments($func_args);

    // Append all subitems to the result.
    foreach ($items as $item) {
      foreach ($subitems as $subitem) {
        $result[] = array_merge(array($item), $subitem);
      }
    }
  }
  return $result;
}

/**
 * Rebuild cached function variations by specified arguments.
 *
 * @param $func
 *   String containing cached function name.
 * @param $func_info
 *   Array of cached function implementation how it was returned by hook_cache_setters().
 * @param $args
 *   Array (of arrays) of arguments to rebuild.
 *
 * @see func_cache_get()
 * @see func_cache_set()
 */
function func_rebuild_args($func, $func_info, $args, $bootstrap = FALSE) {
  $static = &drupal_static('func_get_cache' . ($bootstrap? '_bootstrap': ''));

  foreach($args as $arg) {
    $_cid = func_cache_get_cid($func, $arg, $func_info);
    if (!is_array($arg)) {
      $arg = array($arg);
    }

    // Get function results and set cache.
    $result = call_user_func_array($func, $arg);
    if ($result != FUNC_CACHE_EMPTY) {
      cache_set($_cid, $result, $func_info['bin']);
      $static[$_cid] = $result;
    }
  }
}

/**
 * Rebuild all cached function variations.
 * Warning! May consume a lot of memory and CPU depending on a number of cached
 * function arguments.
 * This function is called during cache warmup or cache rebuild. You should not call it
 * manually. Use func_clear_cache_func() instead.
 *
 * @param $func
 *   String containing cached function name.
 * @param $func_info
 *   Array of cached function implementation how it was returned by hook_cache_setters().
 *
 * @see func_rebuild_args()
 * @see func_clear_cache_func()
 */
function func_rebuild_all_args($func, $func_info, $bootstrap = FALSE) {
  func_rebuild_args($func, $func_info, func_cache_get_arguments($func_info['args']), $bootstrap);
}

/**
 * Rebuild all cache entries.
 * Called by warmup script which is also should be called by cron to rebuild entries
 * with a limited lifetime.
 *
 * @param $bin
 *   Cache table name to rebuild.
 * @param $batch_func
 *   Cached function name to rebuild. Used by delayed cache warmup process.
 * @param $batch_offset
 *   Cached function name to rebuild. Used by delayed cache warmup process.
 *
 */
function func_process_cache($bin = NULL, $batch_func = NULL, $batch_offset = NULL) {
  $static = &drupal_static('func_get_cache');

  $cache_setters = func_get_setters();

  $batch_functions = array();

  $total_count = 0;
  $portion = variable_get('func_cache_portion', FUNC_PROCESS_CACHE_PORTION_DEFAULT);

  $memory_limit = parse_size(ini_get('memory_limit'));
  // Warmup process should have at least 5 megabytes for each call.
  $safe_memory_size = 5 * 1024 * 1024;

  foreach($cache_setters as $func => $func_info) {
    $func_info = &$cache_setters[$func];

    $func_info['func'] = $func;

    if (!empty($batch_func) && $func != $batch_func) {
      // Skip for the necessarry cache function in batch process.
      continue;
    }

    // Skip irrelevant bins if bin to rebuild is specified.
    if (!empty($bin) && $bin != $func_info['bin']) continue;

    if (drupal_is_cli() && ($memory_left = ($memory_limit - memory_get_usage())) < $safe_memory_size) {
      // Memory comes to the end, all other cache warming functions will run in background.
      $func_info['args'] = array();
      // We should remember function names for which we should start batch operations.
      $batch_functions[] = $func;

      if (drupal_is_cli()) {
        print "WARNING! Not enough memory. Memory limit is " . ini_get('memory_limit') . " and only " . format_size($memory_left, 'en') . " left. Postpone warming of the $func.\n";
      }
      continue;
    }

    func_file_include($func_info);

    if (!function_exists($func)) {
      func_cache_notify('ERROR: Function cache-setter doesn\'t exist: ' . $func);
      continue;
    }

    // Rebuild a set of args if present.
    if (!empty($func_info['args']) && count($func_info['args']) > 0) {
      // Generating of arguments may consume a lot of memory.
      // Right here we are sure that we have least $safe_memory_size bytes of free memory.
      $func_info['args'] = func_cache_get_arguments($func_info['args']);

      if (count($func_info['args']) > $portion) {
        // If function params number greater that a portion size then process it separately in batch execution.

        if ($batch_func === NULL) {
          // It's a main rebuild process. Don't rebuild big amount of cache entries on the main cache warm-up.
          $func_info['args'] = array();
          // We should remember function names for which we should start batch operations
          $batch_functions[] = $func;

          if (drupal_is_cli()) {
            print t("Postpone warming of the @func because number of arguments is greater than @num", array('@func' => $func, '@num' => $portion)) . "\n\n";
          }
        }
        else {
          // It's a batch process.
          if ($total_count < $portion) {
            // If we didn't reached the limit then get a portion of args and process it
            $func_info['args'] = array_slice($func_info['args'], $batch_offset, $portion);
          }
          else {
            // We are done in the current process, allow to invoke the next one with the same
            // function and the next offset.
            $batch_function = $func;
            return array($func, $total_count);
          }
        }
      }

      if (!empty($func_info['args'])) {
        if (drupal_is_cli()) {
          print "Regenerate: $func\n";
        }

        $count = 0;
        $overall = count($func_info['args']);

        // Evaluate each args cache entries
        foreach($func_info['args'] as $arg) {
          $count++;
          $total_count++;
          $_cid = func_cache_get_cid($func, $arg, $func_info);
          if (!is_array($arg)) {
            $arg = array($arg);
          }

          $data = cache_get($_cid, $func_info['bin']);
          if (empty($data) || (!empty($func_info['lifetime']) && $func_info['lifetime'] > 0 && $data->created < time() - $func_info['lifetime'])) {
            if (drupal_is_cli()) {
              $part = str_replace("\n", '', substr($_cid, strlen($func) + 1));
              // Max width of the terminal is 80 characters but max length of the cache keys is 255.
              // Trimming extra characters at the end.
              if (strlen($part) > 79) {
                $part = substr($part, 0, 76) . '...';
                $suffix = str_repeat(" ", 10);
              }
              else {
                $suffix = str_repeat(" ", 79 - strlen($part));
              }
              print "\rcid=$part ($count of $overall)$suffix";
            }
            $result = call_user_func_array($func, $arg);
            if ($result != FUNC_CACHE_EMPTY) {
              cache_set($_cid, $result, $func_info['bin']);
              $static[$_cid] = $result;
            }
          }
        }
        if (empty($func_info['args'])) {
          $data = cache_get($func, $func_info['bin']);
          // Rebuild only if there is no data or it's expired.
          if (empty($data) || (!empty($func_info['lifetime']) && $func_info['lifetime'] > 0 && $data->created < time() - $func_info['lifetime'])) {
            $result = call_user_func_array($func);
            if ($result != FUNC_CACHE_EMPTY) {
              cache_set($func, $result, $func_info['bin']);
              $static[$func] = $result;
            }
          }
        }
        if (drupal_is_cli()) {
          print "\ndone. Mem usage: " . format_size(memory_get_usage(), 'en') . '/' . ini_get('memory_limit') . ".\n\n";
        }

      }
      if ($batch_offset !== NULL) {
        // It's a batch process.
        if ($total_count >= variable_get('func_cache_portion', FUNC_PROCESS_CACHE_PORTION_DEFAULT)) {
          // We've just completed another batch operation and should continue from the same function
          // but with next offset.
          return $batch_offset + $total_count;
        }
      }
    }
    else {
      // Function have no params
      $data = cache_get($func, $func_info['bin']);
      // Rebuild only if there is no data or it's expired
      if (empty($data) || (!empty($func_info['lifetime']) && $func_info['lifetime'] > 0 && $data->created < time() - $func_info['lifetime'])) {
        if (drupal_is_cli()) print "regenerate: $func\n";
        $result = $func();

        if ($result != FUNC_CACHE_EMPTY) {
          cache_set($func, $result, $func_info['bin']);
          $static[$func] = $result;
        }
      }
    }
  }
  if (!empty($batch_functions)) {
    // It's a main rebuild process and we have a batch function with big amount of data,
    // tell about it to the rebuild script.
    return $batch_functions;
  }
}

/**
 * Rebuild a cached function entry/entries.
 *
 * First argument should be always a function name string.
 * The behavior is different and based on a provided arguments after function name:
 *  - single entry will be rebuilded if a complete function arguments set as defined in
 *    hook_cache_setters() is provided;
 *  - if arguments set is not complete then all entries matching provided argument set
 *    will be rebuilded;
 *  - all the cached function entries will be rebuilded if arguments is empty. Warning!
 *    May consume a lot of resources.
 *
 * @param ...
 *   Function arguments.
 *
 */
function func_clear_cache_func() {
  $static = &drupal_static('func_get_cache');

  $args = func_get_args();
  if (count($args) > 0) {
    $func = array_shift($args);
  }

  $cache_setters = func_get_setters();

  $func_info = &$cache_setters[$func];

  func_file_include($func_info);

  if (!function_exists($func)) {
    func_cache_notify('ERROR: Function cache-setter doesn\'t exist: ' . $func);
    return FALSE;
  }

  $args_example = array();

  if (!empty($func_info['args'])) {
    $args_example = current($func_info['args']);
    if (!is_array($args_example)) $args_example = array($args_example);
  }

  if (!empty($func_info['args']) && empty($args)) {
    // Rebuild all arguments variations if no arguments was provided.
    func_rebuild_all_args($func, $func_info);
  }
  else if (!empty($func_info['args']) && count($args) < count($func_info['args'])) {
    // We should get all possible function parameters variations and filter
    // only matching arguments.
    $filtered_arguments = func_cache_get_arguments($func_info['args']);
    $args = array_values($args);
    foreach($filtered_arguments as $filter_index => $filter_param) {
      $valid = TRUE;
      foreach($filter_param as $i => $arg) {
        if (isset($args[$i]) && $arg != $args[$i]) {
          $valid = FALSE;
          break;
        }
      }
      if (!$valid) unset($filtered_arguments[$filter_index]);
    }

    // Rebuild only matching arguments.
    func_rebuild_args($func, $func_info, $filtered_arguments);
  }
  else {
    // Rebuild a basic cached function without parameters or a complete arguments set.
    $_cid = func_cache_get_cid($func, $args, $func_info);
    $result = call_user_func_array($func, $args);

    if ($result != FUNC_CACHE_EMPTY) {
      cache_set($_cid, $result, $func_info['bin']);
      $static[$_cid] = $result;
    }
  }
}


/**
 * Special version of func_clear_cache_func_bootstrap() that can be
 * safely called before drupal fully bootstraped.
 *
 * @param ...
 *   Function arguments.
 * @see func_clear_cache_func()
 */
function func_clear_cache_func_bootstrap() {
  $static = &drupal_static('func_get_cache_bootstrap');

  $args = func_get_args();
  if (count($args) > 0) {
    $func = array_shift($args);
  }

  $cache_setters = func_get_setters_bootstrap();

  $func_info = &$cache_setters[$func];

  func_file_include_bootstrap($func_info);
  if (!function_exists($func)) {
    return FALSE;
  }

  $args_example = array();

  if (!empty($func_info['args'])) {
    $args_example = current($func_info['args']);
    if (!is_array($args_example)) $args_example = array($args_example);
  }

  if (!empty($func_info['args']) && empty($args)) {
    // Rebuild all arguments variations if no arguments was provided.
    func_rebuild_all_args($func, $func_info, TRUE);
  }
  else if (!empty($func_info['args']) && count($args) < count($func_info['args'])) {
    // We should get all possible function parameters variations and filter
    // only matching arguments.
    $filtered_arguments = func_cache_get_arguments($func_info['args']);
    $args = array_values($args);
    foreach($filtered_arguments as $filter_index => $filter_param) {
      $valid = TRUE;
      foreach($filter_param as $i => $arg) {
        if (isset($args[$i]) && $arg != $args[$i]) {
          $valid = FALSE;
          break;
        }
      }
      if (!$valid) unset($filtered_arguments[$filter_index]);
    }

    // Rebuild only matching arguments.
    func_rebuild_args($func, $func_info, $filtered_arguments, TRUE);
  }
  else {
    // Rebuild a basic cached function without parameters or a complete arguments set.
    $_cid = func_cache_get_cid($func, $args, $func_info);
    $result = call_user_func_array($func, $args);

    if ($result != FUNC_CACHE_EMPTY) {
      cache_set($_cid, $result, $func_info['bin']);
      $static[$_cid] = $result;
    }
  }
}

/**
 * Rebuild a cached functions table (bin).
 * All the cached functions in a specified bin will be rebuilded.
 *
 * First argument should be always a bin name string.
 * All the rest arguments will be passed to a func_clear_cache_func().
 *
 * @param ...
 *   Function arguments.
 * @param $batch_func
 *   Cached function name to rebuild. Used by delayed cache warmup process.
 * @param $batch_offset
 *   Cached function name to rebuild. Used by delayed cache warmup process.
 *
 * @see func_clear_cache_func()
 */
function func_clear_cache_bin($bin) {
  $static = &drupal_static('func_get_cache');

  $args = func_get_args();
  if (count($args) > 0) {
    $bin = array_shift($args);
  }

  $cache_setters = func_get_setters();

  foreach($cache_setters as $func => $func_info) {
    $func_info = $cache_setters[$func];
    $cid = func_cache_get_cid($func, $args, $func_info);

    if ($bin == $func_info['bin']) {
      $call_args = $args;
      array_unshift($call_args, $func);
      func_file_include($func_info);
      if (!function_exists($func)) {
        func_cache_notify('ERROR: Function cache-setter doesn\'t exist: ' . $func);
        return FALSE;
      }

      if (empty($args) && !empty($func_info['args'])) {
        // Rebuild all the function cache.
        func_rebuild_all_args($func, $func_info);
      }
      else {
        // Rebuild a basic function cache (without arguments).
        $result = call_user_func_array($func, $args);

        if ($result != FUNC_CACHE_EMPTY) {
          cache_set($cid, $result, $func_info['bin']);
          $static[$cid] = $result;
        }
      }
    }
  }
}

/**
 * Alter a single cache entry.
 *
 * Altering should be used only for setting single element for the big array cache for the
 * purpose of saving CPU and DB calls by not rebuilding entire cache entry.
 * Using of alter for rebuilding many items is a VERY BAD IDEA. You should consider of rebuilding
 * the whole array if you need to alter more than 1-2 items.
 *
 * @param $func string
 *   memoize function name
 * @param $data mixed
 *   altering data. Different types have different behavior:
 *   - If both cache entry and data are assoc arrays then cache entry will be merged with data
 *     (cache keys will be rewritten by data keys). Keys with NULL values will be removed from
 *     the cache entry.
 *   - If both cache entry and data are plain arrays then cache entry will be merged with data
 *     (data values will be added to cache values).
 *   - If cache entry is an object then it will be filled with keys and values provided by data.
 *   - As a fallback behavior cache entry will be replaced by provided data (not recommended, use 
 *     func_clear_cache_func() instead).
 * @param ...
 *   arguments to get cache entry
 * @return mixed
 *   modified cache entry.
 */
function func_alter_cache($func, $data) {
  $static = &drupal_static('func_get_cache');
  $cache_sets = &drupal_static(__FUNCTION__, array());

  // Get all passed args.
  $args = array_slice(func_get_args(), 2);

  // Prepare cache setters registry.
  $cache_setters = func_get_setters();

  // Prepare cached function hook info.
  $func_info = &$cache_setters[$func];
  $cid = func_cache_get_cid($func, $args, $func_info);
 
  // Get existing cache entry. It will be builded if not exists.
  array_unshift($args, $func);
  $existing_data = call_user_func_array('func_get_cache', $args);

  // Prepare new data.
  if (is_array($existing_data) && is_array($data)) {
    $new_data = &$existing_data;
    // If both existing and new data are arrays, merging them.
    foreach ($data as $key => $value) {
      if (is_null($value)) {
        if (isset($new_data[$key])) {
          unset($new_data[$key]);
        }
      }
      else {
        $new_data[$key] = $value;
      }
    }
  }
  else if (is_object($existing_data) && is_array($data)) {
    // If existing data is an object and provided altering data is array then
    // consider data as attributes list that should be changed in the stored object.
    $new_data = $existing_data;
    foreach ($data as $key => $value) {
      if ($value === NULL) {
        unset($new_data->$key);
      }
      else {
        $new_data->$key = $value;
      }
    }
  }
  else {
    // When there is no data or data is a plain value then just replace stored data
    // with a new one.
    $new_data = $data;
  }

  cache_set($cid, $new_data, $func_info['bin']);
  $static[$cid] = $new_data;

  return $new_data;
}

/**
 * Alter a single system core cache entry.
 *
 * @see func_alter_cache()
 */
function func_alter_cache_bootstrap($func, $data) {
  $static = &drupal_static('func_get_cache_bootstrap');
  $cache_sets = &drupal_static(__FUNCTION__, array());

  // Get all passed args.
  $args = array_slice(func_get_args(), 2);

  // Prepare cache setters registry.
  $cache_setters = func_get_setters_bootstrap();

  // Prepare cached function hook info.
  $func_info = &$cache_setters[$func];
  $cid = func_cache_get_cid($func, $args, $func_info);
 
  // Get existing cache entry. It will be builded if not exists.
  array_unshift($args, $func);
  $existing_data = call_user_func_array('func_get_cache_bootstrap', $args);

  // Prepare new data.
  if (is_array($existing_data) && is_array($data)) {
    $new_data = &$existing_data;
    // If both existing and new data are arrays, merging them.
    foreach ($data as $key => $value) {
      if (is_null($value)) {
        if (isset($new_data[$key])) {
          unset($new_data[$key]);
        }
      }
      else {
        $new_data[$key] = $value;
      }
    }
  }
  else if (is_object($existing_data) && is_array($data)) {
    // If existing data is an object and provided altering data is array then
    // consider data as attributes list that should be changed in the stored object.
    $new_data = $existing_data;
    foreach ($data as $key => $value) {
      if ($value === NULL) {
        unset($new_data->$key);
      }
      else {
        $new_data->$key = $value;
      }
    }
  }
  else {
    // When there is no data or data is a plain value then just replace stored data
    // with a new one.
    $new_data = $data;
  }

  cache_set($cid, $new_data, $func_info['bin']);
  $static[$cid] = $new_data;

  return $new_data;
}