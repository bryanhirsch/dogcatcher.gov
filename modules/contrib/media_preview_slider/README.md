The objective of this module is to make identifying the correct thumbnail from a grid view simpler by providing the ability to scale the images.

How it works:

This module programs a default class for the view:  Media Browser. 

If you edit the view and go to "other" section and set a default 
"CSS class" this will be overridden at runtime.

This module also changes the default Media thumbnail imagecache 
preset to 300x300px

the JqueryUI slider will appear next to the exposed filters for changing of the image sizes by sliding. It also stores a LocalStorage variable so that the size is maintained next time you come back to the Media Browser. 
