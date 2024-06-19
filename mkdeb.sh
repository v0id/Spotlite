#!/bin/sh
ARCH=`dpkg-architecture -qDEB_HOST_ARCH`

cd `dirname $0`



rm -rf debian
mkdir debian
cd debian

mkdir -p "usr/share/applications"

binreader="usr/share/applications/spotlite.desktop"
echo "[Desktop Entry]" > "$binreader"
echo "Version=1.0" >> "$binreader"
echo "Name=SpotLite" >> "$binreader"
echo "GenericName=SpotLite" >> "$binreader"
echo "Exec=SpotLite" >> "$binreader"
echo "Terminal=false" >> "$binreader"
echo "Type=Application" >> "$binreader"
echo "Categories=Network" >> "$binreader"
echo "Icon=spotlite.png" >> "$binreader"
echo "MimeType=application/x-spotskin;" >> "$binreader"

mkdir -p usr/share/mime/packages

cat <<EOF > usr/share/mime/packages/spotlite.xml
<?xml version="1.0" encoding="UTF-8"?>
<mime-info xmlns="http://www.freedesktop.org/standards/shared-mime-info">
  <mime-type type="application/x-spotskin">
    <comment>SpotLite skin</comment>
    <glob pattern="*.spotskin"/>
  </mime-type>
</mime-info>
EOF

mkdir -p DEBIAN

control_file="DEBIAN/control"

echo "Package: spotlite" > "$control_file"
echo "Version: 1.0" >> "$control_file"
echo "Architecture: $ARCH" >> "$control_file"
echo "Maintainer: Q." >> "$control_file"
echo "Installed-Size: 1140" >> "$control_file"
echo "Depends: libqt5core5a (>= 5.12.2), libqt5gui5 (>= 5.0.2) | libqt5gui5-gles (>= 5.0.2), libqt5network5 (>= 5.0.2), libqt5sql5 (>= 5.0.2), libqt5webkit5 (>= 5.212.0~alpha3), libqt5widgets5 (>= 5.12.2), libqt5xmlpatterns5 (>= 5.0.2), libssl1.1 (>= 1.1.1), libstdc++6 (>= 5), zlib1g (>= 1:1.1.4)" >> "$control_file"
echo "Description: Spotlite spotnet client" >> "$control_file"
echo " Spotlite spotnet client" >> "$control_file"

postinst_file="DEBIAN/postinst"

echo "#!/bin/sh" > "$postinst_file"
echo "" >> "$postinst_file"
echo "update-mime-database /usr/share/mime" >> "$postinst_file"
chmod 775 "$postinst_file"

postrm_file="DEBIAN/postrm"

echo "#!/bin/sh" > "$postrm_file"
echo "" >> "$postrm_file"
echo "update-mime-database /usr/share/mime" >> "$postrm_file"
chmod 775 "$postrm_file"


mkdir -p usr/bin
cp -a ../SpotLite usr/bin/SpotLite
strip usr/bin/SpotLite

cp -a ../SpotLite-skininstaller/SpotLite-skininstaller usr/bin/SpotLite-skininstaller
strip usr/bin/SpotLite-skininstaller

mkdir -p usr/local/share/SpotLite/skins/standaard
cp -a ../../SpotLite/skins/standaard/screenshot.png usr/local/share/SpotLite/skins/standaard/screenshot.png 

mkdir -p usr/local/share/SpotLite/skins/RoyaleBlueFull
cp -a ../../SpotLite/skins/RoyaleBlueFull/* usr/local/share/SpotLite/skins/RoyaleBlueFull

mkdir -p usr/share/pixmaps
cp -a ../../SpotLite/res/icon.png usr/share/pixmaps/spotlite.png

cd ..
dpkg-deb --build debian
mv debian.deb spotlite-$ARCH.deb

rm -rf debian
