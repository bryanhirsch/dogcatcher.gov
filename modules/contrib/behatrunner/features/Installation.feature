Feature: Site Installation

  Scenario: Installation
    Given I am on "/install.php"
    Then I should see "Drupal already installed"
