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
  * android-specific implementation of the s3eAmazonAds extension.
  */

#include "s3eAmazonAds_internal.h"

#include "s3eEdk.h"
#include "s3eEdk_android.h"
#include <jni.h>
#include "IwDebug.h"
#include <string.h>

#include <map>

static jobject g_Obj;
static jmethodID g_s3eAmazonAdsInit;
static jmethodID g_s3eAmazonAdsTerminate;
static jmethodID g_s3eAmazonAdsPrepareAd;
static jmethodID g_s3eAmazonAdsPrepareAdLayout;
static jmethodID g_s3eAmazonAdsLoadAd;
static jmethodID g_s3eAmazonAdsDestroyAd;
static jmethodID g_s3eAmazonAdsLoadInterstitialAd;
static jmethodID g_s3eAmazonAdsShowAd;
static jmethodID g_s3eAmazonAdsInspectAd;

// updating options:
static jmethodID g_s3eAmazonAdsGetAdOptionsObj;
static jmethodID g_s3eAdOptions_setOptions;
static jmethodID g_s3eAdOptions_getOptions;


#define SET_ERR_STR(A,B) s3eAmazonAdsSetErrorString( S3E_AMAZONADS_ERR_##A, B)

namespace
{
typedef std::map<int, s3eAmazonAdsTargetingOptions*> myopts;

class COptionsMap
{

    static myopts m_Options;
public:
    static s3eAmazonAdsTargetingOptions* GetOptions(int id)
    {
        if (id < 0)
            return NULL;

        s3eAmazonAdsTargetingOptions* options = NULL;
        if (m_Options.find(id) == m_Options.end())
        {
            IwTrace(AMAZONADS, ("Creating options cache object for id: %d", id));
            options = new s3eAmazonAdsTargetingOptions();
            options->m_AdvancedOptions = NULL;
            m_Options[id] = options;
        }
        else
        {
            IwTrace(AMAZONADS, ("Found cache object for id: %d", id));
            options = m_Options[id];

        }
        return options;
    }

    static bool RemoveOptions(int id)
    {
        if (id >=0 && m_Options.find(id) != m_Options.end())
        {
            s3eAmazonAdsTargetingOptions* tmp;
            tmp = m_Options[id];
            if (tmp)
            {
                if (tmp->m_AdvancedOptions)
                    delete[] tmp->m_AdvancedOptions;
                delete tmp;
            }
            m_Options.erase(id);
            return true;
        }
        return false;
    }

    static void RemoveAllOptions()
    {
        if (!m_Options.empty())
        {
            for(myopts::iterator it = m_Options.begin(); it != m_Options.end(); )
            {
                if (RemoveOptions(it->first))
                    it = m_Options.begin();
                else
                    ++it;
            }
            m_Options.clear();
        }
    }
};

myopts COptionsMap::m_Options; // static property instance
}

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


void JNICALL s3eAmazonAds_AdLoadedCallback(JNIEnv* env, jobject obj, jint id, jobject properties)
{
    IwTrace(AMAZONADS, ("s3eAmazonAds_AdLoadedCallback: %d", id));


    jclass cls = env->FindClass( "com/ideaworks3d/marmalade/s3eAmazonAds/s3eAmazonAdsListener$s3eAmazonAdsProperties");

    // boolean m_CanExpand;
    // boolean m_CanPlayAudio;
    // boolean m_CanPlayVideo;
    // int     m_AdType;
    //
    jfieldID fieldexpand = env->GetFieldID(cls, "m_CanExpand", "Z");
    jfieldID fieldaudio  = env->GetFieldID(cls, "m_CanPlayAudio", "Z");
    jfieldID fieldvideo  = env->GetFieldID(cls, "m_CanPlayVideo", "Z");
    jfieldID fieldtype   = env->GetFieldID(cls, "m_AdType", "I");

    s3eAmazonAdsCallbackLoadedData* data = new s3eAmazonAdsCallbackLoadedData();
    data->m_Id = id;
    data->m_Properties = new s3eAmazonAdsProperties();
    data->m_Properties->m_CanExpand    = (bool) env->GetBooleanField(properties, fieldexpand);
    data->m_Properties->m_CanPlayAudio = (bool) env->GetBooleanField(properties, fieldaudio);
    data->m_Properties->m_CanPlayVideo = (bool) env->GetBooleanField(properties, fieldvideo);
    data->m_Properties->m_AdType       = (s3eAmazonAdsAdType) env->GetIntField(properties, fieldtype);

    s3eEdkCallbacksEnqueue(S3E_EXT_AMAZONADS_HASH, S3E_AMAZONADS_CALLBACK_AD_LOADED, (void*) data, 0, NULL, S3E_FALSE, (s3eEdkCallbackCompleteFn) &s3eAmazonAds_CallbackCleanup, (void*) data);
}

void JNICALL s3eAmazonAds_AdActionCallback(JNIEnv* env, jobject obj, jint id, jint action)
{
    IwTrace(AMAZONADS, ("s3eAmazonAds_AdActionCallback: %d", id));

    s3eAmazonAdsCallbackActionData data;
    data.m_Id = id;
    data.m_Type = (s3eAmazonAdsActionType) action;
    s3eEdkCallbacksEnqueue(S3E_EXT_AMAZONADS_HASH, S3E_AMAZONADS_CALLBACK_AD_ACTION, (void*)&data, sizeof(data));
}

void JNICALL s3eAmazonAds_AdErrorCallback(JNIEnv* env, jobject obj, jint id, jint errorcode)
{
    IwTrace(AMAZONADS, ("s3eAmazonAds_AdErrorCallback: %d", id));

    s3eAmazonAdsCallbackErrorData data;
    data.m_Id = id;
    if (errorcode >= S3E_AMAZONADS_ERR_LOAD_NETWORK_ERROR)
        data.m_Error = (s3eAmazonAdsLoadError) errorcode;
    else
        data.m_Error = S3E_AMAZONADS_ERR_LOAD_UNKNOWN_ERROR;
    s3eEdkCallbacksEnqueue(S3E_EXT_AMAZONADS_HASH, S3E_AMAZONADS_CALLBACK_AD_ERROR, (void*)&data, sizeof(data));
}

s3eResult s3eAmazonAdsInit_platform()
{
    // Get the environment from the pointer
    JNIEnv* env = s3eEdkJNIGetEnv();
    jobject obj = NULL;
    jmethodID cons = NULL;

    // Get the extension class
    jclass cls = s3eEdkAndroidFindClass("com/ideaworks3d/marmalade/s3eAmazonAds/s3eAmazonAds");
    if (!cls)
        goto fail;

    // Get its constructor
    cons = env->GetMethodID(cls, "<init>", "()V");
    if (!cons)
        goto fail;

    // Construct the java class
    obj = env->NewObject(cls, cons);
    if (!obj)
        goto fail;

    // Get all the extension methods
    g_s3eAmazonAdsInit = env->GetMethodID(cls, "s3eAmazonAdsInit", "(Ljava/lang/String;ZZ)I");
    if (!g_s3eAmazonAdsInit)
        goto fail;

    g_s3eAmazonAdsTerminate = env->GetMethodID(cls, "s3eAmazonAdsTerminate", "()I");
    if (!g_s3eAmazonAdsTerminate)
        goto fail;

    g_s3eAmazonAdsPrepareAd = env->GetMethodID(cls, "s3eAmazonAdsPrepareAd", "([I)I");
    if (!g_s3eAmazonAdsPrepareAd)
        goto fail;

    g_s3eAmazonAdsPrepareAdLayout = env->GetMethodID(cls, "s3eAmazonAdsPrepareAdLayout", "(IIIII)I");
    if (!g_s3eAmazonAdsPrepareAdLayout)
        goto fail;

    g_s3eAmazonAdsLoadAd = env->GetMethodID(cls, "s3eAmazonAdsLoadAd", "(IZI)I");
    if (!g_s3eAmazonAdsLoadAd)
        goto fail;

    g_s3eAmazonAdsDestroyAd = env->GetMethodID(cls, "s3eAmazonAdsDestroyAd", "(I)I");
    if (!g_s3eAmazonAdsDestroyAd)
        goto fail;

    g_s3eAmazonAdsLoadInterstitialAd = env->GetMethodID(cls, "s3eAmazonAdsLoadInterstitialAd", "(I)I");
    if (!g_s3eAmazonAdsLoadInterstitialAd)
        goto fail;

    g_s3eAmazonAdsShowAd = env->GetMethodID(cls, "s3eAmazonAdsShowAd", "(I)I");
    if (!g_s3eAmazonAdsShowAd)
        goto fail;

    g_s3eAmazonAdsInspectAd = env->GetMethodID(cls, "s3eAmazonAdsInspectAd", "(I[I)I");
    if (!g_s3eAmazonAdsInspectAd)
        goto fail;

    // internal helpers:
    g_s3eAmazonAdsGetAdOptionsObj = env->GetMethodID(cls, "s3eAmazonAdsGetAdOptions", "(I)Lcom/ideaworks3d/marmalade/s3eAmazonAds/s3eAmazonAds$s3eAdOptions;");
    if (!g_s3eAmazonAdsGetAdOptionsObj)
        goto fail;

    // register native callbacks
    {
        jclass listener = s3eEdkAndroidFindClass("com/ideaworks3d/marmalade/s3eAmazonAds/s3eAmazonAdsListener");

        static const JNINativeMethod methods[]=
        {
            {"native_AdLoadedCallback", "(ILcom/ideaworks3d/marmalade/s3eAmazonAds/s3eAmazonAdsListener$s3eAmazonAdsProperties;)V",  (void*) s3eAmazonAds_AdLoadedCallback},
            {"native_AdActionCallback", "(II)V",  (void*) s3eAmazonAds_AdActionCallback},
            {"native_AdErrorCallback",  "(II)V",  (void*) s3eAmazonAds_AdErrorCallback},
        };

        jint ret = env->RegisterNatives(listener, methods, sizeof(methods)/sizeof(methods[0]));
        if (ret)
        {
            IwTrace(AMAZONADS, ("AMAZONADS failed to register native callbacks with error: %d", ret));
            env->DeleteGlobalRef(listener);
            goto fail;
        }
        env->DeleteGlobalRef(listener);
    }

    IwTrace(AMAZONADS, ("AMAZONADS init success"));
    g_Obj = env->NewGlobalRef(obj);
    env->DeleteLocalRef(obj);
    env->DeleteGlobalRef(cls);

    return S3E_RESULT_SUCCESS;

fail:
    jthrowable exc = env->ExceptionOccurred();
    if (exc)
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
        IwTrace(AMAZONADS, ("One or more java methods could not be found"));
    }
    if (obj)
        env->DeleteLocalRef(obj);
    if (cls)
        env->DeleteGlobalRef(cls);
    return S3E_RESULT_ERROR;

}

s3eResult s3eAmazonAdsInit_platform(const char* appKey, s3eBool enableTesting, s3eBool enableLogging)
{
    // TODO key validation
    static bool done = false;
    if (!done)
    {
        s3eAmazonAdsInit_platform();
        done = true;
    }

    JNIEnv* env = s3eEdkJNIGetEnv();
    jstring appKey_jstr = env->NewStringUTF(appKey);

    int res = env->CallIntMethod(g_Obj, g_s3eAmazonAdsInit, appKey_jstr, enableTesting, enableLogging);
    
    env->DeleteLocalRef(appKey_jstr);
     
    if (res != S3E_AMAZONADS_ERR_NONE)
    {
        SET_ERR_STR(INVALID_APPKEY, "App registration error possibly due to invalid app key.");
        return S3E_RESULT_ERROR;
    }

    return S3E_RESULT_SUCCESS;
 }


s3eResult s3eAmazonAdsTerminate_platform()
{
    COptionsMap::RemoveAllOptions();

    JNIEnv* env = s3eEdkJNIGetEnv();
    int res = env->CallIntMethod(g_Obj, g_s3eAmazonAdsTerminate);

    return (res == S3E_AMAZONADS_ERR_NONE) ? S3E_RESULT_SUCCESS : S3E_RESULT_ERROR;
}

s3eResult s3eAmazonAdsPrepareAd_platform(s3eAmazonAdsId* pId)
{
    if (pId == NULL)
    {
        SET_ERR_STR(NULL_PARAM, "pId is NULL");
        return S3E_RESULT_ERROR;
    }

    JNIEnv* env = s3eEdkJNIGetEnv();
    jintArray idarr = (jintArray)env->NewIntArray(1);
    int res = env->CallIntMethod(g_Obj, g_s3eAmazonAdsPrepareAd, idarr);
    int* ptr = (int*) env->GetIntArrayElements(idarr, NULL);
    *pId = ptr[0];

    env->ReleaseIntArrayElements(idarr, ptr, JNI_ABORT);
    env->DeleteLocalRef(idarr);

    if (res != S3E_AMAZONADS_ERR_NONE)
        SET_ERR_STR(UNEXPECTED, "Unexpected error when generating id.");

    return (res == S3E_AMAZONADS_ERR_NONE) ? S3E_RESULT_SUCCESS : S3E_RESULT_ERROR;
}

s3eResult s3eAmazonAdsPrepareAdLayout_platform(s3eAmazonAdsId id, s3eAmazonAdsPosition position, s3eAmazonAdsSize size, int width, int height)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    int res = env->CallIntMethod(g_Obj, g_s3eAmazonAdsPrepareAdLayout, id, position, size, width, height);

    if (res == S3E_AMAZONADS_ERR_INVALID_ID) {
        SET_ERR_STR(INVALID_ID, "Invalid id.  Call s3eAmazonAdsPrepareAd to generate an id.");
    }
    else if (res == S3E_AMAZONADS_ERR_INVALID_POSITION) {
        SET_ERR_STR(INVALID_POSITION, "Invalid position.  Only top and bottom are support for banner ads.");
    }
    else if (res == S3E_AMAZONADS_ERR_INVALID_SIZE) {
        SET_ERR_STR(INVALID_SIZE, "Invalid size.  Use predefined size , auto, or a valid custom size.");
    }
    return (res == S3E_AMAZONADS_ERR_NONE) ? S3E_RESULT_SUCCESS : S3E_RESULT_ERROR;
}

s3eResult s3eAmazonAdsSetAdTargetingOptions_platform(s3eAmazonAdsId id, s3eAmazonAdsTargetingOptions* options)
{
    IwTrace(AMAZONADS, ("s3eAmazonAdsSetAdTargetingOptions"));

    if (options == NULL)
    {
        SET_ERR_STR(NULL_PARAM, "options is NULL");
        return S3E_RESULT_ERROR;
    }

    int res = 0;

    // get the target options object
    JNIEnv* env = s3eEdkJNIGetEnv();
    jobject obj = env->CallObjectMethod(g_Obj, g_s3eAmazonAdsGetAdOptionsObj, id);
    if (obj == NULL)
    {
        SET_ERR_STR(INVALID_ID, "Invalid id.");
        return S3E_RESULT_ERROR;
    }

    jclass cls = s3eEdkAndroidFindClass("com/ideaworks3d/marmalade/s3eAmazonAds/s3eAmazonAds$s3eAdOptions");
    if (g_s3eAdOptions_setOptions == NULL)
    {
        // public void setOptions(int price, boolean update_price, int age, boolean update_age, boolean geolocation, boolean update_geo, String advanced, boolean update_advanced)
        g_s3eAdOptions_setOptions = env->GetMethodID(cls, "setOptions", "(IZIZZZLjava/lang/String;Z)I");
    }

    jstring advanced_jstr = env->NewStringUTF(options->m_AdvancedOptions);

    res = env->CallIntMethod(obj, g_s3eAdOptions_setOptions,
        options->m_FloorPrice,        options->m_OptionsMask & S3E_AMAZONADS_OPTIONS_FLOORPRICE,
        options->m_Age,               options->m_OptionsMask & S3E_AMAZONADS_OPTIONS_AGE,
        options->m_EnableGeoLocation, options->m_OptionsMask & S3E_AMAZONADS_OPTIONS_GEOLOCATION,
        advanced_jstr,                options->m_OptionsMask & S3E_AMAZONADS_OPTIONS_ADVANCED);

    if (res != S3E_AMAZONADS_ERR_NONE) // TODO advanced validation
        SET_ERR_STR(UNEXPECTED, "Unexpected error when setting targeting options. Is the id valid?");

    env->DeleteLocalRef(obj);
    env->DeleteGlobalRef(cls);

    return (res == S3E_AMAZONADS_ERR_NONE) ? S3E_RESULT_SUCCESS : S3E_RESULT_ERROR;
}

s3eAmazonAdsTargetingOptions* s3eAmazonAdsGetAdTargetingOptions_platform(s3eAmazonAdsId id)
{
    JNIEnv* env = s3eEdkJNIGetEnv();

    // get the target options object
    jobject obj = env->CallObjectMethod(g_Obj, g_s3eAmazonAdsGetAdOptionsObj, id);
    if (obj == NULL)
    {
        IwTrace(AMAZONADS, ("s3eAmazonAdsGetAdTargetingOptions Error AdOptionsObj == NULL."));

        SET_ERR_STR(INVALID_ID, "Invalid id.");
        return NULL;
    }

    IwTrace(AMAZONADS, ("s3eAmazonAdsGetAdTargetingOptions_platform"));

    jclass cls = s3eEdkAndroidFindClass("com/ideaworks3d/marmalade/s3eAmazonAds/s3eAmazonAds$s3eAdOptions");
    if (g_s3eAdOptions_getOptions == NULL)
    {
        // public String getOptions(int[] opt)
        g_s3eAdOptions_getOptions = env->GetMethodID(cls, "getOptions", "([I)Ljava/lang/String;");
    }

    jintArray array = (jintArray)env->NewIntArray(7);

    jstring str = (jstring) env->CallObjectMethod(obj, g_s3eAdOptions_getOptions, array);

    // errcode, price, _price, age, _age, geolocation, _geo
    int* ptr = (int*)env->GetIntArrayElements(array, NULL);

    // int res     = ptr[0]; 
    int price   = ptr[1];
    bool _price = (bool) ptr[2];
    int age     = ptr[3];
    bool _age   = (bool) ptr[4];
    bool geo    = (bool) ptr[5];
    bool _geo   = (bool) ptr[6];

    IwTrace(AMAZONADS, ("s3eAmazonAdsGetAdTargetingOptions_platform %d %d %d", age, price, (int) geo));

    int32 mask = (_price && S3E_AMAZONADS_OPTIONS_FLOORPRICE) |
        (_age && S3E_AMAZONADS_OPTIONS_AGE) |
        (_geo && S3E_AMAZONADS_OPTIONS_GEOLOCATION) |
        ((str != NULL) && S3E_AMAZONADS_OPTIONS_ADVANCED);

    env->ReleaseIntArrayElements(array, ptr, JNI_ABORT);
    env->DeleteLocalRef(array);

    char* advanced = NULL;
    if (str != NULL)
    {
        const char *nativeString = env->GetStringUTFChars(str, 0);

        int length = strlen(nativeString) + 1;
        IwTrace(AMAZONADS, ("Length of advanced string: %d", length));

        if (length < 1024*16) // 16KB
        {
            advanced = new char[length];
            strcpy(advanced, nativeString);
        }
        else
        {
            advanced = new char[10];
            advanced[0] = '\0';
            IwTrace(AMAZONADS, ("Error: length > 16KB."));
        }

       env->ReleaseStringUTFChars(str, nativeString);
    }

    env->DeleteLocalRef(obj);
    env->DeleteGlobalRef(cls);

    // cache options
    s3eAmazonAdsTargetingOptions* options = COptionsMap::GetOptions(id);
    if (options)
    {
        options->m_OptionsMask = mask;
        options->m_FloorPrice = price;
        options->m_Age = age;
        options->m_EnableGeoLocation = geo ? S3E_TRUE : S3E_FALSE;

        if (options->m_AdvancedOptions != NULL)
            delete[] options->m_AdvancedOptions;

        options->m_AdvancedOptions = advanced;
        IwTrace(AMAZONADS_VERBOSE, ("Getting options: %s", advanced));
    }
    else
    {
        IwTrace(AMAZONADS, ("Error couldn't allocate memory for options."));
        SET_ERR_STR(UNEXPECTED, ("Unexpected error when trying to allocate memory for options."));
    }

    return options;
}

s3eResult s3eAmazonAdsLoadAd_platform(s3eAmazonAdsId id, s3eBool show, int timeout)
{
    IwTrace(AMAZONADS, ("s3eAmazonAdsLoadAd id: %d show: %d  timeout: %d", id, show, timeout));
    JNIEnv* env = s3eEdkJNIGetEnv();
    int res = env->CallIntMethod(g_Obj, g_s3eAmazonAdsLoadAd, id, show, timeout);

    switch (res)
    {
        case S3E_AMAZONADS_ERR_NONE:
            return S3E_RESULT_SUCCESS;
        case S3E_AMAZONADS_ERR_INVALID_ID:
            SET_ERR_STR(INVALID_ID, "Invalid id.");
            break;
        case S3E_AMAZONADS_ERR_ID_USED_FOR_INTERSTITIAL:
            SET_ERR_STR(ID_USED_FOR_INTERSTITIAL, "Use s3eAmazonAdsLoadInterstitialAd for loading interstitials.");
            break;
        case S3E_AMAZONADS_ERR_BUSY_LOADING:
            SET_ERR_STR(BUSY_LOADING, "Cannot load ad that is busy loading.");
            break;
        default:
            SET_ERR_STR(UNEXPECTED, "Unexpected failure to collapse ad.");
            break;
    }

    return S3E_RESULT_ERROR;
}

s3eResult s3eAmazonAdsDestroyAd_platform(s3eAmazonAdsId id)
{
    IwTrace(AMAZONADS, ("s3eAmazonAdsDestroyAd id: %d", id));

    COptionsMap::RemoveOptions(id);

    JNIEnv* env = s3eEdkJNIGetEnv();
    int res = env->CallIntMethod(g_Obj, g_s3eAmazonAdsDestroyAd, id);

    switch (res)
    {
        case S3E_AMAZONADS_ERR_NONE:
            return S3E_RESULT_SUCCESS;
        case S3E_AMAZONADS_ERR_INVALID_ID:
            SET_ERR_STR(INVALID_ID, "Invalid id.");
            break;
        case S3E_AMAZONADS_ERR_NULL_AD:
            SET_ERR_STR(NULL_AD, "Cannot collapse since AdView is NULL.");
            break;
        default:
            SET_ERR_STR(UNEXPECTED, "Unexpected failure to destroy ad.");
            break;
    }

    return S3E_RESULT_ERROR;
}

s3eResult s3eAmazonAdsCollapseAd_platform(s3eAmazonAdsId /*id*/)
{
    // no longer implemented in SDK - so just return an error
    return S3E_RESULT_ERROR;
}

s3eResult s3eAmazonAdsLoadInterstitialAd_platform(s3eAmazonAdsId id)
{
    IwTrace(AMAZONADS, ("s3eAmazonAdsLoadInterstitialAd id: %d", id));
    JNIEnv* env = s3eEdkJNIGetEnv();
    int res = env->CallIntMethod(g_Obj, g_s3eAmazonAdsLoadInterstitialAd, id);

    switch (res)
    {
        case S3E_AMAZONADS_ERR_NONE:
            return S3E_RESULT_SUCCESS;
        case S3E_AMAZONADS_ERR_INVALID_ID:
            SET_ERR_STR(INVALID_ID, "Invalid id.");
            break;
        case S3E_AMAZONADS_ERR_BUSY_LOADING:
            SET_ERR_STR(BUSY_LOADING, "Busy loading.");
            break;
        case S3E_AMAZONADS_ERR_UNEXPECTED:
        default:
            SET_ERR_STR(UNEXPECTED, "Unexpected failure when loading interstitial ad.");
            break;
    }

    return S3E_RESULT_ERROR;
}

s3eResult s3eAmazonAdsShowAd_platform(s3eAmazonAdsId id)
{
    IwTrace(AMAZONADS, ("s3eAmazonAdsShowAd id: %d", id));
    JNIEnv* env = s3eEdkJNIGetEnv();
    int res = env->CallIntMethod(g_Obj, g_s3eAmazonAdsShowAd, id);

    switch (res)
    {
        case S3E_AMAZONADS_ERR_NONE:
            return S3E_RESULT_SUCCESS;
        case S3E_AMAZONADS_ERR_INVALID_ID:
            SET_ERR_STR(INVALID_ID, "Invalid id.");
            break;
        case S3E_AMAZONADS_ERR_SHOW_FAILED:
            SET_ERR_STR(SHOW_FAILED, "Failed calling when showing ad.");
            break;
        case S3E_AMAZONADS_ERR_NOT_LOADED:
            SET_ERR_STR(NOT_LOADED, "Cannot show ad that has not been loaded.");
            break;
        case S3E_AMAZONADS_ERR_ALREADY_SHOWING:
            SET_ERR_STR(ALREADY_SHOWING, "Cannot show ad that is already showing.");
            break;
        case S3E_AMAZONADS_ERR_NULL_AD:
            SET_ERR_STR(NULL_AD, "Cannot show NULL ad.");
            break;
        default:
            SET_ERR_STR(UNEXPECTED, "Unexpected failure to show ad.");
            break;
    }

    return S3E_RESULT_ERROR;
}

s3eResult s3eAmazonAdsInspectAd_platform(s3eAmazonAdsId id, s3eAmazonAdsAdInfo* info)
{
    IwTrace(AMAZONADS, ("s3eAmazonAdsInspectAd id: %d", id));

    if (info == NULL)
    {
        SET_ERR_STR(NULL_PARAM, "info is NULL");
        return S3E_RESULT_ERROR;
    }

    JNIEnv* env = s3eEdkJNIGetEnv();
    jintArray idarr = (jintArray)env->NewIntArray(1);

    int res = env->CallIntMethod(g_Obj, g_s3eAmazonAdsInspectAd, id, idarr);

    if (res == S3E_AMAZONADS_ERR_NONE)
    {
        int* ptr = (int*) env->GetIntArrayElements(idarr, NULL);
        info->m_IsLoading = ptr[0] == 1 ? S3E_TRUE : S3E_FALSE;
        env->ReleaseIntArrayElements(idarr, ptr, JNI_ABORT);
    }
    else
    {
        SET_ERR_STR(INVALID_ID, "Invalid id.");
    }

    env->DeleteLocalRef(idarr);

    return (res == S3E_AMAZONADS_ERR_NONE) ? S3E_RESULT_SUCCESS : S3E_RESULT_ERROR;
}
