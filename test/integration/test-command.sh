#!/bin/sh
# file: test-install.sh

APTC="../../build/cmd/aptc --rootfs=tmp"

setUp() {
    ./fixture-repo.sh setup
    mkdir tmp
}

tearDown() {
    rm -r tmp
    ./fixture-repo.sh teardown
}

testInitDebian() {
    $APTC init sid
    sourcesList=tmp/etc/apt/sources.list
    assertTrue "no sources file exists" "[ -f $sourcesList ]"
    assertTrue "sources.list deb release not sid" "grep -q sid $sourcesList"
    assertTrue "sources.list mirror not Debian" "grep -q ftp.debian.org $sourcesList"
}

testInitUbuntu() {
    $APTC init bionic
    sourcesList=tmp/etc/apt/sources.list
    assertTrue "no sources file exists" "[ -f $sourcesList ]"
    assertTrue "sources.list deb release not bionic" "grep -q bionic $sourcesList"
    assertTrue "sources.list mirror not Ubuntu" "grep -q archive.ubuntu.com $sourcesList"
}

testInstall() {
    $APTC init sid "file://$(pwd)"
    $APTC update --option="Dir::Etc::trusted=./gpg/trusted.gpg" > /dev/null
    $APTC install test > /dev/null
    assertTrue "no test package installed" "[ -f tmp/usr/share/doc/test/copyright ]"
    assertTrue "test package not in dpkg status" "dpkg --root=tmp -l test | grep -q 0.1-1"
}

# shellcheck disable=SC1091
. /usr/bin/shunit2
