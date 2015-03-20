Installation
================================================================================

*This module must be enabled via drush.*

1. drush en -y behatrunner
2. drush brun

FAQS
================================================================================

How do I run javascript tests?
--------------------------------------------------------------------------------

To run javascript tests:

1. All javascript scenarios must be tagged with @javascript
2. You must have a javascript-enabled web session available.

By default, behatrunner is set to run all javascript tests with
Selenium2. This is dictated by the javascript_session key in public://behat.yml.
Therefore, a Selenium2 server must be running when the test is executed.

@see "How do I install Selenium2" for installation instructions.
@see http://docs.seleniumhq.org/download

How do I install Selenium2?
--------------------------------------------------------------------------------

1. Download Selenium2 standalone server:
   https://code.google.com/p/selenium/downloads/detail?name=selenium-server-standalone-2.39.0.jar&can=1&q=
2. Start the selenium server locally:
   java -jar path/to/selenium-server-standalone-2.39.0.jar

When I run tests, the output says "No scenarios, No steps"
--------------------------------------------------------------------------------

You've probably change a scenario since the last time that Behat Runner
registered it. To update Behat Runner's registration, run `drush brm`.

How do I integrate this module with a continuous integration solution?
--------------------------------------------------------------------------------

Behat Runner writes the results of every test run to public://behatrunner. You
can configure your Jenkins or Travis CI jobs to look in this directory for the
JUnit-formatted XML output.
