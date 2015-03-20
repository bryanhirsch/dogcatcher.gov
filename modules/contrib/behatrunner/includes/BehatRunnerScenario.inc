<?php

/**
 * @file
 * behatrunner_test_location entity code.
 */

/**
 * BehatRunnerScenario class.
 */
class BehatRunnerScenario extends Entity {

  /**
   * The default label.
   *
   * @return int
   *   behatrunner_test_location btid
   */
  protected function defaultLabel() {
    return $this->title;
  }

  /**
   * The default URI.
   *
   * @return array
   *   An array containing the path to the behatrunner_test_location
   */
  protected function defaultUri() {
    return array('path' => 'behatrunner_test_location/' . $this->identifier());
  }
}


/**
 * BehatRunnerScenarioController class
 */
class BehatRunnerScenarioController extends EntityAPIController {

}
