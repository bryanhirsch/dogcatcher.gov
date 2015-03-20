<?php

/**
 * @file
 * Administrative forms for Behat Runner.
 */

/**
 * Returns the administrative configure form for Behat Runner.
 */
function behatrunner_admin_form($form_state) {
  $form = array();

  $form['behatrunner_log'] = array(
    '#type' => 'select',
    '#title' => t('Log destination'),
    '#description' => t('Outputting to the Drupal database allows tests results to be viewed via Views.'),
    '#options' => array(
      'filesystem_database' => t('Drupal database and file system'),
      'filesystem' => t('File system'),
    ),
    '#default_value' => variable_get('behatrunner_log', 'filesystem_database'),
    '#required' => TRUE,
  );

  return system_settings_form($form);
}
