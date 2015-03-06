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
java implementation of the s3eAmazonAds extension.
*/

package com.ideaworks3d.marmalade.s3eAmazonAds;

import com.ideaworks3d.marmalade.LoaderAPI;
import android.app.Activity;
import android.util.Log;
import java.util.HashMap;

import com.amazon.device.ads.*;
import com.amazon.device.ads.AdProperties.AdType;

public class s3eAmazonAdsListener extends DefaultAdListener
{
    // WARNING:
    // Be sure that the enums defined in the s4e match the order of the elements in the following arrays:
    public static AdProperties.AdType s3eAdTypeMap[] = { AdType.IMAGE_BANNER, AdType.MRAID_1, AdType.MRAID_2, AdType.INTERSTITIAL };
    public static AdError.ErrorCode s3eErrorMap[] = {
        AdError.ErrorCode.NETWORK_ERROR, AdError.ErrorCode.NETWORK_TIMEOUT, AdError.ErrorCode.NO_FILL,
        AdError.ErrorCode.INTERNAL_ERROR, AdError.ErrorCode.REQUEST_ERROR };

    public static int ConvertEnum(Enum[] arr, Enum val)
    {
        for (int i = 0; i < arr.length; i++)
        {
            if (arr[i] == val)
                return i;
        }
        return -1;
    }

    // WARNING: update these enums to match s4e
    public final int S3E_AMAZONADS_ACTION_EXPANDED  = 1;
    public final int S3E_AMAZONADS_ACTION_COLLAPSED = 2;
    public final int S3E_AMAZONADS_ACTION_DISMISSED = 3;

    public class s3eAmazonAdsProperties
    {
        public s3eAmazonAdsProperties(AdProperties adProperties)
        {
            m_CanExpand = adProperties.canExpand();
            m_CanPlayAudio = adProperties.canPlayAudio();
            m_CanPlayVideo = adProperties.canPlayVideo();
            m_AdType = ConvertEnum(s3eAdTypeMap, adProperties.getAdType());
        }
        public boolean m_CanExpand;
        public boolean m_CanPlayAudio;
        public boolean m_CanPlayVideo;
        public int m_AdType;
    }

    private static native void native_AdLoadedCallback(int id, s3eAmazonAdsProperties props);
    private static native void native_AdActionCallback(int id, int action);
    private static native void native_AdErrorCallback(int id, int errorcode);

    private static final String LOG_TAG = "s3eAmazonAds";
    private int m_Id;

    public s3eAmazonAdsListener(int id)
    {
        Log.i(LOG_TAG, "s3eAmazonAdsListener instance for id: " + id);
        m_Id = id;
    }

    /**
     * This event is called once an ad loads successfully.
     */
    @Override
    public void onAdLoaded(final Ad ad, final AdProperties adProperties)
    {
        Log.i(LOG_TAG, "onAdLoaded id: " + m_Id);
        Log.i(LOG_TAG, adProperties.getAdType().toString() + " ad loaded successfully.");

        s3eAmazonAdsProperties props = new s3eAmazonAdsProperties(adProperties);
        s3eAmazonAds.s3eAmazonAdsManager.setLoaded(m_Id, true);
        native_AdLoadedCallback(m_Id, props);
    }

    /**
     * This event is called if an ad fails to load.
     */
    @Override
    public void onAdFailedToLoad(final Ad ad, final AdError error)
    {
        Log.i(LOG_TAG, "onAdFailedToLoad id: " + m_Id);
        Log.w(LOG_TAG, "Ad failed to load. Code: " + error.getCode() + ", Message: " + error.getMessage());
        native_AdErrorCallback(m_Id, ConvertEnum(s3eErrorMap, error.getCode()));
    }

    /**
     * This event is called after a rich media ad expands.
     */
    @Override
    public void onAdExpanded(final Ad ad)
    {
        Log.i(LOG_TAG, "onAdExpanded id: " + m_Id);
        native_AdActionCallback(m_Id, S3E_AMAZONADS_ACTION_EXPANDED);
    }

    /**
     * This event is called after a rich media ad has collapsed from an
     * expanded state.
     */
    @Override
    public void onAdCollapsed(final Ad ad)
    {
        Log.i(LOG_TAG, "onAdCollapsed id: " + m_Id);
        native_AdActionCallback(m_Id, S3E_AMAZONADS_ACTION_COLLAPSED);
    }

    /**
     * This event is called after an interstitial ad has been dismissed.
     */
    @Override
    public void onAdDismissed(final Ad ad)
    {
        Log.d(this.LOG_TAG, "onAdDismissed: " + m_Id);
        native_AdActionCallback(m_Id, S3E_AMAZONADS_ACTION_DISMISSED);
    }
}
