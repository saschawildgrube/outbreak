# outbreak
This is the source code of https://www.wildgrube.com/outbreak/

The root folder of the repo is the document root of the website.

## Installation:
1. Make sure to configure your webserver to obey all rules defined in .htaccess
2. Update the folders in .htaccess to match the directory structure on your webserver
3. Check out /_source/env.inc and adjust the following functions to match the directory structure on your webserver:
- GetDocumentRootDir()
- GetRootURL()
- GetTempDir() 
4. Please replace the Google Maps API key with your own in /_source/env.cfg

This website is based on SteelPHP: https://www.steelphp.com.

Enjoy!

## Licence
Copyright 2020-2021 by Sascha Wildgrube

Licensed under the Apache License, Version 2.0 (the "License");

You may not use ChecklistHQ except in compliance with the License.

You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
