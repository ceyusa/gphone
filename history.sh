#!/bin/sh

SQL=`which sqlite3` || exit
HFILE="$HOME/.local/share/gphone/history.db"

echo $SQL
$SQL $HFILE "select * from history"
