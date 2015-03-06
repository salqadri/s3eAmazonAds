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
 
 /*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */

/*
 * Internal header for the s3eAmazonAds extension.
 *
 * This file should be used for any common function definitions etc that need to
 * be shared between the platform-dependent and platform-indepdendent parts of
 * this extension.
 */

#ifndef S3EAMAZONADS_INTERNAL_H
#define S3EAMAZONADS_INTERNAL_H

#include "s3eTypes.h"
#include "s3eAmazonAds.h"
#include "s3eAmazonAds_autodefs.h"

void s3eAmazonAdsSetErrorString(s3eAmazonAdsError err, const char* str);

s3eResult s3eAmazonAdsInit_platform(const char* appKey, s3eBool enableTesting, s3eBool enableLogging);

s3eResult s3eAmazonAdsTerminate_platform();

s3eResult s3eAmazonAdsPrepareAd_platform(s3eAmazonAdsId* pId);

s3eResult s3eAmazonAdsPrepareAdLayout_platform(s3eAmazonAdsId id, s3eAmazonAdsPosition position, s3eAmazonAdsSize size, int width, int height);

s3eResult s3eAmazonAdsSetAdTargetingOptions_platform(s3eAmazonAdsId id, s3eAmazonAdsTargetingOptions* options);

s3eAmazonAdsTargetingOptions* s3eAmazonAdsGetAdTargetingOptions_platform(s3eAmazonAdsId id);

s3eResult s3eAmazonAdsLoadAd_platform(s3eAmazonAdsId id, s3eBool show, int timeout);

s3eResult s3eAmazonAdsDestroyAd_platform(s3eAmazonAdsId id);

s3eResult s3eAmazonAdsCollapseAd_platform(s3eAmazonAdsId id);

s3eResult s3eAmazonAdsLoadInterstitialAd_platform(s3eAmazonAdsId id);

s3eResult s3eAmazonAdsShowAd_platform(s3eAmazonAdsId id);

s3eResult s3eAmazonAdsInspectAd_platform(s3eAmazonAdsId id, s3eAmazonAdsAdInfo* info);


#endif /* !S3EAMAZONADS_INTERNAL_H */
