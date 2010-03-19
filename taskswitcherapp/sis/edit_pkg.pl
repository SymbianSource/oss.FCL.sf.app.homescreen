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
# Description: 
#
#Options:
# -version  -adds Qt version (4,6,0) intp pkg files
# -target  -replaces $(PLATFORM)/$(TARGET) into armv5/urel
# -module  -creates script for module tests sis
# -module  -delete module tests pkg
# default -version

use strict;
use File::Find;
my $base = `cd`;
$base =~ s/\\\w+\s+$//; #move one dir upper 
my $modulebase = `cd`;
$modulebase =~s/(\s+)$//; #remove linefeed from end
print $base;;
my $qtversion = "\, 4\, 6\, 0\,";
my $target = "armv5\/urel";

my $qtversionchnage = 0;
my $qtversionarg = "-version";
my $targetchnage = 0;
my $targetarg = "-target";
my $module = 0;
my $modulearg = "-module";
my $delmodule = 0;
my $delmodulearg = "-delmodule";
my $moduledir = "modulesis";
my $modulepath = $base."\\".$moduledir;
my $ccert = $modulebase."\\"."cert.cer";
my $ckey = $modulebase."\\"."key.pem";
my $modulebatpath = $modulebase."\\".$moduledir."\\createmodulesis.bat"; 

checkCmdLine();

if ( $qtversionchnage == 1 || $targetchnage == 1 )
	{
	find( \&edits, $base );
	}
if ( $module == 1 )
	{
	system "mkdir $moduledir";
	if ( not ( open (FWM, ">$modulebatpath") ) )
		{
		print "\n***Error: Couldn't open $modulebatpath file to write\n";
		return;
		}
	binmode FWM;
	print FWM "del /S/Q *.SIS\r\n";
	print FWM "del /S/Q *.SISX\r\n";
	close FWM;	
	find( \&moduleSis, $base );
	}	
if ( $delmodule == 1 )
	{
	find( \&delModuleSis, $base );
	}	

sub edits() 
	{
	my $file=$_;
	my $winpath=$File::Find::name;
	$winpath=~s/\//\\/g;
	if( -f $file  && $file=~/(_template\.pkg)$/i ) 
		{
		print "$winpath ";
		system "attrib -R $winpath";
		if ( not ( open (FR, $winpath) ) )
			{
			print "Error: Couldn't open $winpath file to read\n";
			return;
			}
		binmode FR;
		my $buf="";
		my $length = 0;
		$length = -s $winpath;
		read( FR, $buf, $length );
		close FR;
		if ( $qtversionchnage == 1 )
			{
			$buf =~s/\,\s*\,\s*\,\s*\,\s*\{\"Qt\"\}/$qtversion \{\"Qt\"\}/si;
			}
		if ( $targetchnage == 1 )
			{
			$buf =~s/\$\(PLATFORM\)\/\$\(TARGET\)/$target/si;
			}
	    
	    if ( not ( open (FW, ">$winpath") ) )
			{
			print "\n***Error: Couldn't open $winpath file to write\n";
			return;
			}
		binmode FW;		
		print FW $buf;
	    close FW;		
		
		print "\r\n";
		}
	}

sub checkCmdLine()
	{
	my $numArgs = $#ARGV + 1;
	if ( $numArgs == 0 )
		{
		$qtversionchnage = 1;
		}
	else
		{
		foreach my $argnum ( 0 .. $#ARGV ) 
			{
			my $argName = lc $ARGV[$argnum];
			if ( $argName eq $qtversionarg )
				{
				$qtversionchnage = 1;
				}
			elsif ( $argName eq $targetarg )
				{
				$targetchnage = 1;
				}
			elsif ( $argName eq $modulearg )
				{
				$module = 1;
				}
			elsif ( $argName eq $delmodulearg )
				{
				$delmodule = 1;
				}
			else
				{
				die "\n***Error: Bad arguments\n";
				}
			}
		}
	}
	
sub moduleSis()
	{
	my $file=$_;
	my $winpath=$File::Find::name;
	$winpath=~s/\//\\/g;	

	
	if( -f $file  && $file=~/(_template\.pkg)$/i && ( $file=~/^(t_)/i || $file=~/test/i ) ) 
		{
		print "$winpath ";
		system "attrib -R $winpath";
		if ( not ( open (FR, $winpath) ) )
			{
			print "Error: Couldn't open $winpath file to read\n";
			return;
			}
		binmode FR;
		my $buf="";
		my $length = 0;
		$length = -s $winpath;
		read( FR, $buf, $length );
		close FR;
		$buf =~s/\,\s*\,\s*\,\s*\,\s*\{\"Qt\"\}/$qtversion \{\"Qt\"\}/si;
		$buf =~s/\$\(PLATFORM\)\/\$\(TARGET\)/$target/si;
	    
	    if ( not ( open (FW, ">$winpath") ) )
			{
			print "Error: Couldn't open $winpath file to write\n";
			return;
			}
		binmode FW;		
		print FW $buf;
	    close FW;				
		print "\r\n";
		
		my $pkgname = sisName( $winpath );
		
		if ( not ( open (FWM2, ">>$modulebatpath") ) )
			{			
			print "Error: Couldn't open $modulebatpath file to append\n";
			return;
			}
		binmode FWM2;		
		print FWM2 "makesis $winpath $pkgname\.SIS\r\n";
		print FWM2 "signsis $pkgname\.SIS $pkgname\.SISX $ccert $ckey\r\n";
		close FWM2;	
		}	
	}
		
sub sisName()
	{
	my $path = shift;
	my $name = "";
	if ( $path =~ /\\(\w+)\.pkg/i )
		{
		$name = $1;
		}
	return $name;
	}
	
sub delModuleSis()
	{
	my $file=$_;
	my $winpath=$File::Find::name;
	$winpath=~s/\//\\/g;	

	
	if( -f $file  && $file=~/(_template\.pkg)$/i && ( $file=~/^(t_)/i || $file=~/test/i ) ) 
		{
		print "Deleting: ";
		print "$winpath ";
		system "attrib -R $winpath";
		system "del /S/Q $winpath";
		print "\r\n";
		}	
	}	