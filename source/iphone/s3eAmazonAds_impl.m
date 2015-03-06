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

#include "s3eAmazonAds_impl.h"
#include "s3eAmazonAd.h"
#include "s3eAmazonAds_platform_extras.h"
#import <AmazonAd/AmazonAdError.h>
#import <AmazonAd/AmazonAdRegistration.h>

static S3eAmazonAdSngl* singleton = nil;

@implementation S3eAmazonAdSngl

+(id) createSingleton:(const char*)appkey enableTesting:(BOOL)enableTesting enableLogging:(BOOL)enableLogging
{
    if (singleton == nil)
    {
        singleton = [[S3eAmazonAdSngl alloc] init:s3eEdkGetUIViewController() appkey:appkey enableTesting:enableTesting enableLogging:enableLogging];
    }
    return singleton;
}

+(id) theSingleton
{
    return singleton;
}

+(void) releaseSingleton
{
    if (singleton != nil)
    {
        [singleton release];
        singleton = nil;
    }
}

-(id) init: (UIViewController*) viewController appkey:(const char*)appkey enableTesting:(BOOL)enableTesting enableLogging:(BOOL)enableLogging
{
    if (!(self = [super init]))
        return nil;
    self->m_Array = [[NSMutableArray alloc] init];
    self->m_ViewController = viewController;
    self->m_AppKey = [[NSString alloc] initWithUTF8String:appkey];
    [self->m_AppKey autorelease];
    self->m_Testing = enableTesting;
    [[AmazonAdRegistration sharedRegistration] setAppKey:self->m_AppKey];
    [[AmazonAdRegistration sharedRegistration] setLogging:enableLogging];
    return self;
}

-(void) dealloc
{
    for (id element in m_Array)
    {
        [element release];
    }
    [m_Array release];
    [m_AppKey release];
    [super dealloc];
}

-(UIViewController*) viewController
{
    return m_ViewController;
}


-(UIViewController*) viewControllerToUse
{
    return m_ViewController;
}

-(BOOL) areTesting
{
    return m_Testing;
}

-(BOOL) checkId: (id)addId
{
    // return false if the suppliedId is nil or not in m_Array
    if (!addId)
        return NO;
    return [m_Array containsObject:addId];
}

-(id) prepareAd
{
    S3EAmazonAd* newAd = [[S3EAmazonAd alloc] init:self];
    [m_Array addObject:newAd];
    return newAd;
}

-(BOOL) prepareAdLayout:(id)addId position:(s3eAmazonAdsPosition)position size:(s3eAmazonAdsSize)size
                width:(int)width height:(int)height
{
    if (![self checkId:addId])
        return NO;
    
    S3EAmazonAd* ad = addId; // treat as pointer
    
    return [ad prepareAdLayout:position size:size width:width height:height];
}

-(BOOL) setAdTargetingOptions:(id)addId options:(s3eAmazonAdsTargetingOptions*)options
{
    if (![self checkId:addId])
        return NO;
    
    S3EAmazonAd* ad = addId; // treat as pointer
    
    return [ad setAdTargetingOptions:options];
}

-(s3eAmazonAdsTargetingOptions*) getAdTargetingOptions:(id)addId
{
    if (![self checkId:addId])
        return nil;
    
    S3EAmazonAd* ad = addId; // treat as pointer
    
    return [ad getAdTargetingOptions];
}

-(BOOL) loadAd:(id)addId show:(BOOL)show timeout:(int)timeout
{
    if (![self checkId:addId])
        return NO;
    
    S3EAmazonAd* ad = addId; // treat as pointer
    
    return [ad loadAd:show timeout:timeout];
}

-(BOOL) destroyAd:(id)addId
{
    if (![self checkId:addId])
        return YES; // no harm
    
    S3EAmazonAd* ad = addId; // treat as pointer
    
    [ad release];
    [m_Array removeObject:ad];
    return YES;
}

-(BOOL) loadInterstitialAd:(id)addId
{
    if (![self checkId:addId])
        return NO;
    
    S3EAmazonAd* ad = addId; // treat as pointer
    
    return [ad loadInterstitialAd];
}


-(BOOL) showAd:(id)addId
{
    if (![self checkId:addId])
        return NO;
    
    S3EAmazonAd* ad = addId; // treat as pointer
    
    return [ad showAd];
}


-(BOOL) inspectAd:(id)addId info:(s3eAmazonAdsAdInfo*)info
{
    if (![self checkId:addId])
    {
        // TODO should we fail this with unknown add?
        info->m_IsLoading = false; // set to false
        return YES;
    }
    
    S3EAmazonAd* ad = addId; // treat as pointer
    
    return [ad inspectAd:info];
}

-(void) onLoaded:(S3EAmazonAd*)ad properties:(struct s3eAmazonAdsProperties*)properties
{
    // TODO change this so that the properties is passed to platform code and we dump m_LoadedData
    m_LoadedData.m_Id = (intptr_t) ad;
    m_LoadedData.m_Properties = properties;
    s3eAmazonAds_onLoad(&m_LoadedData);
}

-(void) onError:(S3EAmazonAd*)ad error:(AmazonAdError*)error
{
    s3eAmazonAds_onError(ad, error.errorCode);
}

-(void) onAction:(S3EAmazonAd*)ad action:(AmazonAdAction)action
{
    s3eAmazonAds_onAction(ad, action);
}

@end
