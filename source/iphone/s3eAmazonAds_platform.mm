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
 * iphone-specific implementation of the s3eAmazonAds extension.
 * Add any platform-specific functionality here.
 */
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
#include "s3eAmazonAds_internal.h"
#include "s3eAmazonAds_impl.h"
#include "s3eEdk.h"
#include "s3eEdk_iphone.h"
#import <AmazonAd/AmazonAdError.h>
#include "IwDebug.h"

const char* s3eAmazonAdsGetErrorString_platform()
{
    return NULL;
}

s3eAmazonAdsError s3eAmazonAdsGetError_platform()
{
    return S3E_AMAZONADS_ERR_NONE;
}

static s3eResult s3eAmazonAdsInit_real(const char* appKey, s3eBool enableTesting, s3eBool enableLogging)
{
    S3eAmazonAdSngl* singleton = [S3eAmazonAdSngl createSingleton:appKey enableTesting:enableTesting enableLogging:enableLogging];
    // check to see whether it has been allocated
    if (singleton != nil)
        return S3E_RESULT_SUCCESS;
    return S3E_RESULT_ERROR;
}

s3eResult s3eAmazonAdsInit_platform(const char* appKey, s3eBool enableTesting, s3eBool enableLogging)
{
    return (s3eResult)(intptr_t)s3eEdkThreadRunOnOS((s3eEdkThreadFunc)s3eAmazonAdsInit_real, 3, appKey, enableTesting, enableLogging);
}

s3eResult s3eAmazonAdsTerminate_platform() // run_on_os_thread
{
    [S3eAmazonAdSngl releaseSingleton];
    return S3E_RESULT_SUCCESS;
}

static s3eResult s3eAmazonAdsPrepareAd_real(s3eAmazonAdsId* pId)
{
    S3eAmazonAdSngl* singleton = [S3eAmazonAdSngl theSingleton];
    id preparedId = [singleton prepareAd];
    if (preparedId)
    {
        *pId = (s3eAmazonAdsId) preparedId; // treat pointer as id
        return S3E_RESULT_SUCCESS;
    }
    return S3E_RESULT_ERROR;
}

s3eResult s3eAmazonAdsPrepareAd_platform(s3eAmazonAdsId* pId)
{
    return (s3eResult)(intptr_t)s3eEdkThreadRunOnOS((s3eEdkThreadFunc)s3eAmazonAdsPrepareAd_real, 1, pId);
}

static s3eResult s3eAmazonAdsPrepareAdLayout_real(s3eAmazonAdsId adId, s3eAmazonAdsPosition position, s3eAmazonAdsSize size, int width, int height)
{
    S3eAmazonAdSngl* singleton = [S3eAmazonAdSngl theSingleton];
    BOOL ok = [singleton prepareAdLayout:(id)adId position:position size:size width:width height:height];
    return ok ? S3E_RESULT_SUCCESS : S3E_RESULT_ERROR;
}

s3eResult s3eAmazonAdsPrepareAdLayout_platform(s3eAmazonAdsId adId, s3eAmazonAdsPosition position, s3eAmazonAdsSize size, int width, int height)
{
    return (s3eResult)(intptr_t)s3eEdkThreadRunOnOS((s3eEdkThreadFunc)s3eAmazonAdsPrepareAdLayout_real, 5, adId, position, size, width, height);
}

static s3eResult s3eAmazonAdsSetAdTargetingOptions_real(s3eAmazonAdsId adId, s3eAmazonAdsTargetingOptions* options)
{
    S3eAmazonAdSngl* singleton = [S3eAmazonAdSngl theSingleton];
    BOOL ok = [singleton setAdTargetingOptions:(id)adId options:options];
    return ok ? S3E_RESULT_SUCCESS : S3E_RESULT_ERROR;
}

s3eResult s3eAmazonAdsSetAdTargetingOptions_platform(s3eAmazonAdsId adId, s3eAmazonAdsTargetingOptions* options)
{
    return (s3eResult)(intptr_t)s3eEdkThreadRunOnOS((s3eEdkThreadFunc)s3eAmazonAdsSetAdTargetingOptions_real, 2, adId, options);
}

static s3eAmazonAdsTargetingOptions* s3eAmazonAdsGetAdTargetingOptions_real(s3eAmazonAdsId adId)
{
    S3eAmazonAdSngl* singleton = [S3eAmazonAdSngl theSingleton];
    return[singleton getAdTargetingOptions:(id)adId];
}

s3eAmazonAdsTargetingOptions* s3eAmazonAdsGetAdTargetingOptions_platform(s3eAmazonAdsId adId)
{
    return (s3eAmazonAdsTargetingOptions*)s3eEdkThreadRunOnOS((s3eEdkThreadFunc)s3eAmazonAdsGetAdTargetingOptions_real, 1, adId);
}

s3eResult s3eAmazonAdsLoadAd_platform(s3eAmazonAdsId adId, s3eBool show, int timeout) // run_on_os_thread
{
    S3eAmazonAdSngl* singleton = [S3eAmazonAdSngl theSingleton];
    BOOL ok = [singleton loadAd:(id)adId show:show timeout:timeout];
    return ok ? S3E_RESULT_SUCCESS : S3E_RESULT_ERROR;
}

s3eResult s3eAmazonAdsDestroyAd_platform(s3eAmazonAdsId adId) // run_on_os_thread
{
    S3eAmazonAdSngl* singleton = [S3eAmazonAdSngl theSingleton];
    BOOL ok = [singleton destroyAd:(id)adId];
    return ok ? S3E_RESULT_SUCCESS : S3E_RESULT_ERROR;
}

s3eResult s3eAmazonAdsCollapseAd_platform(s3eAmazonAdsId id) // run_on_os_thread
{
    // AmazonAdView has no call to force a collapse, so return an error
    return S3E_RESULT_ERROR;
}

s3eResult s3eAmazonAdsLoadInterstitialAd_platform(s3eAmazonAdsId adId) // run_on_os_thread
{
    S3eAmazonAdSngl* singleton = [S3eAmazonAdSngl theSingleton];
    BOOL ok = [singleton loadInterstitialAd:(id)adId];
    return ok ? S3E_RESULT_SUCCESS : S3E_RESULT_ERROR;
}

s3eResult s3eAmazonAdsShowAd_platform(s3eAmazonAdsId adId) // run_on_os_thread
{
    S3eAmazonAdSngl* singleton = [S3eAmazonAdSngl theSingleton];
    BOOL ok = [singleton showAd:(id)adId];
    return ok ? S3E_RESULT_SUCCESS : S3E_RESULT_ERROR;
}

static s3eResult s3eAmazonAdsInspectAd_real(s3eAmazonAdsId adId, s3eAmazonAdsAdInfo* info)
{
    S3eAmazonAdSngl* singleton = [S3eAmazonAdSngl theSingleton];
    BOOL ok = [singleton inspectAd:(id)adId info:info];
    return ok ? S3E_RESULT_SUCCESS : S3E_RESULT_ERROR;
}

s3eResult s3eAmazonAdsInspectAd_platform(s3eAmazonAdsId adId, s3eAmazonAdsAdInfo* info)
{
    return (s3eResult)(intptr_t)s3eEdkThreadRunOnOS((s3eEdkThreadFunc)s3eAmazonAdsInspectAd_real, 2, adId, info);
}

// callbacks

static void s3eAmazonAds_CallbackCleanup(uint32, int32, void*, void*, int32, s3eAmazonAdsCallbackLoadedData* pData)
{
    IwTrace(AMAZONADS, ("Cleaning up callback data: %p", pData));
    if (pData)
    {
        if (pData->m_Properties)
            delete pData->m_Properties;
        delete pData;
    }
}

extern "C" void s3eAmazonAds_onLoad(struct s3eAmazonAdsCallbackLoadedData* origdata)
{
    IwTrace(AMAZONADS, ("onLoad callback"));
    
    s3eAmazonAdsCallbackLoadedData* data = new s3eAmazonAdsCallbackLoadedData();
    data->m_Id = origdata->m_Id;
    data->m_Properties = new s3eAmazonAdsProperties();
    data->m_Properties->m_CanExpand    = origdata->m_Properties->m_CanExpand;
    data->m_Properties->m_CanPlayAudio = origdata->m_Properties->m_CanPlayAudio;
    data->m_Properties->m_CanPlayVideo = origdata->m_Properties->m_CanPlayVideo;
    data->m_Properties->m_AdType       = origdata->m_Properties->m_AdType;
    
    s3eEdkCallbacksEnqueue(S3E_EXT_AMAZONADS_HASH, S3E_AMAZONADS_CALLBACK_AD_LOADED, (void*) data, 0, NULL, S3E_FALSE, (s3eEdkCallbackCompleteFn) &s3eAmazonAds_CallbackCleanup, (void*) data);

}

extern "C" void s3eAmazonAds_onError(id adId, int errorCode)
{
    IwTrace(AMAZONADS, ("onError callback"));
    
    s3eAmazonAdsCallbackErrorData data;
    data.m_Id = (s3eAmazonAdsId)adId;
    // note we need to translate each errorCode since the s3e error values reflect the
    // android values, and the iOS ones are different
    int s3eError;
    switch (errorCode)
    {
        case AmazonAdErrorRequest:
            s3eError = S3E_AMAZONADS_ERR_LOAD_REQUEST_ERROR;
            break;
        case AmazonAdErrorNoFill:
            s3eError = S3E_AMAZONADS_ERR_LOAD_NO_FILL;
            break;
        case AmazonAdErrorInternalServer:
            s3eError = S3E_AMAZONADS_ERR_LOAD_INTERNAL_ERROR;
            break;
        case AmazonAdErrorNetworkConnection:
            s3eError = S3E_AMAZONADS_ERR_LOAD_NETWORK_ERROR;
        default:
            s3eError = S3E_AMAZONADS_ERR_LOAD_UNKNOWN_ERROR;
            break;
    }
    data.m_Error = s3eAmazonAdsLoadError(s3eError);
    s3eEdkCallbacksEnqueue(S3E_EXT_AMAZONADS_HASH, S3E_AMAZONADS_CALLBACK_AD_ERROR, (void*)&data, sizeof(data));
}


extern "C" void s3eAmazonAds_onAction(id adId, AmazonAdAction action)
{
    IwTrace(AMAZONADS, ("onAction callback"));
    
    s3eAmazonAdsCallbackActionData data;
    data.m_Id = (s3eAmazonAdsId) adId;
    data.m_Type = (s3eAmazonAdsActionType) action;
    s3eEdkCallbacksEnqueue(S3E_EXT_AMAZONADS_HASH, S3E_AMAZONADS_CALLBACK_AD_ACTION, (void*)&data, sizeof(data));
}


