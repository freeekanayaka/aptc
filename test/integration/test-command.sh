#!/bin/sh
# file: test-install.sh

APTC="../../build/cmd/aptc --rootfs=tmp --option=Debug::aptc=true"

setUp() {
    ./fixture-repo.sh setup
    mkdir tmp
}

tearDown() {
    rm -r tmp
    ./fixture-repo.sh teardown
}

testInitDebian() {
    aptc init sid
    sourcesList=tmp/etc/apt/sources.list
    assertTrue "no sources file exists" "[ -f $sourcesList ]"
    assertTrue "sources.list deb release not sid" "grep -q sid $sourcesList"
    assertTrue "sources.list mirror not Debian" "grep -q ftp.debian.org $sourcesList"
}

testInitUbuntu() {
    aptc init bionic
    sourcesList=tmp/etc/apt/sources.list
    assertTrue "no sources file exists" "[ -f $sourcesList ]"
    assertTrue "sources.list deb release not bionic" "grep -q bionic $sourcesList"
    assertTrue "sources.list mirror not Ubuntu" "grep -q archive.ubuntu.com $sourcesList"
}

testInstall() {
    aptc init sid "file://$(pwd)"
    aptc update --option="Dir::Etc::trusted=./gpg/trusted.gpg"
    aptc install test
    assertTrue "no test package installed" "[ -f tmp/usr/share/doc/test/copyright ]"
    assertTrue "test package not in dpkg status" "dpkg --root=tmp -l test | grep -q 0.1-1"
}

aptc() {
    if ! $APTC "$@" > aptc.log 2>&1; then
	fail "aptc failed"
	cat aptc.log
    fi
    rm aptc.log
}

# shellcheck disable=SC1091
. /usr/bin/shunit2
