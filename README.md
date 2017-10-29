aptc [![Build Status](https://circleci.com/gh/freeekanayaka/aptc.png)](https://circleci.com/gh/freeekanayaka/aptc)
====

aptc is a commandline package manager for installing deb packages on
an arbitrary filesystem tree, without requiring root privileges.  It
works exactly like regular APT, but it's meant to be used to create
static container images rather than regular full-fledged systems. In
fact it will not run any maintainer script shipped with the packages
being installed.
