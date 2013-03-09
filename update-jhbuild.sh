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
param=""

run() {
    cmd=$1
    ${jhbuild} ${cmd} ${pkgs}
}

while getopts "clp:" opt; do
    case $opt in
	c)
	    run "clean"
	    ;;
	l)
	    run "list"
	    exit
	    ;;
	p)
	    param=$OPTARG
	    ;;
	\?)
	    echo "Invalid option: -$OPTARG" >&2
	    exit
	    ;;
    esac
done

run "build ${param} --nodeps"
