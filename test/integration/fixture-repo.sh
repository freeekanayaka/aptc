#!/bin/sh
#
# Fixture for creating a test APT repository.
#

if [ "${1}" = "teardown" ]; then
    rm -r pool dists
    rm -r gpg
    exit 0
fi

#
# Create a test package
#
echo "2.0" > debian-binary

mkdir -p usr/share/doc/test/
cat > usr/share/doc/test/copyright <<EOF
Copyright file
EOF

md5sum usr/share/doc/test/copyright > md5sums

cat > control <<EOF
Package: test
Source: test
Version: 0.1-1
Architecture: all
Maintainer: Free Ekanayaka <freee@debian.org>
Section: misc
Description: Test package
 Test package
EOF
tar cfz control.tar.gz control md5sums

tar cf data.tar usr
xz data.tar

ar qc test-0.1-1.deb debian-binary control.tar.gz data.tar.xz

rm -rf usr
rm debian-binary control md5sums control.tar.gz data.tar.xz

#
# Create the repository
#
mkdir -p pool/main/t/test/
mv test-0.1-1.deb pool/main/t/test/

mkdir -p dists/sid/main/binary-amd64
apt-ftparchive packages pool/  > dists/sid/main/binary-amd64/Packages

apt-ftparchive release dists/sid/ > dists/sid/Release

#
# Sign the repository
#
mkdir gpg
chmod 700 gpg
gpg --homedir gpg --import --armor > /dev/null 2>&1 <<EOF
-----BEGIN PGP PRIVATE KEY BLOCK-----

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
ApSXK+DhE512sMYDrw==
=HWf0
-----END PGP PRIVATE KEY BLOCK-----
EOF

gpg --homedir gpg/ --clearsign -o dists/sid/InRelease dists/sid/Release
gpg --homedir gpg/ -abs -o dists/sid/Release.gpg dists/sid/Release
gpg --homedir gpg/ --export > gpg/trusted.gpg
