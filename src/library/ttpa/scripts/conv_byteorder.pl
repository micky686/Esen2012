#!/usr/bin/perl
#
# $Id: conv_byteorder.pl,v 1.2 2007-01-16 16:52:23 bernd Exp $
#
#  Copyright (c) 2004, Christian Trödhandl
#  All rights reserved.
# 
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
# 
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
#  * Neither the name of the copyright holders nor the names of
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.


while(<>)
{
    $line = $_;
    if($line =~ /\.section/) {
	print ";;;;; found section\n";
	if($line =~ /\.section[^.]\.ifs_/)  {
	    print ";;;;;; found start\n";
	    $convstat = 1;
	} else {
	     print ";;;;;; found end\n";
	    $convstat = 0;
	}
    }
    if($line =~ /\.data/) {
	print ";;;;;; found end\n";
	    $convstat = 0;
    }
    if($convstat == 1) {
	if($line =~ /^([^.]*)(\.word)([^-0-9]*)([-]?[0-9]*)/ ) {
	    $x = $4;
	    $sep = $3;
	    $outline = "$1.byte 0x" . join("\n$1.byte 0x", map { sprintf "%02x", $_ } unpack("C*", pack("n", $x))) . "\n";
	} elsif ($line =~ /^([^.]*)(\.long)([^-0-9]*)([-]?[0-9]*)/ ) {
	    $x = $4;
	    $sep = $3;
	    $outline = "$1.byte 0x" . join("\n$1.byte 0x", map { sprintf "%02x", $_ } unpack("C*", pack("N", $x))). "\n";
	} else {
	    $outline = $line;
	}
    } else {
	$outline = $line;
    }    
    print $outline;
} 
