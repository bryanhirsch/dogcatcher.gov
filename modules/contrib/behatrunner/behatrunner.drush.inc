<?php
/**
 * @file
 * Drush integration implementation
 */

/**
 * Implements hook_drush_help().
 */
function behatrunner_drush_help($command) {
  switch ($command) {
    case 'drush:br':
      return dt('Run Behat Runner');
  }
}

/**
 * Implements hook_drush_command().
 */
function behatrunner_drush_command() {
  $items = array();

  $items['behat-list-scenarios'] = array(
    'description' => dt('List registered Behat scenarios.'),
    'examples' => array(
      'Standard example' => 'drush behat-list-scenarios',
    ),
    'aliases' => array('bls'),
  );
  $items['behat-register-modules'] = array(
    'description' => dt('Discover and register Behat scenarios. If no argument is specified, all discoverable modules will be registered.'),
    'arguments' => array(
      'module' => '(optional) Name of the module containing the scenarios to be registered.',
    ),
    'examples' => array(
      'Discover and register' => 'drush brm',
      'Register specific module' => 'drush brm behatrunner',
    ),
    'aliases' => array('brm'),
  );
  $items['behat-run-tests'] = array(
    'description' => dt('Run the behat tests.'),
    'options' => array(
      'formats' => array(
        'description' => 'The output formats',
        'example_value' => 'junit',
      ),
    ),
    'examples' => array(
      'Standard example' => 'drush behat-run-tests',
    ),
    'aliases' => array('brun'),
  );
  return $items;
}

/**
 * List known test locations.
 */
function drush_behatrunner_behat_list_scenarios() {
  $scenarios = entity_load('behatrunner_scenario');

  if ($scenarios) {
    $table = array();
    $table[] = array(t('Title'), t('Module'), t('Location'));
    $table[] = array('-----------', '-----------', '-----------');
    foreach ($scenarios as $scenario) {
      $table[] = array(
        $scenario->title,
        $scenario->module,
        $scenario->location);
    }
    drush_print_table($table);
  }
  else {
    drush_print(t('There are no test locations registered.'));
  }
}

/**
 * Register a module's scenarios.
 *
 * @param string $module
 *   The name of the module to register.
 */
function drush_behatrunner_behat_register_modules($module = NULL) {
  module_load_include('inc', 'behatrunner', 'includes/behatrunner');

  if (!$module) {
    behatrunner_discover_modules();
  }
  else {
    behatrunner_register_module($module);
  }
}


/**
 * Run behat tests.
 */
function drush_behatrunner_behat_run_tests() {
  module_load_include('inc', 'behatrunner', 'includes/behatrunner');

  // If there are no registrations, run auto discovery.
  if (!behatrunner_scenarios_exist()) {
    behatrunner_discover_modules();
  }

  $formats = array();
  if ($formats_option = drush_get_option('formats', 'pretty,junit')) {
    $formats = explode(',', $formats_option);
  }

  // TODO: Accept parameters.
  module_load_include('inc', 'behatrunner', 'includes/behatrunner');
  $results = behatrunner_execute_tests(array(), array(), $formats);
}
