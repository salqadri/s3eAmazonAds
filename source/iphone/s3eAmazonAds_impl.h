/*
 * (C) 2001-2012 Marmalade. All Rights Reserved.
 *
 * This document is protected by copyright, and contains information
 * proprietary to Marmalade.
 *
 * This file consists of source code released by Marmalade under
 * the terms of the accompanying End User License Agreement (EULA).
 * Please do not use this program/source code before you have read the
 * EULA and have agreed to be bound by its terms.
 */

#ifndef S3EAMAZONADS_IMPL_H
#define S3EAMAZONADS_IMPL_H

#include <objc/NSObject.h>
#import <Foundation/NSArray.h>
#include "s3eEdk_iphone.h"
#include "s3eAmazonAds.h"
#include "s3eAmazonAds_decs.h"

@class UIViewController;
@class S3EAmazonAd;
@class AmazonAdError;

@interface S3eAmazonAdSngl : NSObject
{
    NSMutableArray* m_Array;
    UIViewController* m_ViewController;
    NSString* m_AppKey;
    BOOL m_Testing;
    
    struct s3eAmazonAdsCallbackLoadedData m_LoadedData;
}

+ (id) createSingleton:(const char*)appkey enableTesting:(BOOL)enableTesting enableLogging:(BOOL)enableLogging;
+ (id) theSingleton;
+ (void) releaseSingleton;

- (id) init: (UIViewController*)controller appkey:(const char*)appkey enableTesting:(BOOL)enableTesting enableLogging:(BOOL)enableLogging;
- (void) dealloc;

// calls from children
- (UIViewController*) viewControllerToUse;
- (BOOL) areTesting;
- (void) onLoaded:(S3EAmazonAd*)ad properties:(struct s3eAmazonAdsProperties*)properties;
- (void) onError:(S3EAmazonAd*)ad error:(AmazonAdError*)error;
- (void) onAction:(S3EAmazonAd*)ad action:(AmazonAdAction)action;

// calls from wrapper
- (id) prepareAd;
- (BOOL) prepareAdLayout:(id)addId position:(s3eAmazonAdsPosition)position size:(s3eAmazonAdsSize)size
                  width:(int)width height:(int)height;
- (BOOL) setAdTargetingOptions:(id)addId options:(s3eAmazonAdsTargetingOptions*)options;
- (s3eAmazonAdsTargetingOptions*) getAdTargetingOptions:(id)addId;
- (BOOL) loadAd:(id)addId show:(BOOL)show timeout:(int)timeout;
- (BOOL) destroyAd:(id)addId;
- (BOOL) loadInterstitialAd:(id)addId;
- (BOOL) showAd:(id)addId;
- (BOOL) inspectAd:(id)addId info:(s3eAmazonAdsAdInfo*)info;

@end

#endif
