<?php
// Plugin definition
$plugin = array(
  'title' => t('Full Sidebar'),
  'icon' => 'demonstratie-full-side-layout.png',
  'category' => t('Demonstratie'),
  'theme' => 'demonstratie_layout_full_side',
  'regions' => array(
    'primary_features' => t('Primary features'),
    'secondary_features' => t('Secondary features'),
    'sidebar' => t('Sidebar'),
    'main_content' => t('Main Content'),
  ),
);
