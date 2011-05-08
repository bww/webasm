// 
// $Id: KSUtility.hh 85 2010-12-06 14:49:30Z brian $
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

#if !defined(__KEYSTONE_KSUTILITY__)
#define __KEYSTONE_KSUTILITY__ 1

#define KSMin(a, b)         (((a) < (b)) ? (a) : (b))
#define KSMax(a, b)         (((a) > (b)) ? (a) : (b))
#define KSAbs(a)            (((a) < 0) ? -(a) : (a))

#define CFRetainSafe(obj)   ((__typeof__(obj))((obj != NULL) ? CFRetain(obj) : NULL))
#define CFSafeRetain(obj)   CFRetainSafe(obj)
#define CFReleaseSafe(obj)  if(obj != NULL){ CFRelease(obj); obj = NULL; }
#define CFSafeRelease(obj)  CFReleaseSafe(obj)

#define KSFreeSafe(ptr)     if(ptr != NULL){ free(ptr); ptr = NULL; }

#endif __KEYSTONE_KSUTILITY__

