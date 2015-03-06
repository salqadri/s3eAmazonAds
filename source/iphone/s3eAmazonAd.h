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

#ifndef S3EAMAZONAD_H
#define S3EAMAZONAD_H

#include <objc/NSObject.h>
#import <UIKit/UIKit.h>
#import <AmazonAd/AmazonAdView.h>
#import <AmazonAd/AmazonAdInterstitial.h>
#include "s3eAmazonAds.h"
#include "s3eAmazonAds_decs.h"

// This class acts as a wrapper to the Amazon code. One object per ad instance.

@class S3eAmazonAdSngl;
@class UIViewController;

@interface S3EAmazonAd : NSObject <AmazonAdViewDelegate, AmazonAdInterstitialDelegate>
{
    S3eAmazonAdSngl* m_Parent;
    AmazonAdView* m_AdView;
    AmazonAdInterstitial* m_AdInterstitial;
    BOOL m_Loading;
    BOOL m_ShowOnLoad;
    
    BOOL m_LayoutSet;
    s3eAmazonAdsPosition m_Position;
    s3eAmazonAdsSize m_Size;
    int m_Width;
    int m_Height;
    
    s3eAmazonAdsTargetingOptions* m_Options; // not owned
    
    struct s3eAmazonAdsProperties m_LoadedProperties;
}

-(id) init:(S3eAmazonAdSngl*) parent;
-(void) dealloc;

-(BOOL) prepareAdLayout:(s3eAmazonAdsPosition)position size:(s3eAmazonAdsSize)size
                  width:(int)width height:(int)height;

-(BOOL) setAdTargetingOptions:(s3eAmazonAdsTargetingOptions*)options;
-(s3eAmazonAdsTargetingOptions*) getAdTargetingOptions;
-(BOOL) loadAd:(BOOL)show timeout:(int)timeout;
-(BOOL) loadInterstitialAd;
-(BOOL) showAd;
-(BOOL) inspectAd:(s3eAmazonAdsAdInfo*) info;

@end

#endif
