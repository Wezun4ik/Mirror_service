# Mirror_service
Mirror is a service that mirrors vertically _.jpeg_ images. This is a test task.

# Prereqisites
* Ubuntu v18.04 (advised)
* Boost library
* Libjpeg library
* OpenCV library

You can install libraries, using commands:
* __sudo apt install libboost-dev__
* __sudo apt install libjpeg-dev__
* __sudo apt install libopencv-dev__

# Usage
1. Use ```Make``` to compile _mirror_ executable
2. Use ```./mirror&``` to run executable in background as a service
3. Use ```bash test.sh``` to run tests on _pics_ folder
4. Use ```curl``` on port 5875 to send arbitrary data
   * __Important note__: send data via ```--data-binary``` specifier
