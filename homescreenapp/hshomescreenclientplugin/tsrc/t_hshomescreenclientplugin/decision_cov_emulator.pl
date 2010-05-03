#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: Script for running branch coverage test
#

#USAGE:
#    Usage: decision_cov.pl [-make=command]
#    -make=instrument || run || ctcpost

# import module
use Getopt::Long;

# read options
my $make = "all";
my $success = GetOptions ("make=s"   => \$make);

if($make) {
    if (($make eq "instrument") || ($make eq "all")) {
        system("del mon.sym");
        system("del mon.dat");
        system("del profile.txt");
        system("rmdir /s /q CTCFUNCTION");
        system("rmdir /s /q CTCDECISION");
		system("call qmake -config coverage");
		system("call sbs -c winscw_udeb.test");
		system("call sbs -c winscw_udeb.test clean");
		system("call sbs -c winscw_udeb clean");
		system("call sbs -c winscw_udeb");
		system("call ctcwrap -i fd -2comp -no-conf-check -C EXCLUDE=* -C NO_EXCLUDE=./../../src/* sbs -c winscw_udeb.test");
	}
    if (($make eq "run") || ($make eq "all")) {
		system("call \\epoc32\\release\\winscw\\udeb\\t_hshomescreenclientplugin.exe -o c:\\t_hshomescreenclientplugin.log");
	}
	if (($make eq "ctcpost") || ($make eq "all")) {
		system("call ctcpost -ff -p profile.txt");
		system("call ctc2html -nsb -i profile.txt -o CTCFUNCTION");
		system("call del profile.txt");
		system("call ctcpost -fd -p profile.txt");
		system("call ctc2html -nsb -i profile.txt -o CTCDECISION");
		system("call del profile.txt");
	}
}	
print "END.\n";
