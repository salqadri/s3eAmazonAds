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
 * WARNING: this is an autogenerated file and will be overwritten by
 * the extension interface script.
 */
#ifndef S3E_EXT_AMAZONADS_H
#define S3E_EXT_AMAZONADS_H

#include <s3eTypes.h>


/**
 * @addtogroup s3egroup
 * @{
 */

/**
 * @defgroup amazonadsapigroup S3E AmazonAds API Reference
 *
 * Allows your application to show ads served from the Amazon Mobile Ad Network.
 *
 * @supported{
 *             @platform{Android,android}
 *             @platform{iOS,ios}
 * }
 *
 */

/**
 * @}
 * @addtogroup amazonadsapigroup
 * @{
 */

// Warning: The error codes below should ideally not be changed - the extension
// code depends on their order and they are declared both here and in java.

/**
 * General error codes.
 *
 * @par Required Header Files
 * s3eAmazonAds.h
 */
typedef enum s3eAmazonAdsError
{
    S3E_AMAZONADS_ERR_NONE                     = 0,    ///< No error.
    S3E_AMAZONADS_ERR_UNEXPECTED               = 1,    ///< Unexpected error (unrecoverable runtime/memory error).
    S3E_AMAZONADS_ERR_INVALID_APPKEY           = 2,    ///< Application Key validation error (null, non-alphanumeric or length).
    S3E_AMAZONADS_ERR_INVALID_SIZE             = 3,    ///< Invalid ad dimensions.
    S3E_AMAZONADS_ERR_INVALID_OPTION           = 4,    ///< One or more options are invalid.
    S3E_AMAZONADS_ERR_NULL_PARAM               = 5,    ///< NULL pointer exception.
    S3E_AMAZONADS_ERR_INVALID_ID               = 6,    ///< Ad identifier is invalid or no longer valid.
    S3E_AMAZONADS_ERR_INVALID_POSITION         = 7,    ///< Ad position is invalid.
    S3E_AMAZONADS_ERR_ALREADY_COLLAPSED        = 8,    ///< Banner cannot be collapsed as it is not expanded.
    S3E_AMAZONADS_ERR_CANNOT_COLLAPSE          = 9,    ///< Ad cannot be collapsed (banner or interstitial).
    S3E_AMAZONADS_ERR_REGISTRATION_EXCEPTION   = 10,   ///< App registration error (likely app key error)
    S3E_AMAZONADS_ERR_BUSY_LOADING             = 11,   ///< Trying to load when ad is already busy loading.
    S3E_AMAZONADS_ERR_ID_USED_FOR_INTERSTITIAL = 12,   ///< Operation reserved for banners used on id used for interstitials.
    S3E_AMAZONADS_ERR_NULL_AD                  = 13,   ///< Operation requires an ad to be loaded, e.g. collapsing an ad.
    S3E_AMAZONADS_ERR_SHOW_FAILED              = 14,   ///< An error occurred in trying to show the ad.
    S3E_AMAZONADS_ERR_ALREADY_SHOWING          = 15,   ///< Cannot show an ad that is already showing.
    S3E_AMAZONADS_ERR_NOT_LOADED               = 16,   ///< Banner ad must be loaded before trying to show it.
} s3eAmazonAdsError;


/**
 * Ad load error codes.
 *
 * @par Required Header Files
 * s3eAmazonAds.h
 */
typedef enum s3eAmazonAdsLoadError
{
    S3E_AMAZONADS_ERR_LOAD_UNKNOWN_ERROR    = -1,   ///< Unknown error during load (unexpected and should not happen).
    S3E_AMAZONADS_ERR_LOAD_NETWORK_ERROR    = 0,    ///< Network error due to connectivity issues. Retry once resolved.
    S3E_AMAZONADS_ERR_LOAD_NETWORK_TIMEOUT  = 1,    ///< Network timeout.
    S3E_AMAZONADS_ERR_LOAD_NO_FILL          = 2,    ///< No ads available. If this happens do not retry immediately.
    S3E_AMAZONADS_ERR_LOAD_INTERNAL_ERROR   = 3,    ///< Error on the Amazon Mobile Ad Network. You can retry immediately but limit the retry count.
    S3E_AMAZONADS_ERR_LOAD_REQUEST_ERROR    = 4,    ///< Request parameters invalid. Retry only after parameters have been fixed.
} s3eAmazonAdsLoadError;


/**
 * Callback events.
 *
 * @par Required Header Files
 * s3eAmazonAds.h
 */
typedef enum s3eAmazonAdsCallback
{
    /**
     * Called when the ad has finished loading.  This is the earliest point
     * at which you can show the ad.  Note that waiting too long
     * to show the ad may result in the ad being expired (see errors).
     * SystemData will point to s3eAmazonAdsCallbackLoadedData.
     */
    S3E_AMAZONADS_CALLBACK_AD_LOADED = 0,

    /**
     * Called after user interaction changes the ad state to being
     * expanded or collapsed (for banner ads) or dismissed (for
     * interstitial ads).
     * SystemData will point to s3eAmazonAdsCallbackActionData.
     * Your may want to choose to pause or resume the app's audio at this point.
     */
    S3E_AMAZONADS_CALLBACK_AD_ACTION = 1,

    /**
     * Called when the ad has failed to load.
     * SystemData will point to s3eAmazonAdsCallbackErrorData.
     */
    S3E_AMAZONADS_CALLBACK_AD_ERROR = 2,

    S3E_AMAZONADS_CALLBACK_MAX
} s3eAmazonAdsCallback;


/**
 * Opaque id
 * This id acts as an opaque handle that identifies an ad request and object.
 * It is generated by calling s3eAmazonAdsPrepareAd and is used throughout this
 * API to perform per ad operations.
 *
 * @remark Negative ids are invalid.
 * @remark Ids not generated by s3eAmazonAdsPrepareAd are invalid.
 * @remark Ids are invalidated when the underlying ad object is destroyed.
 */
typedef intptr_t s3eAmazonAdsId;


/**
 * Ad user action types.
 *
 * @par Required Header Files
 * s3eAmazonAds.h
 */
typedef enum s3eAmazonAdsActionType
{
    S3E_AMAZONADS_ACTION_EXPANDED  = 1,     ///< User expanded a banner ad.
    S3E_AMAZONADS_ACTION_COLLAPSED = 2,     ///< User collapsed a banner ad.
    S3E_AMAZONADS_ACTION_DISMISSED = 3,     ///< User dismissed an interstitial ad.
} s3eAmazonAdsActionType;


/**
 * Ad targeting options bitmask.
 * Use this bitmask to specify which targeting options set in s3eAmazonAdsTargetingOptions
 * you want to use in the call to s3eAmazonAdsSetAdTargetingOptions.
 * It also specifies which options are valid in the s3eAmazonAdsTargetingOptions
 * object returned from s3eAmazonAdsGetAdTargetingOptions.
 *
 * @par Required Header Files
 * s3eAmazonAds.h
 */
typedef enum s3eAmazonAdsTargetingOptionsMask
{
    S3E_AMAZONADS_OPTIONS_AGE          = 0x1,    ///< Banner ad can be expanded.
    S3E_AMAZONADS_OPTIONS_FLOORPRICE   = 0x2,    ///< Ad can play audio when expanded.
    S3E_AMAZONADS_OPTIONS_GEOLOCATION  = 0x4,    ///< Ad can play video when expanded.
    S3E_AMAZONADS_OPTIONS_ADVANCED     = 0x8,    ///< Ad can play video when expanded.
    S3E_AMAZONADS_OPTONS_ALL           = 0xF,    ///< All options.
} s3eAmazonAdsTargetingOptionsMask;


/**
 * Ad type.
 *
 * @par Required Header Files
 * s3eAmazonAds.h
 */
typedef enum s3eAmazonAdsAdType
{
    S3E_AMAZONADS_TYPE_UNKNOWN           = -1,     ///< Unknown ad type.
    S3E_AMAZONADS_TYPE_IMAGE_BANNER      = 0,      ///< Simple image banner without rich media features.
    S3E_AMAZONADS_TYPE_RICH_MEDIA_MRAID1 = 1,      ///< Rich media ad based on MRAID v1.0 spec.
    S3E_AMAZONADS_TYPE_RICH_MEDIA_MRAID2 = 2,      ///< Rich media ad based on MRAID v2.0 spec.
    S3E_AMAZONADS_TYPE_INTERSTITIAL      = 3,      ///< Interstitial ad.
} s3eAmazonAdsAdType;


/**
 * The Amazon Mobile Ad Network supports the standard ad sizes enumerated below.
 * The dimensions are in device independent pixels (dp). The actual size of an ad
 * in pixels (px) will therefore depend on the device's resolution and pixel density.
 *
 * 1dp : 0.75px at 120 dpi (ldpi)
 * 1dp : 1.00px at 160 dpi (mdpi)
 * 1dp : 1.50px at 240 dpi (hdpi)
 * 1dp : 2.00px at 320 dpi (xhdpi)
 * 1dp : 3.00px at 480 dpi (xxhdpi)
 *
 * For example, S3E_AMAZONADS_SIZE_600x90 on an xxhdpi device (Samsung Galaxy S5)
 * will translate to 1800x270 and appear, in portrait, outside the bounds of the display.
 *
 * Use one of the predefined sizes or S3E_AMAZONADS_SIZE_AUTO when calling
 * s3eAmazonAdsShow.
 *
 * Note: S3E_AMAZONADS_SIZE_600x90 is not currently supported on iOS.
 *
 * @par Required Header Files
 * s3eAmazonAds.h
 */
typedef enum s3eAmazonAdsSize
{
    S3E_AMAZONADS_SIZE_CUSTOM    = -1,  ///< Custom size ad.
    S3E_AMAZONADS_SIZE_AUTO      = 0,   ///< Auto size ad.
    S3E_AMAZONADS_SIZE_300x50    = 1,   ///< Suitable ad size for Android phone.
    S3E_AMAZONADS_SIZE_320x50    = 2,   ///< Suitable ad size for Android phone.
    S3E_AMAZONADS_SIZE_300x250   = 3,   ///< Suitable ad size for Android phone/tablet and Kindle Fire.
    S3E_AMAZONADS_SIZE_600x90    = 4,   ///< Suitable ad size for Android tablet and Kindle Fire. Unsupported on iOS.
    S3E_AMAZONADS_SIZE_728x90    = 5,   ///< Suitable ad size for Android tablet.
    S3E_AMAZONADS_SIZE_1024x50   = 6    ///< Suitable ad size for Android tablet and Kindle Fire.
} s3eAmazonAdsSize;


/**
 * Ad position.
 *
 * @par Required Header Files
 * s3eAmazonAds.h
 */
typedef enum s3eAmazonAdsPosition
{
    S3E_AMAZONADS_POSITION_TOP    = 1,  ///< Ad is positioned at top of screen.
    S3E_AMAZONADS_POSITION_BOTTOM = 2,  ///< Ad is positioned at bottom of screen.
} s3eAmazonAdsPosition;


/**
 * Ad properties.
 * These properties are obtained via the S3E_AMAZONADS_CALLBACK_AD_LOADED callback.
 *
 * @par Required Header Files
 * s3eAmazonAds.h
 */
typedef struct s3eAmazonAdsProperties
{
    s3eAmazonAdsAdType m_AdType;    ///< Enumeration specifying type of ad.
    s3eBool m_CanPlayAudio;         ///< Flag indicating that the ad can play audio.
    s3eBool m_CanPlayVideo;         ///< Flag indicating that the ad can play video.
    s3eBool m_CanExpand;            ///< Flag indicating that the ad can expand.
} s3eAmazonAdsProperties;


/**
 * Callback ad action data.
 * Supplied to callback when callback event is S3E_AMAZONADS_CALLBACK_AD_ACTION.
 *
 * @par Required Header Files
 * s3eAmazonAds.h
 */
typedef struct s3eAmazonAdsCallbackActionData
{
    s3eAmazonAdsId m_Id;                    ///< Ad object identifier.
    s3eAmazonAdsActionType m_Type;          ///< Action that triggered callback.
} s3eAmazonAdsCallbackActionData;


/**
 * Callback loaded data.
 * Supplied to callback when callback event is S3E_AMAZONADS_CALLBACK_AD_LOADED.
 *
 * @par Required Header Files
 * s3eAmazonAds.h
 */
typedef struct s3eAmazonAdsCallbackLoadedData
{
    s3eAmazonAdsId m_Id;                    ///< Ad object identifier.
    s3eAmazonAdsProperties* m_Properties;   ///< Properties of newly loaded ad.
} s3eAmazonAdsLoadedData;


/**
 * Callback error data.
 * Supplied to callback when callback event is S3E_AMAZONADS_CALLBACK_AD_ERROR.
 *
 * @par Required Header Files
 * s3eAmazonAds.h
 */
typedef struct s3eAmazonAdsCallbackErrorData
{
    s3eAmazonAdsId m_Id;                    ///< Ad object identifier.
    s3eAmazonAdsLoadError m_Error;          ///< Ad loading error.
} s3eAmazonAdsCallbackErrorData;


/**
 * Ad targeting options.
 * To set targeting options for an ad request an instance of this structure must be passed
 * to s3eAmazonAdsSetAdTargetingOptions.  The m_OptionsMask bitmask must be set to
 * specify which fields you are interested in setting.  The other fields will simply be
 * ignored.
 * When retrieving options using s3eAmazonAdsGetAdTargetingOptions, use m_OptionsMask
 * to determine which fields hold valid data.
 * An advanced option is a string triplet of the format:
 *    key\nvalue\naction
 * For multiple advanced options concatenate the above triplet with a \n separator:
 *    key\nvalue\naction\nkey\nvalue\naction\nkey\nvalue\naction
 * where action can be "add" or "del" (to delete an existing key/value)
 * @remark Values are passed directly to the Amazon Mobile Ads API without validation.
 *
 * @par Required Header Files
 * s3eAmazonAds.h
 */
typedef struct s3eAmazonAdsTargetingOptions
{
    int32 m_OptionsMask;                ///< s3eAmazonAdsTargetingOptionsMask
    int32 m_FloorPrice;                 ///< Floor price in micro-dollars.
    int32 m_Age;                        ///< Age to pass to the Amazon Mobile Ad Network. Use average age if a range is desired. DEPRECATED
    s3eBool m_EnableGeoLocation;        ///< Allow device to send latitude and longitude coordinates to the Amazon Mobile Ad Network.
    char* m_AdvancedOptions;            ///< Advanced options stored as key/value/action triplets separated by a newline
} s3eAmazonAdsTargetingOptions;


/**
 * Ad state information.
 *
 * @par Required Header Files
 * s3eAmazonAds.h
 */
typedef struct s3eAmazonAdsAdInfo
{
    s3eBool m_IsLoading;        ///< Flag indicating whether or not the ad is loading in the background.
} s3eAmazonAdsAdInfo;
// \cond HIDDEN_DEFINES
S3E_BEGIN_C_DECL
// \endcond

/**
 * Returns S3E_TRUE if the AmazonAds extension is available.
 */
s3eBool s3eAmazonAdsAvailable();

/**
 * Registers a callback to be called for an operating system event.
 *
 * The available callback types are listed in @ref s3eAmazonAdsCallback.
 * @param cbid ID of the event for which to register.
 * @param fn callback function.
 * @param userdata Value to pass to the @e userdata parameter of @e NotifyFunc.
 * @return
 *  - @ref S3E_RESULT_SUCCESS if no error occurred.
 *  - @ref S3E_RESULT_ERROR if the operation failed.\n
 *
 * @see s3eAmazonAdsUnRegister
 * @note For more information on the system data passed as a parameter to the callback
 * registered using this function, see the @ref s3eAmazonAdsCallback enum.
 */
s3eResult s3eAmazonAdsRegister(s3eAmazonAdsCallback cbid, s3eCallback fn, void* userData);

/**
 * Unregister a callback for a given event.
 * @param cbid ID of the callback to unregister.
 * @param fn Callback Function.
 * @return
 * - @ref S3E_RESULT_SUCCESS if no error occurred.
 * - @ref S3E_RESULT_ERROR if the operation failed.\n
 * @note For more information on the systemData passed as a parameter to the callback
 * registered using this function, see the s3eAmazonAdsCallback enum.
 * @note It is not necessary to define a return value for any registered callback.
 * @see s3eAmazonAdsRegister
 */
s3eResult s3eAmazonAdsUnRegister(s3eAmazonAdsCallback cbid, s3eCallback fn);

/**
 * Get the last set error string.
 *
 * @par Required Header Files
 * s3eAmazonAds.h
 */
const char* s3eAmazonAdsGetErrorString();

/**
 * Get the last set error.
 * @return s3eAmazonAdsError error code
 *
 * @par Required Header Files
 * s3eAmazonAds.h
 */
s3eAmazonAdsError s3eAmazonAdsGetError();

/**
 * Initializes the extension using the unique Application Key.
 * Must be called before calling any other function in this extension.
 *
 * @param appKey 32-character globally unique alphanumeric string used to identify your app.
 * @param enableTesting Flag to indicate whether or not ad requests are for testing (default: S3E_TRUE)
 * @param enableLogging Flag to enable logging from the API during testing (default: S3E_TRUE)
 *
 * @remarks Be sure to turn off testing and logging before publishing your application.
 *
 * @return
 * - #S3E_RESULT_SUCCESS if no error occurred.
 * - #S3E_RESULT_ERROR if an error has occurred. Use s3eAmazonAdsGetError to get the error code:
 *      #S3E_AMAZONADS_ERR_INVALID_APPKEY if the appKey has failed simple validation.
 *
 * @par Required Header Files
 * s3eAmazonAds.h
 */
s3eResult s3eAmazonAdsInit(const char* appKey, s3eBool enableTesting S3E_DEFAULT(S3E_TRUE), s3eBool enableLogging S3E_DEFAULT(S3E_TRUE));

/**
 * Terminates the extension and frees up resources.
 * Call this function to free all ad resources and invalidate all ids.
 *
 * @par Required Header Files
 * s3eAmazonAds.h
 */
s3eResult s3eAmazonAdsTerminate();

/**
 * Prepare an ad and obtain its ID prior to making an ad load request.
 * This function must be called to set up the resources and default options
 * required to load an ad.  The options can be modified by calling
 * s3eAmazonAdsPrepareLayout for banner ads and s3eAmazonAdsPrepareOptions
 * for setting targeting options.
 *
 * @param[out] pId Identifier to use for loading an ad and in ad callbacks
 *
 * @return
 * - #S3E_RESULT_SUCCESS if no error occurred.
 * - #S3E_RESULT_ERROR if an error has occurred.  Use s3eAmazonAdsGetError to get the error code:
 *      #S3E_AMAZONADS_ERR_NULL_PARAM if pId is NULL
 *      #S3E_AMAZONADS_ERR_INVALID_SIZE if the size is invalid
 *      #S3E_AMAZONADS_ERR_UNEXPECTED if there was an unexpected error creating ad resources
 */
s3eResult s3eAmazonAdsPrepareAd(s3eAmazonAdsId* pId);

/**
 * Prepare the layout of the ad.
 * This function can optionally be called to set the position and size of the
 * banner ad. The ad size must be a pre-defined size or auto-size. Custom
 * sizes are provided in case new size formats become available in future.
 *
 * Currently only top and bottom banner positions are supported.
 *
 * @param id Identifier returned by s3eAmazonAdsPrepareAd.
 * @param position Position of banner (does not apply to interstitial ads).
 * @param size Requested ad size enum (either predefined, auto or custom).
 * @param width Custom ad width if S3E_AMAZONADS_SIZE_CUSTOM size is passed.
 * @param height Custom ad height if S3E_AMAZONADS_SIZE_CUSTOM is passed.
 *
 * @remark Custom dimensions will result in ad load failure if the size is
 * not supported by Amazon Mobile Ad Network for the specific device.
 * @remark On android, auto size will stretch to fit the width of the view.
 *
 * @return
 * - #S3E_RESULT_SUCCESS if no error occurred.
 * - #S3E_RESULT_ERROR if an error has occurred. Use s3eAmazonAdsGetError to get the error code:
 *      #S3E_AMAZONADS_ERR_NULL_PARAM if id is NULL
 *      #S3E_AMAZONADS_ERR_INVALID_SIZE if the size is invalid
 *      #S3E_AMAZONADS_ERR_UNEXPECTED if there was an unexpected error creating ad resources
 */
s3eResult s3eAmazonAdsPrepareAdLayout(s3eAmazonAdsId id, s3eAmazonAdsPosition position S3E_DEFAULT(S3E_AMAZONADS_POSITION_TOP), s3eAmazonAdsSize size S3E_DEFAULT(S3E_AMAZONADS_SIZE_AUTO), int width S3E_DEFAULT(0), int height S3E_DEFAULT(0));

/**
 * Set ad targeting options prior to making an ad load request.
 * CAREFUL: Do not re-use the object returned by s3eAmazonAdsGetAdTargetingOptions when calling
 * this function.  Instead create your own s3eAmazonAdsTargetingOptions object.
 *
 * @param id Identifier returned by s3eAmazonAdsPrepareAd.
 * @param[in] options Ad targeting options.
 *
 * @remark This function will make a deep copy of the options so the object can be freed afterwards.
 * @remark Once loaded, these options are no longer modifiable.
 * @remark Default options apply if this is not called.
 *
 * @return
 * - #S3E_RESULT_SUCCESS if no error occurred.
 * - #S3E_RESULT_ERROR if an error has occurred. Use s3eAmazonAdsGetError to get the error code:
 *      #S3E_AMAZONADS_ERR_INVALID_OPTION if an option is invalid
 *      #S3E_AMAZONADS_ERR_NULL_PARAM if options is NULL
 *      #S3E_AMAZONADS_ERR_INVALID_ID if id is invalid
 */
s3eResult s3eAmazonAdsSetAdTargetingOptions(s3eAmazonAdsId id, s3eAmazonAdsTargetingOptions* options);

/**
 * Get targeting options for the given ad.
 * The object returned is only valid for the duration of the ad object, i.e. until you
 * call s3eAmazonAdsTerminate or s3eAmazonAdsDestroyAd. Do not delete or free the returned
 * object yourself.  Its lifetime is managed by this API.
 * CAREFUL: Do not use object returned in the call to s3eAmazonAdsSetAdTargetingOptions.
 * Instead create your own s3eAmazonAdsTargetingOptions object when setting.
 *
 * @param id Identifier returned by s3eAmazonAdsPrepareAd.
 *
 * @return
 * - Valid pointer to s3eAmazonAdsTargetingOptions if no error occurred.
 * - NULL if an error has occurred. Use s3eAmazonAdsGetError to get the error code:
 *      #S3E_AMAZONADS_ERR_INVALID_OPTION if an option is invalid
 *      #S3E_AMAZONADS_ERR_NULL_PARAM if options is NULL
 *      #S3E_AMAZONADS_ERR_INVALID_ID if id is invalid
 */
s3eAmazonAdsTargetingOptions* s3eAmazonAdsGetAdTargetingOptions(s3eAmazonAdsId id);

/**
 * Loads the given ad asynchronously and by default show it immediately.
 * If you do not want the ad to show immediately then it will be loaded behind the main view.
 * When calling s3eAmazonAdsShowAd the ad will then be brought to the front. The advantage of this is that
 * you can preload an ad, wait for the load callback and better time when to display it.
 *
 * Loading an already loaded ad will force a reload. This is useful when the orientation has changed
 * and it is up to the developer to listen for screen size changes in their app.
 *
 * @param id Identifier returned by s3eAmazonAdsPrepareAd that you want to load.
 * @param show Flag determining whether or not to show the ad immediately after loading (default: S3E_TRUE)
 * @param timeout Timeout value for the asynchronous load task that requests the ad (default: 20000 ms)
 *
 * @return
 * #S3E_RESULT_SUCCESS on success. Errors that occur during the asynchronous load task itself
 * will be passed via the S3E_AMAZONADS_AD_FAILED_TO_LOAD callback.
 * #S3E_RESULT_ERROR on error trying to initiate a load. Use s3eAmazonAdsGetError to get the error code:
 *      #S3E_AMAZONADS_ERR_INVALID_ID if id is invalid
 *
 * @remarks
 * On success, the user will receive the S3E_AMAZONADS_CALLBACK_AD_LOADED callback with
 * an s3eAmazonAdsId to identify the loaded ad.
 * On error, the user will receive the S3E_AMAZONADS_CALLBACK_AD_ERROR callback
 * with the error code.
 *
 * @par Required Header Files
 * s3eAmazonAds.h
 */
s3eResult s3eAmazonAdsLoadAd(s3eAmazonAdsId id, s3eBool show S3E_DEFAULT(S3E_TRUE), int timeout S3E_DEFAULT(20000));

/**
 * Destroys a loaded or prepared ad.
 * A loaded ad will have all its resources released and will be removed from the view hierarchy.
 * A prepared ad will have its identifier invalidated and its associated option data released.
 *
 * @param id Identifier returned by s3eAmazonAdsPrepareAd.
 *
 * @return
 * #S3E_RESULT_SUCCESS on successful release of ad resources.
 * #S3E_RESULT_ERROR on error. Use s3eAmazonAdsGetError to get the error code:
 *      #S3E_AMAZONADS_ERR_INVALID_ID if id is invalid
 *
 * @par Required Header Files
 * s3eAmazonAds.h
 */
s3eResult s3eAmazonAdsDestroyAd(s3eAmazonAdsId id);

/**
 * @deprecated Feature no longer supported by underlying library - will always return S3E_RESULT_ERROR
 */
s3eResult s3eAmazonAdsCollapseAd(s3eAmazonAdsId id);

/**
 * Loads an interstitial ad asynchronously given an s3eAmazonAdsId.
 *
 * @param id Identifier returned by s3eAmazonAdsPrepareAd.
 *
 * @return
 * - #S3E_RESULT_SUCCESS if no error occurred.
 * - #S3E_RESULT_ERROR if an error has occurred. Use s3eAmazonAdsGetError to get the error code:
 *      #S3E_AMAZONADS_ERR_INVALID_ID if id is invalid
 *
 * @remarks
 * On success, the user will receive the S3E_AMAZONADS_CALLBACK_AD_LOADED callback with
 * id returned to identify the loaded ad.
 * On error, the user will receive the S3E_AMAZONADS_CALLBACK_AD_FAILED_TO_LOAD callback
 * with the error code.
 *
 * @par Required Header Files
 * s3eAmazonAds.h
 */
s3eResult s3eAmazonAdsLoadInterstitialAd(s3eAmazonAdsId id);

/**
 * Shows a previously loaded ad given the s3eAmazonAdsId that identifies it.
 * This should only be called if s3eAmazonAdsLoadAd was called with show set to S3E_FALSE.
 *
 * @param id Identifier returned by s3eAmazonAdsPrepareAd.
 *
 * @return
 * - #S3E_RESULT_SUCCESS if no error occurred when trying to show the ad
 * - #S3E_RESULT_ERROR if an error has occurred. Use s3eAmazonAdsGetError to get the error code:
 *      #S3E_AMAZONADS_ERR_INVALID_ID if id is invalid
 *
 * @par Required Header Files
 * s3eAmazonAds.h
 */
s3eResult s3eAmazonAdsShowAd(s3eAmazonAdsId id);

/**
 * Determine the given ad's current state.
 * This function will determine if an ad is currently loading.
 *
 * @param id Identifier returned by s3eAmazonAdsPrepareAd.
 * @param[out] info s3eAmazonAdsInfo object provided by caller to store the returned state data.
 *
 * @return
 * - #S3E_RESULT_SUCCESS if no error occurred.
 * - #S3E_RESULT_ERROR if an error has occurred. Use s3eAmazonAdsGetError to get the error code:
 *      #S3E_AMAZONADS_ERR_NULL_PARAM if info is NULL
 *      #S3E_AMAZONADS_ERR_INVALID_ID if id is invalid
 */
s3eResult s3eAmazonAdsInspectAd(s3eAmazonAdsId id, s3eAmazonAdsAdInfo* info);

S3E_END_C_DECL

/** @} */

#endif /* !S3E_EXT_AMAZONADS_H */