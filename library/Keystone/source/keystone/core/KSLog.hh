// 
// $Id: KSLog.hh 57 2010-01-22 23:51:37Z brian $
// Keystone Framework
// 
// Copyright (c) 2008 Wolter Group New York, Inc., All rights reserved.
// 
// This software ("WGNY Software") is supplied to you by Wolter Group New York
// ("WGNY") in consideration of your acceptance of the terms of the Agreement
// under which the software was licensed to you. Your use or installation of
// this Software constitutes acceptance of these terms.  If you do not agree
// with these terms, do not use or install this software.
// 
// The WGNY Software is provided by WGNY on an "AS IS" basis.  WGNY MAKES NO
// WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
// WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE, REGARDING THE WGNY SOFTWARE OR ITS USE AND OPERATION
// ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
// 
// IN NO EVENT SHALL WGNY BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION
// AND/OR DISTRIBUTION OF THE WGNY SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER
// THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE), STRICT LIABILITY OR
// OTHERWISE, EVEN IF WGNY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// Designed and Engineered by Wolter Group in New York City
// 

#if !defined(__KEYSTONE_KSLOG__)
#define __KEYSTONE_KSLOG__ 1

#include <CoreFoundation/CoreFoundation.h>

#define KSFORMAT(c, f)  "\x1b\x5b " #f "m" c "\x1b\x5b m"
#define KSNORMAL(c)     c
#define KSBOLD(c)       KSFORMAT(c, 1)
#define KSITALIC(c)     KSFORMAT(c, 3)
#define KSULINE(c)      KSFORMAT(c, 4)
#define KSSTRIKE(c)     KSFORMAT(c, 9)
#define KSBOLDITALIC(c) KSFORMAT(c, 1;3)
#define KSBOLDULINE(c)  KSFORMAT(c, 1;4)

#define KSBLACK         KSFORMAT(c, 30)
#define KSRED           KSFORMAT(c, 31)
#define KSGREEN         KSFORMAT(c, 32)
#define KSYELLOW        KSFORMAT(c, 33)
#define KSBLUE          KSFORMAT(c, 34)
#define KSMAGENTA       KSFORMAT(c, 35)
#define KSCYAN          KSFORMAT(c, 36)
#define KSWHITE         KSFORMAT(c, 37)

#define KSBGBLACK       KSFORMAT(c, 40)
#define KSBGRED         KSFORMAT(c, 41)
#define KSBGGREEN       KSFORMAT(c, 42)
#define KSBGYELLOW      KSFORMAT(c, 43)
#define KSBGBLUE        KSFORMAT(c, 44)
#define KSBGMAGENTA     KSFORMAT(c, 45)
#define KSBGCYAN        KSFORMAT(c, 46)
#define KSBGWHITE       KSFORMAT(c, 47)

typedef void (*KSLogObserverCallBack)(const char *message);

KSLogObserverCallBack KSLogGetObserverCallBack(void);
void KSLogSetObserverCallBack(KSLogObserverCallBack observer);

#define KSLogError(f, a...)   KSLog(f, ##a)
#define KSLogErrorK(f, a...)  KSLogK(f, ##a)

void KSLog(const char *f, ...);
void KSLogK(const char *f, ...);

#endif __KEYSTONE_KSLOG__



