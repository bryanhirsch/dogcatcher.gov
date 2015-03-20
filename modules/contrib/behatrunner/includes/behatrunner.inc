<?php

/**
 * @file
 * Utility functions for Behat Runner.
 */

/**
 * Discovers and registers all modules implementing hook_behat_info().
 *
 * @return array
 *   An array of the modules that were discovered.
 */
function behatrunner_discover_modules() {
  $modules = module_list();

  $discovered = array();
  foreach ($modules as $module) {
    $registered = behatrunner_register_module($module);
    if ($registered) {
      $discovered[] = $module;
    }
  }

  return $discovered;
}

/**
 * Registers the locations of all scenarios for a given module.
 *
 * @param string $module
 *   The machine name of a module.
 *
 * @return bool
 *   TRUE is the module contained features to register.
 */
function behatrunner_register_module($module) {
  $registered = behatrunner_module_is_registered($module);
  // If this module has been registered, remove its scenario registrations.
  if ($registered) {
    behatrunner_deregister_module_scenarios($module);
  }

  // Discover all features for the given module.
  $features = behatrunner_discover_module_features($module);
  foreach ($features as $feature) {
    // Register all scenarios for each feature.
    $status = behatrunner_register_feature_scenarios($feature);
  }

  return (bool) $features;
}

/**
 * Discovers all features for a given module.
 *
 * @param string $module
 *   The machine name of a module.
 *
 * @return array
 *   An associative array of features, keyed by filename.
 */
function behatrunner_discover_module_features($module) {
  $system_path = behatrunner_module_features_path($module);
  $feature_files = file_scan_directory($system_path, '/.*\.feature$/', array('recurse' => TRUE));
  $parser = behatrunner_get_parser();

  $features = array();
  foreach ($feature_files as $feature_file) {
    $feature = $parser->parse(file_get_contents($feature_file->uri));
    $feature->setFile($feature_file->uri);
    $feature->drupalModule = $module;
    $features[$feature_file->filename] = $feature;
  }

  return $features;
}

/**
 * Registers all scenarios for a given feature.
 *
 * @param object $feature
 *   A Behat feature.
 */
function behatrunner_register_feature_scenarios($feature) {
  $feature_location = behatrunner_convert_absolute_to_relative_path($feature->getFile());
  $scenarios = $feature->getScenarios();

  foreach ($scenarios as $scenario) {
    $data['title'] = $scenario->getTitle();
    // @todo Figure out why this isn't getting the feature!
    $data['feature'] = $scenario->getFeature()->getTitle();
    $data['location'] = $feature_location . ':' . $scenario->getLine();
    $data['module'] = $feature->drupalModule;
    $entity = entity_create('behatrunner_scenario', $data);
    $entity->save();
  }

  // @todo return somesthing here, particularly on failure.
}

/**
 * Returns a Behat Parser object for parsing Gherkin.
 *
 * @return object
 *   An Behat\Gherkin\Parser object initialized with default English keywords.
 */
function behatrunner_get_parser() {
  $keywords = new Behat\Gherkin\Keywords\ArrayKeywords(array(
    'en' => array(
      'feature'          => 'Feature',
      'background'       => 'Background',
      'scenario'         => 'Scenario',
      'scenario_outline' => 'Scenario Outline|Scenario Template',
      'examples'         => 'Examples|Scenarios',
      'given'            => 'Given',
      'when'             => 'When',
      'then'             => 'Then',
      'and'              => 'And',
      'but'              => 'But',
    ),
  ));
  $lexer  = new Behat\Gherkin\Lexer($keywords);
  $parser = new Behat\Gherkin\Parser($lexer);

  return $parser;
}

/**
 * Converts a path relative to the drupal root into an absolute system path.
 *
 * @param string $relative_path
 *   A file path, relative the the drupal root.
 *
 * @return string
 *   The absolute system path.
 */
function behatrunner_convert_relative_to_absolute_path($relative_path) {
  return DRUPAL_ROOT . '/' . $relative_path;
}

/**
 * Converts an absolute system path to a path relative to the drupal root.
 *
 * @param string $absolute_path
 *   An absolute system path.
 *
 * @return string
 *   The relative path.
 */
function behatrunner_convert_absolute_to_relative_path($absolute_path) {
  return str_replace(DRUPAL_ROOT . '/', '', $absolute_path);
}

/**
 * Checks whether a given module containing behat scenarios is registered.
 *
 * @param string $module
 *   The module to check.
 *
 * @return bool
 *   TRUE if the module has behat scenarios registered with behatrunner.
 */
function behatrunner_module_is_registered($module) {
  $query = 'SELECT bsid FROM {behatrunner_scenario}';
  $query .= ' WHERE module=:module LIMIT 1';
  $result = db_query($query, array(':module' => $module));
  $record = $result->fetchObject();

  return (boolean) $record;
}

/**
 * Looks up all registered scenarios for the given module.
 *
 * @param string $module
 *   The module name.
 *
 * @return array
 *   An array of locations for registered scenarios for the given module, keyed
 *   by entitiy id.
 */
function behatrunner_get_module_scenario_registrations($module) {
  $query = 'SELECT bsid, location FROM {behatrunner_scenario}';
  $query .= ' WHERE module=:module';
  $result = db_query($query, array(':module' => $module));
  $registrations = $result->fetchAllKeyed(0, 1);

  return $registrations;
}

/**
 * De-registers a test location.
 *
 * @param int $bsid
 *   The id for this location.
 */
function behatrunner_deregister_scenario($bsid) {
  entity_delete('behatrunner_scenario', $bsid);
}

/**
 * De-registers all scenario registrations for a given module.
 *
 * @param string $module
 *   The name for this location.
 *
 * @return bool
 *   FALSE if the given entity type isn't compatible to the CRUD API.
 */
function behatrunner_deregister_module_scenarios($module) {
  $scenario_registrations = behatrunner_get_module_scenario_registrations($module);
  $entity_ids = array_keys($scenario_registrations);

  return entity_delete_multiple('behatrunner_scenario', $entity_ids);
}

/**
 * Gets environmental parameters for Behat.
 *
 * @return array
 *   An associateive array of environmental Behat parameters.
 */
function behatrunner_get_env_params() {
  $behat_params = parse_url(getenv('BEHAT_PARAMS'));

  return $behat_params;
}

/**
 * Sets an environmental Behat parameter.
 *
 * @param string $key
 *   The parameter key.
 *
 * @param string $value
 *   The parameter value.
 *
 * @return bool
 *   TRUE if parameter was set successfully.
 */
function behatrunner_set_env_param($key, $value) {
  $behat_params = parse_url(getenv('BEHAT_PARAMS'));
  $behat_params[$key] = $value;

  foreach ($behat_params as $param_key => $param_value) {
    if ($param_value === '') {
      unset($behat_params[$param_key]);
    }
  }

  $behat_params_value = http_build_query($behat_params);
  return putenv("BEHAT_PARAMS=$behat_params_value");
}

/**
 * Execute Behat tests.
 *
 * @param array $scenarios
 *   An array of scenario entities.
 *
 * @param array $tags
 *   An tags to run. If empty, all tests will be run.
 *
 * @param array $formats
 *   An array of formats to output. Valid values are:
 *   - pretty
 *   - progress
 *   - html
 *   - junit
 *   - failed
 *   - snippets
 *
 * @return string
 *   The command line output.
 */
function behatrunner_execute_tests($scenarios = array(), $tags = array(), $formats = array()) {
  composer_manager_register_autoloader();
  $results = '';

  $tags = behatrunner_tags_argument_build($tags);
  $formats = behatrunner_format_arguments_build($formats);
  $config = behatrunner_config_argument_build();

  $vendor = composer_manager_vendor_dir();
  $binary_path = $vendor . '/bin/behat';

  if (!$scenarios) {
    $scenarios = entity_load('behatrunner_scenario');
  }
  foreach ($scenarios as $scenario) {
    $scenario_location = ' ' . DRUPAL_ROOT . '/' . $scenario->location;
    $features_path = behatrunner_module_features_path($scenario->module);
    $bootstrap_path = behatrunner_module_bootstrap_path($scenario->module);

    // Construct the command.
    $command = array(
      $binary_path,
      $formats,
      $scenario_location,
      $config,
    );

    // @todo Replace with implementation of behat_set_env_params(). We should
    // not overwrite the entire BEHAT_PARAMS variable, only a specific key.
    // behat_set_env_param('paths[features]', $features_path);
    putenv("BEHAT_PARAMS=paths[features]=$features_path&paths[bootstrap]=$bootstrap_path");

    $results .= behatrunner_execute_tests_command($command);

    if (!empty($result_file_location)) {
      if (variable_get('behatrunner_log', 'filesystem_database') == "filesystem_database") {
        // Log results to database.
        $junit_result = behatrunner_parse_junit($result_file_location);
        // @todo Complete logging function.
        behatrunner_log_scenario_result($junit_result);
      }
    }
  }

  return $results;
}

/**
 * Builds the --formats and --out arguments for the behat command.
 *
 * @param array $formats
 *   An array of behat output formats.
 *
 * @return string
 *   A snippet of the behat command containg format flag.
 */
function behatrunner_format_arguments_build($formats) {
  if (!$formats) {
    $formats = array('progress', 'junit');
  }

  // Build an array of output destinations for each format.
  $out_dests = array();
  foreach ($formats as $format) {
    // Junit XML is output to file system.
    if ($format == 'junit') {
      $dest_dir = 'public://behat';
      $output_dir = file_prepare_directory($dest_dir, FILE_CREATE_DIRECTORY);
      $wrapper = file_stream_wrapper_get_instance_by_uri($dest_dir);
      $output_dest = $wrapper->realpath();
      $out_dests[$format] = $output_dest;
    }
    // Other formats are written to stdout.
    else {
      $out_dests[$format] = '';
    }
  }

  $formats = '-f ' . implode(',', $formats);
  $out = "--out='" . implode(',', $out_dests) . ",'";

  return $formats . ' ' . $out;
}

/**
 * Builds the --tags argument for the behat command.
 *
 * @param array $tags
 *   An array of behat tags.
 *
 * @return string
 *   The --tags argument. E.g., "--tags=@sometag".
 *
 * @todo Refactor this, it doesn't really work!
 */
function behatrunner_tags_argument_build($tags) {
  $tags_string = '';
  if (count($tags) > 0) {
    $tags_string = ' --tags "~@';
    $tags_string .= implode($tags, '&&@');
    $tags_string .= '"';
  }

  return $tags_string;
}

/**
 * Builds the --config argument for the behat command.
 *
 * @return string
 *   A snippet of behat command containing the configuration flag.
 */
function behatrunner_config_argument_build() {
  $config_file = drupal_realpath('public://behatrunner/behat.yml');

  return '--config  ' . $config_file;
}

/**
 * Executes a command via the Symfony Process compontet.
 *
 * @param array $command
 *   An array of strings, to be imploded and run as a command.
 *
 * @return string
 *   The output of the executed command.
 */
function behatrunner_execute_tests_command($command) {

  $command = implode(' ', $command);
  $process = new \Symfony\Component\Process\Process($command);
  $process->run(function ($type, $buffer) {
    print $buffer;
  });

  if (!$process->isSuccessful()) {
    $output = $process->getErrorOutput();
  }
  else {
    $output = $process->getOutput();
  }

  return $output;
}

/**
 * Parses a Behat feature result in junit format to a PHP array.
 *
 * @param string $result_file_location
 *   The system path of the junit result file.
 *
 * @return array
 *   An array of the parsed junit result xml.
 */
function behatrunner_parse_junit($result_file_location) {
  $xml = simplexml_load_file($result_file_location);
  $attributes = (array) $xml->testcase->attributes();
  $result = $attributes['@attributes'];
  $failure = (array) $xml->testcase->failure && $xml->testcase->failure->attributes();
  $result['message'] = ($failure ? $failure['@attributes']['message'] : '');
  $result['status'] = ($failure ? 1 : 0);

  return $result;
}

/**
 * Saves results from Behat scenario runs to the database.
 *
 * @param array $junit_results
 *   The parsed junit result xml.
 */
function behatrunner_log_scenario_result($junit_results) {
  $scenarios = entity_load('behatrunner_scenario');

  // We need to key the entities by their title because the junit results are
  // keyed by title.
  $scenarios_by_title = array();
  foreach ($scenarios as $scenario) {
    $scenarios_by_title[$scenario->title] = $scenario;
  }

  // Delete log entries for previous runs for this scenario.
  $num_deleted = db_delete('behatrunner_log')
    ->condition('bsid', $junit_results->bsid)
    ->execute();

  $values = array(
    'bsid' => 1,
    'duration' => $junit_results['time'],
    'assertions' => $junit_results['assertions'],
    'message' => $junit_results['message'],
    'status' => $junit_results['status'],
    'timestamp' => REQUEST_TIME,
  );

  // Insert new log entry for current run of this Behat scenario.
  $blid = db_insert('behatrunner_log')
    ->fields($values)
    ->execute();

  // Update scenario entity with the id of the new log entry.
  // $scenario->blid = $blid;
  // @todo save entity with new blid.
}

/**
 * Determine if there are any scenarios registered in the database.
 *
 * @return bool
 *   TRUE if at least one behat scenario was found in the database.
 */
function behatrunner_scenarios_exist() {

  // Check if there are any registrations.
  $query = 'SELECT bsid FROM {behatrunner_scenario} LIMIT 1';
  $result = db_query($query);
  $record = $result->fetchObject();

  return (bool) $record;
}

/**
 * Returns the system file path for a given module's Behat features.
 *
 * @param string $module
 *   The module for which to find the features system path.
 *
 * @return string
 *   The system path for the module's Behat features directory.
 */
function behatrunner_module_features_path($module) {
  $system_path = DRUPAL_ROOT . '/' . drupal_get_path('module', $module) . '/features';

  return $system_path;
}


/**
 * Returns the system file path for a given module's bootstrap directory.
 *
 * The bootstrap directory must contain a FeatureContext.php class.
 *
 * @param string $module
 *   The module for which to find the features system path.
 *
 * @return string
 *   The system path for the module's Behat bootstrap directory.
 */
function behatrunner_module_bootstrap_path($module) {
  if (file_exists(drupal_get_path('module', $module) . '/features/bootstrap/FeatureContext.php')) {
    $system_path = DRUPAL_ROOT . '/' . drupal_get_path('module', $module) . '/features/bootstrap';
  }
  else {
    $system_path = DRUPAL_ROOT . '/' . drupal_get_path('module', 'behatrunner') . '/features/bootstrap';
  }

  return $system_path;
}
