<?php
/**
 * Set memory limit.
 */
ini_set('memory_limit', '256M');

if (file_exists('/var/www/site-php')) {
  require '/var/www/site-php/az1/az1-settings.inc';
}
