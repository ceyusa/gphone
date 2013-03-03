#!/bin/sh

jhbuild=`which jhbuild`
pkgs="\
   gst-plugins-base \
   gst-plugins-good \
   gst-plugins-bad \
   vala \
   libnotify \
   libcanberra \
   gnome-icon-theme-symbolic \
   opal"

${jhbuild} clean ${pkgs}
${jhbuild} build -fac --nodeps ${pkgs}
