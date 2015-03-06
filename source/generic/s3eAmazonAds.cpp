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
Generic implementation of the s3eAmazonAds extension.
This file should perform any platform-indepedentent functionality
(e.g. error checking) before calling platform-dependent implementations.
*/

#include "s3eAmazonAds_internal.h"
#include <string>

namespace
{
std::string g_ErrorString;
s3eAmazonAdsError g_Error = S3E_AMAZONADS_ERR_NONE;
}

void s3eAmazonAdsSetErrorString(s3eAmazonAdsError err, const char* str)
{
    g_ErrorString = str == NULL ? std::string("") : std::string(str);
    g_Error = err;
}

const char* s3eAmazonAdsGetErrorString()
{
    return g_ErrorString.c_str();
}

s3eAmazonAdsError s3eAmazonAdsGetError()
{
    return g_Error;

}
s3eResult s3eAmazonAdsInit(const char* appKey, s3eBool enableTesting, s3eBool enableLogging)
{
    return s3eAmazonAdsInit_platform(appKey, enableTesting, enableLogging);
}

s3eResult s3eAmazonAdsTerminate()
{
    return s3eAmazonAdsTerminate_platform();
}

s3eResult s3eAmazonAdsPrepareAd(s3eAmazonAdsId* pId)
{
    return s3eAmazonAdsPrepareAd_platform(pId);
}

s3eResult s3eAmazonAdsPrepareAdLayout(s3eAmazonAdsId id, s3eAmazonAdsPosition position, s3eAmazonAdsSize size, int width, int height)
{
    return s3eAmazonAdsPrepareAdLayout_platform(id, position, size, width, height);
}

s3eResult s3eAmazonAdsSetAdTargetingOptions(s3eAmazonAdsId id, s3eAmazonAdsTargetingOptions* options)
{
    return s3eAmazonAdsSetAdTargetingOptions_platform(id, options);
}

s3eAmazonAdsTargetingOptions* s3eAmazonAdsGetAdTargetingOptions(s3eAmazonAdsId id)
{
    return s3eAmazonAdsGetAdTargetingOptions_platform(id);
}

s3eResult s3eAmazonAdsLoadAd(s3eAmazonAdsId id, s3eBool show, int timeout)
{
    return s3eAmazonAdsLoadAd_platform(id, show, timeout);
}

s3eResult s3eAmazonAdsDestroyAd(s3eAmazonAdsId id)
{
    return s3eAmazonAdsDestroyAd_platform(id);
}

s3eResult s3eAmazonAdsCollapseAd(s3eAmazonAdsId id)
{
    return s3eAmazonAdsCollapseAd_platform(id);
}

s3eResult s3eAmazonAdsLoadInterstitialAd(s3eAmazonAdsId id)
{
    return s3eAmazonAdsLoadInterstitialAd_platform(id);
}

s3eResult s3eAmazonAdsShowAd(s3eAmazonAdsId id)
{
    return s3eAmazonAdsShowAd_platform(id);
}

s3eResult s3eAmazonAdsInspectAd(s3eAmazonAdsId id, s3eAmazonAdsAdInfo* info)
{
    return s3eAmazonAdsInspectAd_platform(id, info);
}
