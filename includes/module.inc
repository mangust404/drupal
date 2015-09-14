<?php

/**
 * @file
 * API for loading and interacting with Drupal modules.
 */

/**
 * Load all the modules that have been enabled in the system table.
 */
function module_load_all() {
  foreach (module_list(TRUE, FALSE) as $module) {
    drupal_load('module', $module);
  }
}

/**
 * Call a function repeatedly with each module in turn as an argument.
 */
function module_iterate($function, $argument = '') {
  foreach (module_list() as $name) {
    $function($name, $argument);
  }
}

/**
 * Collect a list of all loaded modules. During the bootstrap, return only
 * vital modules. See bootstrap.inc
 *
 * @param $refresh
 *   Whether to force the module list to be regenerated (such as after the
 *   administrator has changed the system settings).
 * @param $bootstrap
 *   Whether to return the reduced set of modules loaded in "bootstrap mode"
 *   for cached pages. See bootstrap.inc.
 * @param $sort
 *   By default, modules are ordered by weight and filename, settings this option
 *   to TRUE, module list will be ordered by module name.
 * @param $fixed_list
 *   (Optional) Override the module list with the given modules. Stays until the
 *   next call with $refresh = TRUE.
 * @return
 *   An associative array whose keys and values are the names of all loaded
 *   modules.
 */
function module_list($refresh = FALSE, $bootstrap = TRUE, $sort = FALSE, $fixed_list = NULL) {
  static $list, $sorted_list;

  if ($refresh || $fixed_list) {
    $list = array();
    $sorted_list = NULL;
    if ($fixed_list) {
      foreach ($fixed_list as $name => $module) {
        drupal_get_filename('module', $name, $module['filename']);
        $list[$name] = $name;
      }
    }
    else {
      if ($bootstrap) {
        $result = db_query("SELECT name, filename, throttle FROM {system} WHERE type = 'module' AND status = 1 AND bootstrap = 1 ORDER BY weight ASC, filename ASC");
      }
      else {
        $result = db_query("SELECT name, filename, throttle FROM {system} WHERE type = 'module' AND status = 1 ORDER BY weight ASC, filename ASC");
      }
      while ($module = db_fetch_object($result)) {
        if (file_exists($module->filename)) {
          // Determine the current throttle status and see if the module should be
          // loaded based on server load. We have to directly access the throttle
          // variables, since throttle.module may not be loaded yet.
          $throttle = ($module->throttle && variable_get('throttle_level', 0) > 0);
          if (!$throttle) {
            drupal_get_filename('module', $module->name, $module->filename);
            $list[$module->name] = $module->name;
          }
        }
      }
    }
  }
  if ($sort) {
    if (!isset($sorted_list)) {
      $sorted_list = $list;
      ksort($sorted_list);
    }
    return $sorted_list;
  }
  return $list;
}

/**
 * Builds a list of bootstrap modules and enabled modules and themes.
 * Backport from D7.
 *
 * @param $type
 *   The type of list to return:
 *   - module_enabled: All enabled modules.
 *   - bootstrap: All enabled modules required for bootstrap.
 *   - theme: All themes.
 *
 * @return
 *   An associative array of modules or themes, keyed by name. For $type
 *   'bootstrap', the array values equal the keys. For $type 'module_enabled'
 *   or 'theme', the array values are objects representing the respective
 *   database row, with the 'info' property already unserialized.
 *
 * @see module_list()
 * @see list_themes()
 */
function system_list($type) {
  $lists = &drupal_static(__FUNCTION__);

  // For bootstrap modules, attempt to fetch the list from cache if possible.
  // if not fetch only the required information to fire bootstrap hooks
  // in case we are going to serve the page from cache.
  if ($type == 'bootstrap') {
    if (isset($lists['bootstrap'])) {
      return $lists['bootstrap'];
    }
    if ($cached = cache_get('bootstrap_modules', 'cache')) {
      $bootstrap_list = $cached->data;
    }
    else {
      $result = db_query("SELECT name, filename FROM {system} WHERE status = 1 AND bootstrap = 1 AND type = 'module' ORDER BY weight ASC, name ASC");
      $bootstrap_list = array();
      while($item = db_fetch_array($result)) {
        $bootstrap_list[] = $item['name'];
      }
      cache_set('bootstrap_modules', $bootstrap_list, 'cache');
    }
    // To avoid a separate database lookup for the filepath, prime the
    // drupal_get_filename() static cache for bootstrap modules only.
    // The rest is stored separately to keep the bootstrap module cache small.
    foreach ($bootstrap_list as $module) {
      drupal_get_filename('module', $module->name, $module->filename);
    }
    // We only return the module names here since module_list() doesn't need
    // the filename itself.
    $lists['bootstrap'] = array_keys($bootstrap_list);
  }
  // Otherwise build the list for enabled modules and themes.
  elseif (!isset($lists['module_enabled'])) {
    if ($cached = cache_get('system_list', 'cache')) {
      $lists = $cached->data;
    }
    else {
      $lists = array(
        'module_enabled' => array(),
        'theme' => array(),
        'filepaths' => array(),
      );
      // The module name (rather than the filename) is used as the fallback
      // weighting in order to guarantee consistent behavior across different
      // Drupal installations, which might have modules installed in different
      // locations in the file system. The ordering here must also be
      // consistent with the one used in module_implements().
      $result = db_query("SELECT * FROM {system} WHERE type = 'theme' OR (type = 'module' AND status = 1) ORDER BY weight ASC, name ASC");
      while($record = db_fetch_object($result)) {
        $record->info = unserialize($record->info);
        // Build a list of all enabled modules.
        if ($record->type == 'module') {
          $lists['module_enabled'][$record->name] = $record;
        }
        // Build a list of themes.
        if ($record->type == 'theme') {
          $lists['theme'][$record->name] = $record;
        }
        // Build a list of filenames so drupal_get_filename can use it.
        if ($record->status) {
          $lists['filepaths'][] = array('type' => $record->type, 'name' => $record->name, 'filepath' => $record->filename);
        }
      }
      foreach ($lists['theme'] as $key => $theme) {
        if (!empty($theme->info['base theme'])) {
          // Make a list of the theme's base themes.
          require_once DRUPAL_ROOT . '/includes/theme.inc';
          $lists['theme'][$key]->base_themes = drupal_find_base_themes($lists['theme'], $key);
          // Don't proceed if there was a problem with the root base theme.
          if (!current($lists['theme'][$key]->base_themes)) {
            continue;
          }
          // Determine the root base theme.
          $base_key = key($lists['theme'][$key]->base_themes);
          // Add to the list of sub-themes for each of the theme's base themes.
          foreach (array_keys($lists['theme'][$key]->base_themes) as $base_theme) {
            $lists['theme'][$base_theme]->sub_themes[$key] = $lists['theme'][$key]->info['name'];
          }
          // Add the base theme's theme engine info.
          $lists['theme'][$key]->info['engine'] = isset($lists['theme'][$base_key]->info['engine']) ? $lists['theme'][$base_key]->info['engine'] : 'theme';
        }
        else {
          // A plain theme is its own engine.
          $base_key = $key;
          if (!isset($lists['theme'][$key]->info['engine'])) {
            $lists['theme'][$key]->info['engine'] = 'theme';
          }
        }
        // Set the theme engine prefix.
        $lists['theme'][$key]->prefix = ($lists['theme'][$key]->info['engine'] == 'theme') ? $base_key : $lists['theme'][$key]->info['engine'];
      }
      cache_set('system_list', $lists, 'cache');
    }
    // To avoid a separate database lookup for the filepath, prime the
    // drupal_get_filename() static cache with all enabled modules and themes.
    foreach ($lists['filepaths'] as $item) {
      drupal_get_filename($item['type'], $item['name'], $item['filepath']);
    }
  }

  return $lists[$type];
}

/**
 * Resets all system_list() caches. Backport from D7
 */
function system_list_reset() {
  drupal_static_reset('system_list');
  drupal_static_reset('system_rebuild_module_data');
  drupal_static_reset('list_themes');
  cache_clear_all('bootstrap_modules', 'cache');
  cache_clear_all('system_list', 'cache');
}

/**
 * Function wrapper for backward compatibility
 */
function module_rebuild_cache() {
  return system_rebuild_module_data(TRUE);
}

/**
 * Determines which modules require and are required by each module. Backport from D7
 *
 * @param $files
 *   The array of filesystem objects used to rebuild the cache.
 *
 * @return
 *   The same array with the new keys for each module:
 *   - requires: An array with the keys being the modules that this module
 *     requires.
 *   - required_by: An array with the keys being the modules that will not work
 *     without this module.
 */
function _module_build_dependencies($files) {
  require_once DRUPAL_ROOT . '/includes/graph.inc';
  foreach ($files as $filename => $file) {
    $graph[$file->name]['edges'] = array();
    if (isset($file->info['dependencies']) && is_array($file->info['dependencies'])) {
      foreach ($file->info['dependencies'] as $dependency) {
        $dependency_data = drupal_parse_dependency($dependency);
        $graph[$file->name]['edges'][$dependency_data['name']] = $dependency_data;
      }
    }
  }
  drupal_depth_first_search($graph);
  foreach ($graph as $module => $data) {
    $files[$module]->required_by = isset($data['reverse_paths']) ? $data['reverse_paths'] : array();
    $files[$module]->requires = isset($data['paths']) ? $data['paths'] : array();
    $files[$module]->sort = $data['weight'];
  }
  return $files;
}

/**
 * Determine whether a given module exists.
 *
 * @param $module
 *   The name of the module (without the .module extension).
 * @return
 *   TRUE if the module is both installed and enabled.
 */
function module_exists($module) {
  $list = module_list();
  return array_key_exists($module, $list);
}

/**
 * Load a module's installation hooks.
 */
function module_load_install($module) {
  // Make sure the installation API is available
  include_once './includes/install.inc';

  module_load_include('install', $module);
}

/**
 * Loads a module include file.
 *
 * Examples:
 * @code
 *   // Load node.admin.inc from the node module.
 *   module_load_include('inc', 'node', 'node.admin');
 *   // Load content_types.inc from the node module.
 *   module_load_include('inc', 'node', 'content_types');
 * @endcode
 *
 * Do not use this function to load an install file, use module_load_install()
 * instead. Do not use this function in a global context since it requires
 * Drupal to be fully bootstrapped, use require_once DRUPAL_ROOT . '/path/file'
 * instead.
 *
 * @param $type
 *   The include file's type (file extension).
 * @param $module
 *   The module to which the include file belongs.
 * @param $name
 *   (optional) The base file name (without the $type extension). If omitted,
 *   $module is used; i.e., resulting in "$module.$type" by default.
 *
 * @return
 *   The name of the included file, if successful; FALSE otherwise.
 */
function module_load_include($type, $module, $name = NULL) {
  if (!isset($name)) {
    $name = $module;
  }

  if (function_exists('drupal_get_path')) {
    $file = DRUPAL_ROOT . '/' . drupal_get_path('module', $module) . "/$name.$type";
    if (is_file($file)) {
      require_once $file;
      return $file;
    }
  }
  return FALSE;
}

/**
 * Load an include file for each of the modules that have been enabled in
 * the system table.
 */
function module_load_all_includes($type, $name = NULL) {
  $modules = module_list();
  foreach ($modules as $module) {
    module_load_include($type, $module, $name);
  }
}

/**
 * Enables or installs a given list of modules. Backport from D7
 *
 * Definitions:
 * - "Enabling" is the process of activating a module for use by Drupal.
 * - "Disabling" is the process of deactivating a module.
 * - "Installing" is the process of enabling it for the first time or after it
 *   has been uninstalled.
 * - "Uninstalling" is the process of removing all traces of a module.
 *
 * Order of events:
 * - Gather and add module dependencies to $module_list (if applicable).
 * - For each module that is being enabled:
 *   - Install module schema and update system registries and caches.
 *   - If the module is being enabled for the first time or had been
 *     uninstalled, invoke hook_install() and add it to the list of installed
 *     modules.
 *   - Invoke hook_enable().
 * - Invoke hook_modules_installed().
 * - Invoke hook_modules_enabled().
 *
 * @param $module_list
 *   An array of module names.
 * @param $enable_dependencies
 *   If TRUE, dependencies will automatically be added and enabled in the
 *   correct order. This incurs a significant performance cost, so use FALSE
 *   if you know $module_list is already complete and in the correct order.
 *
 * @return
 *   FALSE if one or more dependencies are missing, TRUE otherwise.
 *
 * @see hook_install()
 * @see hook_enable()
 * @see hook_modules_installed()
 * @see hook_modules_enabled()
 */
function module_enable($module_list, $enable_dependencies = TRUE) {
  if ($enable_dependencies) {
    // Get all module data so we can find dependencies and sort.
    $module_data = system_rebuild_module_data();
    // Create an associative array with weights as values.
    $module_list = array_flip(array_values($module_list));

    while (list($module) = each($module_list)) {
      if (!isset($module_data[$module])) {
        // This module is not found in the filesystem, abort.
        return FALSE;
      }
      if ($module_data[$module]->status) {
        // Skip already enabled modules.
        unset($module_list[$module]);
        continue;
      }
      $module_list[$module] = $module_data[$module]->sort;

      // Add dependencies to the list, with a placeholder weight.
      // The new modules will be processed as the while loop continues.
      foreach (array_keys($module_data[$module]->requires) as $dependency) {
        if (!isset($module_list[$dependency])) {
          $module_list[$dependency] = 0;
        }
      }
    }

    if (!$module_list) {
      // Nothing to do. All modules already enabled.
      return TRUE;
    }

    // Sort the module list by pre-calculated weights.
    arsort($module_list);
    $module_list = array_keys($module_list);
  }
  // Required for module installation checks.
  include_once DRUPAL_ROOT . '/includes/install.inc';

  $modules_installed = array();
  $modules_enabled = array();
  foreach ($module_list as $module) {
    // Only process modules that are not already enabled.
    $existing = db_fetch_object(db_query("SELECT status FROM {system} WHERE type = '%s' AND name = '%s'", 'module', $module));
    if ($existing->status == 0) {
      // Load the module's code.
      drupal_load('module', $module);
      module_load_install($module);

      // Update the database and module list to reflect the new module. This
      // needs to be done first so that the module's hook implementations,
      // hook_schema() in particular, can be called while it is being
      // installed.
      db_query("UPDATE {system} SET status = 1 WHERE type = 'module' AND name = '%s'", $module);

      // Refresh the module list to include all the installed modules.
      system_list_reset();
      module_list(TRUE, FALSE);
      module_implements('', FALSE, TRUE);
      _system_update_bootstrap_status();
      // Refresh the schema to include it.
      drupal_get_schema(NULL, TRUE);
      // Update the theme registry to include it.
      drupal_theme_rebuild();

      // Now install the module if necessary.
      if (drupal_get_installed_schema_version($module, TRUE) == SCHEMA_UNINSTALLED) {
        // D6 modules should not install schema automatically. 
        // Schema installation is being invoked from hook_install 
        //drupal_install_schema($module);

        // Set the schema version to the number of the last update provided
        // by the module.
        $versions = drupal_get_schema_versions($module);
        $version = $versions ? max($versions) : SCHEMA_INSTALLED;

        // If the module has no current updates, but has some that were
        // previously removed, set the version to the value of
        // hook_update_last_removed().
        if ($last_removed = module_invoke($module, 'update_last_removed')) {
          $version = max($version, $last_removed);
        }
        drupal_set_installed_schema_version($module, $version);
        // Allow the module to perform install tasks.
        module_invoke($module, 'install');
        // Record the fact that it was installed.
        $modules_installed[] = $module;
        watchdog('system', '%module module installed.', array('%module' => $module), WATCHDOG_INFO);
      }

    }
  }

  if(!empty($modules_installed)) {
    foreach ($modules_installed as $module) {
      // Enable the module.
      module_invoke($module, 'enable');

      // Record the fact that it was enabled.
      $modules_enabled[] = $module;
      watchdog('system', '%module module enabled.', array('%module' => $module), WATCHDOG_INFO);
    }
  }

  // If any modules were newly installed, invoke hook_modules_installed().
  if (!empty($modules_installed)) {
    module_invoke_all('modules_installed', $modules_installed);
  }

  // If any modules were newly enabled, invoke hook_modules_enabled().
  if (!empty($modules_enabled)) {
    module_invoke_all('modules_enabled', $modules_enabled);
  }

  return TRUE;
}

/**
 * Disable a given set of modules.
 *
 * @param $module_list
 *   An array of module names.
 */
function module_disable($module_list, $disable_dependents = TRUE) {
  if ($disable_dependents) {
    // Get all module data so we can find dependents and sort.
    $module_data = system_rebuild_module_data();
    // Create an associative array with weights as values.
    $module_list = array_flip(array_values($module_list));

    $profile = drupal_get_profile();

    while (list($module) = each($module_list)) {
      if (!isset($module_data[$module]) || !$module_data[$module]->status) {
        // This module doesn't exist or is already disabled, skip it.
        unset($module_list[$module]);
        continue;
      }
      $module_list[$module] = $module_data[$module]->sort;

      // Add dependent modules to the list, with a placeholder weight.
      // The new modules will be processed as the while loop continues.
      foreach ($module_data[$module]->required_by as $dependent => $dependent_data) {
        if (!isset($module_list[$dependent]) && $dependent != $profile) {
          $module_list[$dependent] = 0;
        }
      }
    }

    // Sort the module list by pre-calculated weights.
    asort($module_list);
    $module_list = array_keys($module_list);
  }

  $invoke_modules = array();
  foreach ($module_list as $module) {
    if (module_exists($module)) {
      // Check if node_access table needs rebuilding.
      if (!node_access_needs_rebuild() && module_hook($module, 'node_grants')) {
        node_access_needs_rebuild(TRUE);
      }

      module_load_install($module);
      module_invoke($module, 'disable');
      db_query("UPDATE {system} SET status = %d, throttle = %d WHERE type = '%s' AND name = '%s'", 0, 0, 'module', $module);
      $invoke_modules[] = $module;
    }
  }

  if (!empty($invoke_modules)) {
    // Refresh the module list to exclude the disabled modules.
    system_list_reset();
    module_list(TRUE, FALSE);
    module_implements('', FALSE, TRUE);
    // Invoke hook_modules_disabled before disabling modules,
    // so we can still call module hooks to get information.
    module_invoke_all('modules_disabled', $invoke_modules);
    _system_update_bootstrap_status();
    // Update the theme registry to remove the newly-disabled module.
    drupal_theme_rebuild();
  }

  // If there remains no more node_access module, rebuilding will be
  // straightforward, we can do it right now.
  if (node_access_needs_rebuild() && count(module_implements('node_grants')) == 0) {
    node_access_rebuild();
  }}

/**
 * @defgroup hooks Hooks
 * @{
 * Allow modules to interact with the Drupal core.
 *
 * Drupal's module system is based on the concept of "hooks". A hook is a PHP
 * function that is named foo_bar(), where "foo" is the name of the module
 * (whose filename is thus foo.module) and "bar" is the name of the hook. Each
 * hook has a defined set of parameters and a specified result type.
 *
 * To extend Drupal, a module need simply implement a hook. When Drupal wishes
 * to allow intervention from modules, it determines which modules implement a
 * hook and calls that hook in all enabled modules that implement it.
 *
 * The available hooks to implement are explained here in the Hooks section of
 * the developer documentation. The string "hook" is used as a placeholder for
 * the module name in the hook definitions. For example, if the module file is
 * called example.module, then hook_help() as implemented by that module would
 * be defined as example_help().
 */

/**
 * Determines whether a module implements a hook. Backport from D7
 *
 * @param $module
 *   The name of the module (without the .module extension).
 * @param $hook
 *   The name of the hook (e.g. "help" or "menu").
 *
 * @return
 *   TRUE if the module is both installed and enabled, and the hook is
 *   implemented in that module.
 */
function module_hook($module, $hook) {
  $function = $module . '_' . $hook;
  if (function_exists($function)) {
    return TRUE;
  }
  // If the hook implementation does not exist, check whether it may live in an
  // optional include file registered via hook_hook_info().
  $hook_info = module_hook_info();
  if (isset($hook_info[$hook]['group'])) {
    module_load_include('inc', $module, $module . '.' . $hook_info[$hook]['group']);
    if (function_exists($function)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Determines which modules are implementing a hook. Backport from D7
 *
 * @param $hook
 *   The name of the hook (e.g. "help" or "menu").
 * @param $sort
 *   By default, modules are ordered by weight and filename, settings this option
 *   to TRUE, module list will be ordered by module name.
 * @param $reset
 *   For internal use only: Whether to force the stored list of hook
 *   implementations to be regenerated (such as after enabling a new module,
 *   before processing hook_enable).
 *
 * @return
 *   An array with the names of the modules which are implementing this hook.
 *
 * @see module_implements_write_cache()
 */
function module_implements($hook, $sort = FALSE, $reset = FALSE) {
  // Use the advanced drupal_static() pattern, since this is called very often.
  static $drupal_static_fast;
  if (!isset($drupal_static_fast)) {
    $drupal_static_fast['implementations'] = &drupal_static(__FUNCTION__);
  }
  $implementations = &$drupal_static_fast['implementations'];

  // We maintain a persistent cache of hook implementations in addition to the
  // static cache to avoid looping through every module and every hook on each
  // request. Benchmarks show that the benefit of this caching outweighs the
  // additional database hit even when using the default database caching
  // backend and only a small number of modules are enabled. The cost of the
  // cache_get() is more or less constant and reduced further when non-database
  // caching backends are used, so there will be more significant gains when a
  // large number of modules are installed or hooks invoked, since this can
  // quickly lead to module_hook() being called several thousand times
  // per request.
  if ($reset) {
    $implementations = array();
    cache_set('module_implements', array(), 'cache');
    drupal_static_reset('module_hook_info');
    drupal_static_reset('drupal_alter');
    cache_clear_all('hook_info', 'cache');
    return;
  }

  // Fetch implementations from cache.
  if (empty($implementations) && function_exists('cache_get')) {
    $implementations = cache_get('module_implements', 'cache');
    if (empty($implementations)) {
      $implementations = array();
    }
    else {
      $implementations = $implementations->data;
    }
  }

  if (!isset($implementations[$hook])) {
    // The hook is not cached, so ensure that whether or not it has
    // implementations, that the cache is updated at the end of the request.
    $implementations['#write_cache'] = TRUE;
    $hook_info = module_hook_info();
    $implementations[$hook] = array();
    $list = module_list(FALSE, FALSE, $sort);
    foreach ($list as $module) {
      $include_file = isset($hook_info[$hook]['group']) && module_load_include('inc', $module, $module . '.' . $hook_info[$hook]['group']);
      // Since module_hook() may needlessly try to load the include file again,
      // function_exists() is used directly here.
      if (function_exists($module . '_' . $hook)) {
        $implementations[$hook][$module] = $include_file ? $hook_info[$hook]['group'] : FALSE;
      }
    }
    // Allow modules to change the weight of specific implementations but avoid
    // an infinite loop.
    if ($hook != 'module_implements_alter') {
      drupal_alter('module_implements', $implementations[$hook], $hook);
    }
  }
  else {
    foreach ($implementations[$hook] as $module => $group) {
      // If this hook implementation is stored in a lazy-loaded file, so include
      // that file first.
      if ($group) {
        module_load_include('inc', $module, "$module.$group");
      }
      // It is possible that a module removed a hook implementation without the
      // implementations cache being rebuilt yet, so we check whether the
      // function exists on each request to avoid undefined function errors.
      // Since module_hook() may needlessly try to load the include file again,
      // function_exists() is used directly here.
      if (!function_exists($module . '_' . $hook)) {
        // Clear out the stale implementation from the cache and force a cache
        // refresh to forget about no longer existing hook implementations.
        unset($implementations[$hook][$module]);
        $implementations['#write_cache'] = TRUE;
      }
    }
  }

  return array_keys($implementations[$hook]);
}

/**
 * Retrieves a list of hooks that are declared through hook_hook_info(). Backport from D7
 *
 * @return
 *   An associative array whose keys are hook names and whose values are an
 *   associative array containing a group name. The structure of the array
 *   is the same as the return value of hook_hook_info().
 *
 * @see hook_hook_info()
 */
function module_hook_info() {
  // This function is indirectly invoked from bootstrap_invoke_all(), in which
  // case common.inc, subsystems, and modules are not loaded yet, so it does not
  // make sense to support hook groups resp. lazy-loaded include files prior to
  // full bootstrap.
  if (drupal_bootstrap(NULL, FALSE) < DRUPAL_BOOTSTRAP_FULL) {
    return array();
  }
  $hook_info = &drupal_static(__FUNCTION__);

  if (!isset($hook_info)) {
    $hook_info = array();
    $cache = cache_get('hook_info', 'cache');
    if (empty($cache)) {
      // Rebuild the cache and save it.
      // We can't use module_invoke_all() here or it would cause an infinite
      // loop.
      foreach (module_list() as $module) {
        $function = $module . '_hook_info';
        if (function_exists($function)) {
          $result = $function();
          if (isset($result) && is_array($result)) {
            $hook_info = array_merge_recursive($hook_info, $result);
          }
        }
      }
      // We can't use drupal_alter() for the same reason as above.
      foreach (module_list() as $module) {
        $function = $module . '_hook_info_alter';
        if (function_exists($function)) {
          $function($hook_info);
        }
      }
      cache_set('hook_info', $hook_info, 'cache');
    }
    else {
      $hook_info = $cache->data;
    }
  }

  return $hook_info;
}

/**
 * Invoke a hook in a particular module.
 *
 * @param $module
 *   The name of the module (without the .module extension).
 * @param $hook
 *   The name of the hook to invoke.
 * @param ...
 *   Arguments to pass to the hook implementation.
 * @return
 *   The return value of the hook implementation.
 */
function module_invoke($module, $hook) {
  $args = func_get_args();
  // Remove $module and $hook from the arguments.
  unset($args[0], $args[1]);
  if (module_hook($module, $hook)) {
    return call_user_func_array($module . '_' . $hook, $args);
  }
}

/**
 * Writes the hook implementation cache. Backport from D7
 *
 * @see module_implements()
 */
function module_implements_write_cache() {
  $implementations = &drupal_static('module_implements');
  if (isset($implementations['#write_cache'])) {
    unset($implementations['#write_cache']);
    cache_set('module_implements', $implementations, 'cache');
  }
}

/**
 * Invoke a hook in all enabled modules that implement it.
 *
 * @param $hook
 *   The name of the hook to invoke.
 * @param ...
 *   Arguments to pass to the hook.
 * @return
 *   An array of return values of the hook implementations. If modules return
 *   arrays from their implementations, those are merged into one array.
 */
function module_invoke_all() {
  $args = func_get_args();
  $hook = $args[0];
  unset($args[0]);
  $return = array();
  foreach (module_implements($hook) as $module) {
    $function = $module .'_'. $hook;
    $result = call_user_func_array($function, $args);
    if (isset($result) && is_array($result)) {
      $return = array_merge_recursive($return, $result);
    }
    else if (isset($result)) {
      $return[] = $result;
    }
  }

  return $return;
}

/**
 * @} End of "defgroup hooks".
 */

/**
 * Array of modules required by core.
 */
function drupal_required_modules() {
  return array('block', 'filter', 'node', 'system', 'user');
}
