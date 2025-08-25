# cb-UnitTestPlugin
Code::Blocks Unit Test plugin for google test

## Description

Forked from UnitTestPlugin attached to https://forums.codeblocks.org/index.php/topic,18947.msg129777.html developed by code::blocks contributer koonschi

## Getting Started
### Getting the source
```
git clone https://github.com/josephch/cb-UnitTestPlugin
```
### Build using cmake ( Tested only on Linux)
Make sure codeblocks.pc is present in default package config search paths or export PKG_CONFIG_PATH with its location
```
mkdir cmake-build
cd cmake-build
cmake ../
make
```
### Build using Code::Blocks
Build after opening project in Code::Blocks

### Installation and Setup
Install the plugin by clicking ```Install New``` in ```Plugins->Manage Plugin``` menu and selecting UnitTest.cbplugin.  

## Acknowledgments
Thank you koonschi, original author of this plugin
