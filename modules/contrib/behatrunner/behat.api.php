<?php

/**
 * @file
 * API Implementation examples for Behat Runner.
 */

/**
 * Provides information about implementing module's behat features.
 *
 * The provided features path will be scanned for .feauture files, which will
 * be registered by Behat Runner.
 */
function hook_behat_info() {
  $info = array(
    'paths' => array(
      // Specify the directory in which .feature files are located, relative
      // to the module directory.
      'features' => 'features',
    ),
  );

  return $info;
}

/**
 * Allows behat info to be altered.
 */
function hook_behat_info_alter(&$alter) {
  $info['mymodule']['paths']['features'] = '/some/new/path';

  return $info;
}
