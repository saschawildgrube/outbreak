# outbreak
This is the source code of https://www.wildgrube.com/outbreak/

The root folder of the repo is the document root of the website.

Installation guide:
1. Make sure to configure your webserver to obey all rules defined in .htaccess
2. Update the folders in .htaccess to match the directory structure on your webserver
3. Check out /_source/env.inc and adjust the following functions to match the directory structure on your webserver:
- GetDocumentRootDir()
- GetRootURL()
- GetTempDir() 
4. Please replace the Google Maps API key with your own in /_source/website_outbreak.inc

This website is based on SteepPHP: https://www.steelphp.com.

Enjoy!
