
## ------------------------------------------------------------------------------------
## For Developers (IMPORTANT: requires python3)
##
## 1: Enable this code to generate the files.
## 2: copy them into unity/app/ or unity/app/ directory
## 3: edit unity.pri and patch the correct path to the unity_*.cpp files
## 4: Open all unity_*.cpp and replace "src/" to "../../src/" to path to the correct paths.

##UNITY_BUILD = 1
##UNITY_MOC_MODE = MOC_LVL_1
##include(../../include/qmakeUnity/qmake_unity.pri)

## ------------------------------------------------------------------------------------

contains(USE_UNITY_BUILD, 1) {
	include(../../unity/$${DIGITALNOTE_APP_NAME}/unity.pri)
}