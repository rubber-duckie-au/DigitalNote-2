##
## Qmake doesnt use cc and c++ compiler names as a default on ubuntu.
## Instead it uses gcc if detected. With cc and c++ names you can eazily switch compilers.
##
## Example usage:
##     sudo update-alternatives --config cc
##     sudo update-alternatives --config c++
##

contains(USE_UPDATE_ALTERNATIVES, 1) {
	QMAKE_CC = cc
	QMAKE_CXX = c++
}
